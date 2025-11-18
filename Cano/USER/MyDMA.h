/**
  ******************************************************************************
  * @file    
  * @author  Your Name
  * @version V1.0
  * @brief   微秒级延时头文件 for STM32F401RE
  ******************************************************************************
  */

#ifndef __MYDMA_H
#define __MYDMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"

#define AUDIO_BUFFER_SIZE 512
uint16_t audio_buffer[AUDIO_BUFFER_SIZE];

void DMA_init(void);

void DMA_Start(void);

#ifdef __cplusplus
}
#endif

#endif /*  */