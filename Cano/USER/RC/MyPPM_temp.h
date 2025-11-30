#ifndef __MYPPM_TEMP_H
#define __MYPPM_TEMP_H

#include "stm32f4xx.h"
#include <stdint.h>

/* ============================
   PPM 配置（可根据需要调整）
   ============================ */

// PPM 对应的定时器 与 引脚
// 使用 TIM2_CH1 = PA0 （推荐）
// 定时器时钟：84MHz
// 我们设置 PSC=83 → 1MHz，即 1us 计数分辨率

#define PPM_MAX_CHANNELS    8        // Radiolink R8EF 支持 8 通道
#define PPM_SYNC_US         3000     // 超过此值判定为同步帧（3ms）
#define PPM_MIN_US          800      // 可判定为有效脉宽最小值
#define PPM_MAX_US          2200     // 可判定为有效脉宽最大值


/* ============================
   全局变量（在 MyPPM.c 中定义）
   ============================ */

// 存储每个通道的脉宽 (单位：us)
extern volatile uint16_t PPM_Channels[PPM_MAX_CHANNELS];

// 当前解析到的通道编号
extern volatile uint8_t  PPM_ChannelIndex;

// 上一次捕获的计数值
extern volatile uint32_t PPM_LastCapture;


/* ============================
   函数声明
   ============================ */

// 初始化 PPM 输入
void PPM_Init(void);

// TIM2 捕获中断服务函数（由 stm32f4xx_it.c 调用）
void PPM_TIM2_IRQHandler(void);

// 获取某一通道的值（单位：us）
uint16_t PPM_GetChannel(uint8_t ch);

// 是否有新的 PPM 数据帧（可选）
uint8_t PPM_FrameReady(void);


#endif
