#ifndef __MYPWM_H
#define __MYPWM_H

#include "stm32f4xx.h"

void PWM_Init_PA8(void);          // 初始化 PA8 = TIM1_CH1
void PWM_SetDuty(uint16_t us);    // 设置油门 (1000–2000us)

#endif
