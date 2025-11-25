#ifndef __MY_SBUS_H
#define __MY_SBUS_H

#include "stm32f4xx.h"
#include <stdint.h>

#define SBUS_FRAME_LEN   25
#define SBUS_CHANNELS    16

typedef struct {
    uint16_t channels[SBUS_CHANNELS];
    uint8_t failsafe;
    uint8_t frameLost;
    uint8_t updated;
} SBUS_Data_t;

/**
 * @brief 初始化 USART3(PB11) 接收 SBUS。
 */
void SBUS_Init(void);

/**
 * @brief 获取最新解析的数据指针。
 */
const SBUS_Data_t *SBUS_GetData(void);

#endif /* __MY_SBUS_H */
