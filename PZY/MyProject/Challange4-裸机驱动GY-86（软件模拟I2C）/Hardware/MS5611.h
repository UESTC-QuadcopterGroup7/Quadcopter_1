#ifndef __MS5611_H
#define __MS5611_H

#include "stm32f4xx.h"

/* 初始化MS5611传感器 */
void MS5611_Init(void);
uint16_t MS5611_ReadPROM(uint8_t PromAddr);
/* 获取气压和温度数据
 * Pressure：气压数据（单位：0.01hPa）
 * Temperature：温度数据（单位：0.01℃）
 */
void MS5611_GetData(int32_t *Pressure, int32_t *Temperature);

#endif