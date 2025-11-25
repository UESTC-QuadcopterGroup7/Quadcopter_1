#include "MyBMP.h"
#include "Delay.h"
#include <math.h>
#include <stddef.h>

static BMP180_CalibData_t g_calib_data;
static uint8_t g_calib_ready = 0;

static void BMP180_WriteReg(uint8_t reg, uint8_t data)
{
    MyI2C_Start();
    MyI2C_SendByte(BMP180_WRITE_ADDR);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(data);
    MyI2C_ReceiveAck();
    MyI2C_Stop();
}

static int BMP180_ReadRegs(uint8_t reg, uint8_t *buffer, uint8_t length)
{
    if (buffer == NULL || length == 0)
    {
        return -1;
    }

    MyI2C_Start();
    MyI2C_SendByte(BMP180_WRITE_ADDR);
    if (MyI2C_ReceiveAck())
    {
        MyI2C_Stop();
        return -1;
    }

    MyI2C_SendByte(reg);
    if (MyI2C_ReceiveAck())
    {
        MyI2C_Stop();
        return -1;
    }

    MyI2C_Start();
    MyI2C_SendByte(BMP180_READ_ADDR);
    if (MyI2C_ReceiveAck())
    {
        MyI2C_Stop();
        return -1;
    }

    while (length--)
    {
        *buffer++ = MyI2C_ReceiveByte();
        MyI2C_SendAck(length == 0);
    }

    MyI2C_Stop();
    return 0;
}

static int32_t BMP180_ReadUncompensatedTemperature(void)
{
    uint8_t data[2];

    BMP180_WriteReg(BMP180_REG_CTRL_MEAS, BMP180_CMD_TEMP);
    Delay_ms(5);

    if (BMP180_ReadRegs(BMP180_REG_OUT_MSB, data, 2) != 0)
    {
        return 0;
    }

    return ((int32_t)data[0] << 8) | data[1];
}

static int32_t BMP180_ReadUncompensatedPressure(BMP180_OSS_t oss)
{
    uint8_t data[3];
    uint8_t cmd;

    switch (oss)
    {
    case BMP180_OSS_ULTRA_LOW_POWER:
        cmd = BMP180_CMD_PRESSURE_0;
        break;
    case BMP180_OSS_STANDARD:
        cmd = BMP180_CMD_PRESSURE_1;
        break;
    case BMP180_OSS_HIGH_RES:
        cmd = BMP180_CMD_PRESSURE_2;
        break;
    case BMP180_OSS_ULTRA_HIGH_RES:
    default:
        cmd = BMP180_CMD_PRESSURE_3;
        break;
    }

    BMP180_WriteReg(BMP180_REG_CTRL_MEAS, cmd);

    switch (oss)
    {
    case BMP180_OSS_ULTRA_LOW_POWER:
        Delay_ms(5);
        break;
    case BMP180_OSS_STANDARD:
        Delay_ms(8);
        break;
    case BMP180_OSS_HIGH_RES:
        Delay_ms(14);
        break;
    case BMP180_OSS_ULTRA_HIGH_RES:
    default:
        Delay_ms(26);
        break;
    }

    if (BMP180_ReadRegs(BMP180_REG_OUT_MSB, data, 3) != 0)
    {
        return 0;
    }

    int32_t up = ((int32_t)data[0] << 16) | ((int32_t)data[1] << 8) | data[2];
    up >>= (8 - oss);
    return up;
}

static int BMP180_ReadCalibration(BMP180_CalibData_t *calib)
{
    uint8_t buffer[22];

    if (calib == NULL)
    {
        return -1;
    }

    if (BMP180_ReadRegs(BMP180_REG_AC1_MSB, buffer, sizeof(buffer)) != 0)
    {
        return -1;
    }

    calib->AC1 = (int16_t)((buffer[0] << 8) | buffer[1]);
    calib->AC2 = (int16_t)((buffer[2] << 8) | buffer[3]);
    calib->AC3 = (int16_t)((buffer[4] << 8) | buffer[5]);
    calib->AC4 = (uint16_t)((buffer[6] << 8) | buffer[7]);
    calib->AC5 = (uint16_t)((buffer[8] << 8) | buffer[9]);
    calib->AC6 = (uint16_t)((buffer[10] << 8) | buffer[11]);
    calib->B1  = (int16_t)((buffer[12] << 8) | buffer[13]);
    calib->B2  = (int16_t)((buffer[14] << 8) | buffer[15]);
    calib->MB  = (int16_t)((buffer[16] << 8) | buffer[17]);
    calib->MC  = (int16_t)((buffer[18] << 8) | buffer[19]);
    calib->MD  = (int16_t)((buffer[20] << 8) | buffer[21]);

    return 0;
}

static float BMP180_CalcTemperature(int32_t ut, int32_t *b5_out)
{
    int32_t x1 = ((ut - (int32_t)g_calib_data.AC6) * (int32_t)g_calib_data.AC5) >> 15;
    int32_t x2 = ((int32_t)g_calib_data.MC << 11) / (x1 + g_calib_data.MD);
    int32_t b5 = x1 + x2;

    if (b5_out != NULL)
    {
        *b5_out = b5;
    }

    return ((b5 + 8) >> 4) / 10.0f;
}

static float BMP180_CalcPressure(int32_t up, int32_t b5, BMP180_OSS_t oss)
{
    int32_t b6 = b5 - 4000;
    int32_t x1 = ((int32_t)g_calib_data.B2 * ((b6 * b6) >> 12)) >> 11;
    int32_t x2 = ((int32_t)g_calib_data.AC2 * b6) >> 11;
    int32_t x3 = x1 + x2;
    int32_t b3 = ((((int32_t)g_calib_data.AC1 * 4 + x3) << oss) + 2) >> 2;
    x1 = ((int32_t)g_calib_data.AC3 * b6) >> 13;
    x2 = ((int32_t)g_calib_data.B1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    uint32_t b4 = ((uint32_t)g_calib_data.AC4 * (uint32_t)(x3 + 32768)) >> 15;
    uint32_t b7 = ((uint32_t)up - (uint32_t)b3) * (uint32_t)(50000 >> oss);

    int32_t p;
    if (b7 < 0x80000000)
    {
        p = (int32_t)((b7 << 1) / b4);
    }
    else
    {
        p = (int32_t)((b7 / b4) << 1);
    }

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    p = p + ((x1 + x2 + 3791) >> 4);

    return (float)p;
}

BMP180_Status BMP180_Init(void)
{
    if (BMP180_ReadCalibration(&g_calib_data) != 0)
    {
        g_calib_ready = 0;
        return BMP180_STATUS_ERROR;
    }

    g_calib_ready = 1;
    return BMP180_STATUS_OK;
}

BMP180_Status BMP180_GetData(BMP180_Data_t *data, BMP180_OSS_t oss)
{
    if (!g_calib_ready || data == NULL)
    {
        return BMP180_STATUS_ERROR;
    }

    int32_t ut = BMP180_ReadUncompensatedTemperature();
    int32_t up = BMP180_ReadUncompensatedPressure(oss);

    int32_t b5 = 0;
    data->temperature = BMP180_CalcTemperature(ut, &b5);
    data->pressure = BMP180_CalcPressure(up, b5, oss);
    data->altitude = BMP180_CalculateAltitude(data->pressure, 101325.0f);

    return BMP180_STATUS_OK;
}

float BMP180_CalculateAltitude(float pressure, float sea_level_pressure)
{
    if (pressure <= 0.0f || sea_level_pressure <= 0.0f)
    {
        return 0.0f;
    }

    float ratio = pressure / sea_level_pressure;
    return 44330.0f * (1.0f - powf(ratio, 0.1903f));
}
