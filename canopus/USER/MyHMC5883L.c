#include "MyHMC5883L.h"
#include "MyI2C.h"
#include "MyMPU6050.h"

static void HMC5883L_WriteReg(uint8_t reg_address, uint8_t data)
{
    MyI2C_Start();
    MyI2C_SendByte(HMC5883L_I2C_ADDR_WRITE);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg_address);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(data);
    MyI2C_ReceiveAck();
    MyI2C_Stop();
}

static uint8_t HMC5883L_ReadReg(uint8_t reg_address)
{
    uint8_t data;

    MyI2C_Start();
    MyI2C_SendByte(HMC5883L_I2C_ADDR_WRITE);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg_address);
    MyI2C_ReceiveAck();

    MyI2C_Start();
    MyI2C_SendByte(HMC5883L_I2C_ADDR_READ);
    MyI2C_ReceiveAck();
    data = MyI2C_ReceiveByte();
    MyI2C_SendAck(1);
    MyI2C_Stop();

    return data;
}

int HMC5883L_Init(void)
{
    MPU6050_EnableBypass();

    HMC5883L_WriteReg(HMC5883L_REG_CONFIG_A, 0x78);
    HMC5883L_WriteReg(HMC5883L_REG_CONFIG_B, 0xA0);
    HMC5883L_WriteReg(HMC5883L_REG_MODE, 0x00);

    if (HMC5883L_ReadReg(HMC5883L_REG_ID_A) != 'H' ||
        HMC5883L_ReadReg(HMC5883L_REG_ID_B) != '4' ||
        HMC5883L_ReadReg(HMC5883L_REG_ID_C) != '3') {
        MPU6050_DisableBypass();
        return -1;
    }

    MPU6050_DisableBypass();
    return 0;
}

void HMC5883L_GetData(int16_t *mag_x, int16_t *mag_y, int16_t *mag_z)
{
    uint8_t buffer[6];

    MPU6050_EnableBypass();
    for (uint8_t i = 0; i < 6; i++) {
        buffer[i] = HMC5883L_ReadReg(HMC5883L_REG_DATA_X_MSB + i);
    }
    MPU6050_DisableBypass();

    if (mag_x) { *mag_x = (buffer[0] << 8) | buffer[1]; }
    if (mag_z) { *mag_z = (buffer[2] << 8) | buffer[3]; }
    if (mag_y) { *mag_y = (buffer[4] << 8) | buffer[5]; }
}
