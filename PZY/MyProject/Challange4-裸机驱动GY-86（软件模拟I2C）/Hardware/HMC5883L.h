#ifndef __HMC5883L_H
#define __HMC5883L_H

#include "stm32f4xx.h"
void HMC5883L_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t HMC5883L_ReadReg(uint8_t RegAddress);

/* 初始化HMC5883L磁强计 */
void HMC5883L_Init(void);

/* 获取三轴磁场数据
 * MagX：X轴磁场数据（单位：mG，毫高斯）
 * MagY：Y轴磁场数据（单位：mG）
 * MagZ：Z轴磁场数据（单位：mG）
 */
void HMC5883L_GetData(int16_t *MagX, int16_t *MagY, int16_t *MagZ);

/* 获取HMC5883L的ID（3字节组合，默认0x483433） */
uint32_t HMC5883L_GetID(void);

#endif