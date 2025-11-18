#include "MyHMC5883L.h"
#include "Delay.h"

#define HMC5883L_WRITE_ADDR   0x3C
#define HMC5883L_READ_ADDR    0x3D

static void HMC5883L_WriteReg(uint8_t reg, uint8_t data)
{
    MyI2C_Start();
    MyI2C_SendByte(HMC5883L_WRITE_ADDR);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(data);
    MyI2C_ReceiveAck();
    MyI2C_Stop();
}

static uint8_t HMC5883L_ReadReg(uint8_t reg)
{
    uint8_t data;

    MyI2C_Start();
    MyI2C_SendByte(HMC5883L_WRITE_ADDR);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg);
    MyI2C_ReceiveAck();

    MyI2C_Start();
    MyI2C_SendByte(HMC5883L_READ_ADDR);
    MyI2C_ReceiveAck();
    data = MyI2C_ReceiveByte();
    MyI2C_SendAck(1);
    MyI2C_Stop();

    return data;
}

static void HMC5883L_ReadRegs(uint8_t reg, uint8_t *buffer, uint8_t length)
{
    if (buffer == NULL || length == 0)
    {
        return;
    }

    MyI2C_Start();
    MyI2C_SendByte(HMC5883L_WRITE_ADDR);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg);
    MyI2C_ReceiveAck();

    MyI2C_Start();
    MyI2C_SendByte(HMC5883L_READ_ADDR);
    MyI2C_ReceiveAck();

    while (length--)
    {
        *buffer++ = MyI2C_ReceiveByte();
        MyI2C_SendAck(length == 0);
    }

    MyI2C_Stop();
}

void HMC5883L_Init(void)
{
    MPU6050_EnableBypass();
    HMC5883L_WriteReg(HMC5883L_CONFIG_A, HMC5883L_CRA_8AVG_15HZ);
    HMC5883L_WriteReg(HMC5883L_CONFIG_B, HMC5883L_CRB_GAIN_1_3G);
    HMC5883L_WriteReg(HMC5883L_MODE, HMC5883L_MODE_CONT);
    Delay_ms(10);
    MPU6050_DisableBypass();
}

uint8_t HMC5883L_GetID(void)
{
    uint8_t id[3] = {0};

    MPU6050_EnableBypass();
    HMC5883L_ReadRegs(HMC5883L_ID_A, id, 3);
    MPU6050_DisableBypass();

    return id[0];
}

void HMC5883L_GetData(int16_t *MagX, int16_t *MagY, int16_t *MagZ)
{
    uint8_t buffer[6];

    if (MagX == NULL || MagY == NULL || MagZ == NULL)
    {
        return;
    }

    MPU6050_EnableBypass();
    HMC5883L_ReadRegs(HMC5883L_DATA_X_MSB, buffer, 6);
    MPU6050_DisableBypass();

    *MagX = (int16_t)((buffer[0] << 8) | buffer[1]);
    *MagZ = (int16_t)((buffer[2] << 8) | buffer[3]);
    *MagY = (int16_t)((buffer[4] << 8) | buffer[5]);
}
