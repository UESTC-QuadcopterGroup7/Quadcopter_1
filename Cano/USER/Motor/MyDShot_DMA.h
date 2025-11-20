#ifndef __MY_DSHOT_DMA_H
#define __MY_DSHOT_DMA_H

#include "stm32f4xx.h"
#include "MyDShot.h"

/**
 * @brief 初始化与 DShot 相关的 DMA 时钟/资源。
 *
 * DShot_Init_All 内部会调用该函数，无需单独调用。
 */
void DShot_DMA_Init(void);

/**
 * @brief 使用 DMA 发送一帧 DShot 数据。
 * @param motor 电机枚举
 * @param buffer 要发送的计数数组，长度需要等于 len
 * @param len    计数长度（通常为 DSHOT_FRAME_LEN）
 */
void DShot_DMA_Transmit(DShot_Motor_t motor, uint16_t *buffer, uint32_t len);

#endif /* __MY_DSHOT_DMA_H */
