#include "MyBMP.h"
#include <string.h>
#include <math.h>

static int BMP280_WriteReg(uint8_t reg, uint8_t value);
static int BMP280_ReadMulti(uint8_t reg, uint8_t *buffer, uint8_t length);
static int BMP280_ReadCalibration(BMP280_HandleTypeDef *handle);
static int BMP280_ReadRawValues(int32_t *raw_press, int32_t *raw_temp);
static int32_t BMP280_CompensateTemperature(BMP280_HandleTypeDef *handle, int32_t raw_temp);
static uint32_t BMP280_CompensatePressure(BMP280_HandleTypeDef *handle, int32_t raw_press);

void BMP280_HandleInit(BMP280_HandleTypeDef *handle)
{
    if (handle == NULL) {
        return;
    }

    memset(handle, 0, sizeof(BMP280_HandleTypeDef));
}

int BMP280_ReadChipID(uint8_t *chip_id)
{
    if (chip_id == NULL) {
        return -1;
    }

    uint8_t id = 0;
    if (BMP280_ReadMulti(BMP280_REG_ID, &id, 1) != 0) {
        return -2;
    }

    *chip_id = id;
    return 0;
}

int BMP280_Init(BMP280_HandleTypeDef *handle)
{
    if (handle == NULL) {
        return -1;
    }

    BMP280_HandleInit(handle);

    MyI2C_Init();
    Delay_ms(10);

    uint8_t chip_id = 0;
    if (BMP280_ReadChipID(&chip_id) != 0 || chip_id != BMP280_CHIP_ID) {
        return -2;
    }

    if (BMP280_WriteReg(BMP280_REG_RESET, BMP280_SOFT_RESET_VALUE) != 0) {
        return -3;
    }
    Delay_ms(10);

    if (BMP280_ReadCalibration(handle) != 0) {
        return -4;
    }

    if (BMP280_WriteReg(BMP280_REG_CTRL_MEAS, BMP280_DEFAULT_OVERSAMPLING) != 0) {
        return -5;
    }

    if (BMP280_WriteReg(BMP280_REG_CONFIG, BMP280_DEFAULT_CONFIG) != 0) {
        return -6;
    }

    handle->initialized = 1;
    return 0;
}

int BMP280_ReadTemperaturePressure(BMP280_HandleTypeDef *handle,
                                   float *temperature_c,
                                   float *pressure_pa)
{
    if (handle == NULL || handle->initialized == 0) {
        return -1;
    }

    int32_t raw_press = 0;
    int32_t raw_temp = 0;
    if (BMP280_ReadRawValues(&raw_press, &raw_temp) != 0) {
        return -2;
    }

    int32_t temp_x100 = BMP280_CompensateTemperature(handle, raw_temp);
    uint32_t pressure_q24 = BMP280_CompensatePressure(handle, raw_press);

    if (temperature_c != NULL) {
        *temperature_c = temp_x100 / 100.0f;
    }

    if (pressure_pa != NULL) {
        *pressure_pa = pressure_q24 / 256.0f;
    }

    return 0;
}

float BMP280_CalcAltitude(float pressure_pa, float sea_level_pa)
{
    if (pressure_pa <= 0.0f || sea_level_pa <= 0.0f) {
        return 0.0f;
    }

    float ratio = pressure_pa / sea_level_pa;
    return 44330.0f * (1.0f - powf(ratio, 0.19029495f));
}

static int BMP280_WriteReg(uint8_t reg, uint8_t value)
{
    MyI2C_Start();
    MyI2C_SendByte(BMP280_I2C_ADDRESS);
    if (MyI2C_ReceiveAck() != 0) {
        MyI2C_Stop();
        return -1;
    }

    MyI2C_SendByte(reg);
    if (MyI2C_ReceiveAck() != 0) {
        MyI2C_Stop();
        return -2;
    }

    MyI2C_SendByte(value);
    if (MyI2C_ReceiveAck() != 0) {
        MyI2C_Stop();
        return -3;
    }

    MyI2C_Stop();
    return 0;
}

static int BMP280_ReadMulti(uint8_t reg, uint8_t *buffer, uint8_t length)
{
    if (buffer == NULL || length == 0) {
        return -1;
    }

    MyI2C_Start();
    MyI2C_SendByte(BMP280_I2C_ADDRESS);
    if (MyI2C_ReceiveAck() != 0) {
        MyI2C_Stop();
        return -2;
    }

    MyI2C_SendByte(reg);
    if (MyI2C_ReceiveAck() != 0) {
        MyI2C_Stop();
        return -3;
    }

    MyI2C_Start();
    MyI2C_SendByte(BMP280_I2C_ADDRESS | 0x01U);
    if (MyI2C_ReceiveAck() != 0) {
        MyI2C_Stop();
        return -4;
    }

    for (uint8_t i = 0; i < length; i++) {
        buffer[i] = MyI2C_ReceiveByte();
        MyI2C_SendAck((i == (length - 1)) ? 1 : 0);
    }

    MyI2C_Stop();
    return 0;
}

static int BMP280_ReadCalibration(BMP280_HandleTypeDef *handle)
{
    uint8_t calib_buffer[BMP280_CALIB_DATA_LEN];
    if (BMP280_ReadMulti(BMP280_REG_CALIB_START, calib_buffer, sizeof(calib_buffer)) != 0) {
        return -1;
    }

    handle->calib.dig_T1 = (uint16_t)(calib_buffer[1] << 8 | calib_buffer[0]);
    handle->calib.dig_T2 = (int16_t)(calib_buffer[3] << 8 | calib_buffer[2]);
    handle->calib.dig_T3 = (int16_t)(calib_buffer[5] << 8 | calib_buffer[4]);
    handle->calib.dig_P1 = (uint16_t)(calib_buffer[7] << 8 | calib_buffer[6]);
    handle->calib.dig_P2 = (int16_t)(calib_buffer[9] << 8 | calib_buffer[8]);
    handle->calib.dig_P3 = (int16_t)(calib_buffer[11] << 8 | calib_buffer[10]);
    handle->calib.dig_P4 = (int16_t)(calib_buffer[13] << 8 | calib_buffer[12]);
    handle->calib.dig_P5 = (int16_t)(calib_buffer[15] << 8 | calib_buffer[14]);
    handle->calib.dig_P6 = (int16_t)(calib_buffer[17] << 8 | calib_buffer[16]);
    handle->calib.dig_P7 = (int16_t)(calib_buffer[19] << 8 | calib_buffer[18]);
    handle->calib.dig_P8 = (int16_t)(calib_buffer[21] << 8 | calib_buffer[20]);
    handle->calib.dig_P9 = (int16_t)(calib_buffer[23] << 8 | calib_buffer[22]);

    return 0;
}

static int BMP280_ReadRawValues(int32_t *raw_press, int32_t *raw_temp)
{
    uint8_t buffer[6];
    if (BMP280_ReadMulti(BMP280_REG_PRESS_MSB, buffer, sizeof(buffer)) != 0) {
        return -1;
    }

    int32_t press = (int32_t)((((uint32_t)buffer[0]) << 12) |
                              (((uint32_t)buffer[1]) << 4) |
                              ((uint32_t)buffer[2] >> 4));

    int32_t temp = (int32_t)((((uint32_t)buffer[3]) << 12) |
                             (((uint32_t)buffer[4]) << 4) |
                             ((uint32_t)buffer[5] >> 4));

    if (raw_press != NULL) {
        *raw_press = press;
    }

    if (raw_temp != NULL) {
        *raw_temp = temp;
    }

    return 0;
}

static int32_t BMP280_CompensateTemperature(BMP280_HandleTypeDef *handle, int32_t raw_temp)
{
    int32_t var1, var2;

    var1 = ((((raw_temp >> 3) - ((int32_t)handle->calib.dig_T1 << 1))) *
            ((int32_t)handle->calib.dig_T2)) >> 11;

    var2 = (((((raw_temp >> 4) - (int32_t)handle->calib.dig_T1) *
             ((raw_temp >> 4) - (int32_t)handle->calib.dig_T1)) >> 12) *
            (int32_t)handle->calib.dig_T3) >> 14;

    handle->t_fine = var1 + var2;
    return (handle->t_fine * 5 + 128) >> 8;
}

static uint32_t BMP280_CompensatePressure(BMP280_HandleTypeDef *handle, int32_t raw_press)
{
    int64_t var1, var2, pressure;

    var1 = ((int64_t)handle->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)handle->calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)handle->calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)handle->calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)handle->calib.dig_P3) >> 8) +
           ((var1 * (int64_t)handle->calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1) * (int64_t)handle->calib.dig_P1) >> 33;

    if (var1 == 0) {
        return 0;
    }

    pressure = 1048576 - raw_press;
    pressure = (((pressure << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)handle->calib.dig_P9) * (pressure >> 13) * (pressure >> 13)) >> 25;
    var2 = (((int64_t)handle->calib.dig_P8) * pressure) >> 19;
    pressure = ((pressure + var1 + var2) >> 8) + (((int64_t)handle->calib.dig_P7) << 4);

    return (uint32_t)pressure;
}
