#ifndef MY_HMC5883L_H
#define MY_HMC5883L_H

#include "stm32f4xx.h"

#define HMC5883L_I2C_ADDR_WRITE   0x3C
#define HMC5883L_I2C_ADDR_READ    0x3D

#define HMC5883L_REG_CONFIG_A     0x00
#define HMC5883L_REG_CONFIG_B     0x01
#define HMC5883L_REG_MODE         0x02
#define HMC5883L_REG_DATA_X_MSB   0x03
#define HMC5883L_REG_ID_A         0x0A
#define HMC5883L_REG_ID_B         0x0B
#define HMC5883L_REG_ID_C         0x0C

int HMC5883L_Init(void);
void HMC5883L_GetData(int16_t *mag_x, int16_t *mag_y, int16_t *mag_z);

#endif
