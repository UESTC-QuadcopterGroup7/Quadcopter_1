#ifndef __DSHOT_H
#define __DSHOT_H

#include "stm32f4xx.h"

/* ---------------- DShot 参数配置 ---------------- */

#define DSHOT_TIMER_CLOCK     84000000UL   // TIM1, TIM3 时钟 84MHz
#define DSHOT600_RATE         600000UL     // 600kbit/s

// 每 bit 的周期计数值（84MHz / 600kHz = 140）
#define DSHOT_PERIOD_TICKS    (DSHOT_TIMER_CLOCK / DSHOT600_RATE)   // 140

// 0/1 的高电平占空比（整数近似）
#define DSHOT_BIT0_TICKS      52    // 大约 0.375 * 140
#define DSHOT_BIT1_TICKS      105   // 大约 0.75  * 140

// 帧长度：16位（11位数据+4位校验+1位 telemetry）+ 1 个收尾低电平
#define DSHOT_FRAME_BITS      16
#define DSHOT_FRAME_LEN       (DSHOT_FRAME_BITS + 1)   // 17

// 通道枚举，方便调用接口
typedef enum {
    DSHOT_M1 = 0,  // TIM1_CH1 (PA8)
    DSHOT_M2,      // TIM1_CH2 (PA9)
    DSHOT_M3,      // TIM1_CH3 (PA10)
    DSHOT_M4       // TIM3_CH3 (PB0)
} DShot_Motor_t;

/* ---------------- 对外接口 ---------------- */

void DShot_Init_All(void);

/**
 * 发送一个 DShot 值（0~2047），telemetry 先固定为 0
 * motor: 选哪个电机（DSHOT_M1~DSHOT_M4）
 * value: 油门值（通常 48~2000），小于 48 一般认为是“特殊命令”
 */
void DShot_Send(DShot_Motor_t motor, uint16_t value);

/**
 * 同时给 4 个电机发一帧（其实内部是顺序发的，简单易理解）
 */
void DShot_Send_All(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);

#endif /* __DSHOT_H */
