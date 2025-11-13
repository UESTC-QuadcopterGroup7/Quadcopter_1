#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx.h"  // 标准库头文件

// 声明标准库的系统时钟变量（仅声明，不定义）
extern uint32_t SystemCoreClock;

/**
 * @brief  配置系统时钟为84MHz（最大频率）
 */
void RCC_Configuration(void);

/**
 * @brief  微秒级延时（基于SysTick）
 * @param  us：延时微秒数（最大约200,000us）
 */
void Delay_us(uint32_t us);

/**
 * @brief  毫秒级延时（基于微秒延时）
 * @param  ms：延时毫秒数（无上限）
 */
void Delay_ms(uint32_t ms);

#endif /* __DELAY_H */