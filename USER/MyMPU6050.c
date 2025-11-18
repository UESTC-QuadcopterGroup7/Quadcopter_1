#include "MyMPU6050.h"

static void MPU6050_WriteReg(uint8_t reg_address, uint8_t data)
{
    MyI2C_Start();
    MyI2C_SendByte(MPU6050_I2C_ADDR_WRITE);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg_address);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(data);
    MyI2C_ReceiveAck();
    MyI2C_Stop();
}

static uint8_t MPU6050_ReadReg(uint8_t reg_address)
{
    uint8_t data;

    MyI2C_Start();
    MyI2C_SendByte(MPU6050_I2C_ADDR_WRITE);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg_address);
    MyI2C_ReceiveAck();

    MyI2C_Start();
    MyI2C_SendByte(MPU6050_I2C_ADDR_READ);
    MyI2C_ReceiveAck();
    data = MyI2C_ReceiveByte();
    MyI2C_SendAck(1);
    MyI2C_Stop();

    return data;
}

int MPU6050_Init(void)
{
    uint8_t device_id;

    MyI2C_Init();
    device_id = MPU6050_GetID();
    if (device_id != MPU6050_I2C_ADDR) {
        return -1;
    }

    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
    MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);

    device_id = MPU6050_GetID();
    if (device_id != MPU6050_I2C_ADDR) {
        return -2;
    }

    return 0;
}

uint8_t MPU6050_GetID(void)
{
    return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(int16_t *acc_x, int16_t *acc_y, int16_t *acc_z,
                     int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z,
                     int16_t *temperature)
{
    uint8_t data_h, data_l;

    data_h = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
    data_l = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H + 1);
    if (acc_x) { *acc_x = (data_h << 8) | data_l; }

    data_h = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
    data_l = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H + 1);
    if (acc_y) { *acc_y = (data_h << 8) | data_l; }

    data_h = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
    data_l = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H + 1);
    if (acc_z) { *acc_z = (data_h << 8) | data_l; }

    data_h = MPU6050_ReadReg(MPU6050_TEMP_OUT_H);
    data_l = MPU6050_ReadReg(MPU6050_TEMP_OUT_H + 1);
    if (temperature) { *temperature = (data_h << 8) | data_l; }

    data_h = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
    data_l = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H + 1);
    if (gyro_x) { *gyro_x = (data_h << 8) | data_l; }

    data_h = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
    data_l = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H + 1);
    if (gyro_y) { *gyro_y = (data_h << 8) | data_l; }

    data_h = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
    data_l = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H + 1);
    if (gyro_z) { *gyro_z = (data_h << 8) | data_l; }
}

void MPU6050_EnableBypass(void)
{
    uint8_t reg_value = MPU6050_ReadReg(MPU6050_INT_PIN_CFG);
    reg_value |= MPU6050_INT_PIN_CFG_BYPASS;
    MPU6050_WriteReg(MPU6050_INT_PIN_CFG, reg_value);
}

void MPU6050_DisableBypass(void)
{
    uint8_t reg_value = MPU6050_ReadReg(MPU6050_INT_PIN_CFG);
    reg_value &= (uint8_t)(~MPU6050_INT_PIN_CFG_BYPASS);
    MPU6050_WriteReg(MPU6050_INT_PIN_CFG, reg_value);
}

float MPU6050_ConvertTemperature(int16_t raw_value)
{
    return (raw_value / 340.0f) + 36.53f;
}
