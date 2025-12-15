#ifndef __MYONESHOT_H
#define __MYONESHOT_H

#include "stm32f4xx.h"

/* 电机编号 */
typedef enum {
    MOTOR1 = 0,   // TIM1_CH1 = PA8
    MOTOR2,       // TIM1_CH2 = PA9
    MOTOR3,       // TIM1_CH3 = PA10
    MOTOR4        // TIM1_CH4 = PA11
} Motor_t;

/* 初始化（TIM1 + PA8~PA11） */
void OneShot_Init(void);

/* 发送 throttle (1000–2000 μs) */
void OneShot_Write(Motor_t motor, uint16_t throttle_us);

/* 4 路一起写（可选） */
void OneShot_WriteAll(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);

#endif
