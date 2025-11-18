/**
  ******************************************************************************
  * @file    MyMPU6050.h
  * @brief   Minimal MPU6050 driver based on software I2C
  ******************************************************************************
  */

#ifndef _MYMPU6050_H
#define _MYMPU6050_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "MyI2C.h"
#include "MPU6050.h"

int MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
void MPU6050_GetData(int16_t *AccX,
                     int16_t *AccY,
                     int16_t *AccZ,
                     int16_t *GyroX,
                     int16_t *GyroY,
                     int16_t *GyroZ);
void MPU6050_EnableBypass(void);
void MPU6050_DisableBypass(void);

#ifdef __cplusplus
}
#endif

#endif /* _MYMPU6050_H */
