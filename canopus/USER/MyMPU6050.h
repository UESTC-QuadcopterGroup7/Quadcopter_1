#ifndef MY_MPU6050_H
#define MY_MPU6050_H

#include "stm32f4xx.h"
#include "MyI2C.h"

#define MPU6050_I2C_ADDR              0x68
#define MPU6050_I2C_ADDR_WRITE        (MPU6050_I2C_ADDR << 1)
#define MPU6050_I2C_ADDR_READ         ((MPU6050_I2C_ADDR << 1) | 0x01)

#define MPU6050_SMPLRT_DIV            0x19
#define MPU6050_CONFIG                0x1A
#define MPU6050_GYRO_CONFIG           0x1B
#define MPU6050_ACCEL_CONFIG          0x1C
#define MPU6050_ACCEL_XOUT_H          0x3B
#define MPU6050_ACCEL_YOUT_H          0x3D
#define MPU6050_ACCEL_ZOUT_H          0x3F
#define MPU6050_TEMP_OUT_H            0x41
#define MPU6050_GYRO_XOUT_H           0x43
#define MPU6050_GYRO_YOUT_H           0x45
#define MPU6050_GYRO_ZOUT_H           0x47
#define MPU6050_PWR_MGMT_1            0x6B
#define MPU6050_PWR_MGMT_2            0x6C
#define MPU6050_INT_PIN_CFG           0x37
#define MPU6050_WHO_AM_I              0x75

#define MPU6050_INT_PIN_CFG_BYPASS    0x02

int MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
void MPU6050_GetData(int16_t *acc_x, int16_t *acc_y, int16_t *acc_z,
                     int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z,
                     int16_t *temperature);
void MPU6050_EnableBypass(void);
void MPU6050_DisableBypass(void);
float MPU6050_ConvertTemperature(int16_t raw_value);

#endif
