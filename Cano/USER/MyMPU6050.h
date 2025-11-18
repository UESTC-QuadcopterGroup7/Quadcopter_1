/**
  ******************************************************************************
  * @file    MyMPU6050.h
  * @author  Your Name
  * @version V1.0
  * @brief   微秒级延时头文件 for STM32F401RE
  ******************************************************************************
  */

#ifndef _MyMPU6050_H
#define _MyMPU6050_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "MyI2C.h"
//#include "MPU6050.h"
/*
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);

uint8_t MPU6050_ReadReg(uint8_t RegAddress);

int MPU6050_Init(void);

uint8_t MPU6050_GetID(void);

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);

void MPU6050_EnableBypass();

void MPU6050_DisableBypass();
*/

// 传感器数据变量
int16_t AccX, AccY, AccZ;
int16_t GyroX, GyroY, GyroZ;
int16_t Temperature;

// MPU6050地址和寄存器定义
#define MPU6050_ADDRESS 0xD0
#define MPU6050_WHO_AM_I 0x75
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_CONFIG 0x1A
#define MPU6050_GYRO_CONFIG 0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_TEMP_OUT_H 0x41
#define MPU6050_GYRO_XOUT_H 0x43


void MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
void MPU6050_GetData(int16_t* AccX, int16_t* AccY, int16_t* AccZ, 
                     int16_t* GyroX, int16_t* GyroY, int16_t* GyroZ, 
                     int16_t* Temp);

#ifdef __cplusplus
}
#endif

#endif /**/