/**
  ******************************************************************************
  * @file    MyHMC5883L.h
  * @brief   Driver for the HMC5883L magnetometer on the GY-87 board
  ******************************************************************************
  */

#ifndef __MYHMC5883L_H
#define __MYHMC5883L_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "MyI2C.h"
#include "MyMPU6050.h"
#include <stddef.h>

#define HMC5883L_CONFIG_A      0x00
#define HMC5883L_CONFIG_B      0x01
#define HMC5883L_MODE          0x02
#define HMC5883L_DATA_X_MSB    0x03
#define HMC5883L_DATA_X_LSB    0x04
#define HMC5883L_DATA_Z_MSB    0x05
#define HMC5883L_DATA_Z_LSB    0x06
#define HMC5883L_DATA_Y_MSB    0x07
#define HMC5883L_DATA_Y_LSB    0x08
#define HMC5883L_STATUS        0x09
#define HMC5883L_ID_A          0x0A

#define HMC5883L_CRA_8AVG_15HZ 0x70
#define HMC5883L_CRB_GAIN_1_3G 0x20
#define HMC5883L_MODE_CONT     0x00

void HMC5883L_Init(void);
uint8_t HMC5883L_GetID(void);
void HMC5883L_GetData(int16_t *MagX, int16_t *MagY, int16_t *MagZ);

#ifdef __cplusplus
}
#endif

#endif /* __MYHMC5883L_H */
