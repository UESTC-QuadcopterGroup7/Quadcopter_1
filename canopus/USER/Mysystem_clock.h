/**
  ******************************************************************************
  * @file    
  * @author  Your Name
  * @version V1.0
  * @brief   微秒级延时头文件 for STM32F401RE
  ******************************************************************************
  */

#ifndef __MYSYSTEM_CLOCK_H
#define __MYSYSTEM_CLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

void SystemClock_Config_HSI(void);//使用HSI

void SystemClock_Config_BT(void);//使用HSE

#define SIN_TABLE_SIZE 361

extern const float sin_table[SIN_TABLE_SIZE];

#include "stm32f4xx.h"

#ifdef __cplusplus
}
#endif

#endif /*  */