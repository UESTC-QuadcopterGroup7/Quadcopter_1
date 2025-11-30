#ifndef __HC05_H
#define __HC05_H

#include <stdint.h>

/* ===============================
   HC05 接收缓冲区（只暴露状态，不暴露内部实现）
   =============================== */
extern volatile uint8_t HC05_RxBuffer[100];
extern volatile uint8_t HC05_RxFlag;

/* ===============================
   对外公开的功能接口（API）
   =============================== */

/**
 * @brief 初始化 HC05 模块
 * @param baudrate 初始串口波特率，例如 9600
 */
void HC05_Init(uint32_t baudrate);

/**
 * @brief 发送一个字节
 */
void HC05_SendByte(uint8_t data);

/**
 * @brief 发送字符串
 */
void HC05_SendString(const char *str);

/**
 * @brief 设置新的波特率（内部重初始化 UART）
 */
void HC05_ChangeBaudRate(uint32_t baudrate);

/**
 * @brief 简单初始化（进入 AT 模式 + 写配置）
 */
void HC05_SimpleInit(void);

/**
 * @brief HC05 的 UART 接收中断处理函数（由 USART2_IRQHandler 调用）
 */
void HC05_UART_IRQHandler(void);

#endif
