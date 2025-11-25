/**
  ******************************************************************************
  * @file    
  * @author  Your Name
  * @version V1.0
  * @brief   微秒级延时头文件 for STM32F401RE
  ******************************************************************************
  */

#ifndef __MYI2S_H
#define __MYI2S_H

#ifdef __cplusplus
extern "C" {
#endif

void I2S2_Init(void);

void I2S2_Start(void);

#include "stm32f4xx.h"



#ifdef __cplusplus
}
#endif

#endif /*  */