/**
  ******************************************************************************
  * @file    
  * @author  Your Name
  * @version V1.0
  * @brief   微秒级延时头文件 for STM32F401RE
  ******************************************************************************
  */

#ifndef __MYUSART_H
#define __MYUSART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include <stdio.h>

/*
// USART端口定义
typedef enum {
    USART1_PORT = 0,
    USART2_PORT,
    USART3_PORT,
    UART4_PORT,
    UART5_PORT,
    USART6_PORT
} USART_Port;

// USART配置结构体
typedef struct {
    USART_Port port;
    uint32_t baudrate;
    uint16_t word_length;
    uint16_t stop_bits;
    uint16_t parity;
    uint16_t mode;
    uint16_t hardware_flow_control;
} USART_ConfigTypeDef;

void MyUSART_Init(USART_Port port, uint32_t baudrate);
void MyUSART_DeInit(USART_Port port);
uint8_t USART_SendByte(USART_Port port, uint8_t data);
uint8_t USART_ReceiveByte(USART_Port port, uint8_t *data);
uint16_t MyUSART_SendData(USART_Port port, const uint8_t *data, uint16_t length);
uint16_t MyUSART_ReceiveData(USART_Port port, uint8_t *buffer, uint16_t buffer_size, uint32_t timeout);
uint8_t USART_SendString(USART_Port port, const char *str);
void USART_Printf(USART_Port port, const char *format, ...);
uint8_t USART_GetChar(USART_Port port, uint8_t *ch, uint32_t timeout);
uint16_t USART_GetLine(USART_Port port, char *buffer, uint16_t buffer_size, uint32_t timeout);
uint8_t USART_IsDataAvailable(USART_Port port);
void USART_FlushRxBuffer(USART_Port port);
void USART_EnableInterrupt(USART_Port port, uint32_t interrupt);
void USART_DisableInterrupt(USART_Port port, uint32_t interrupt);

USART_TypeDef* USART_GetInstance(USART_Port port);

//使用寄存器配置
// USART基地址定义
#define MyUSART1_BASE    0x40011000
#define MyUSART2_BASE    0x40004400
#define MyUSART3_BASE    0x40004800
#define MyUART4_BASE     0x40004C00
#define MyUART5_BASE     0x40005000
#define MyUSART6_BASE    0x40011400

// USART寄存器结构体
typedef struct {
    volatile uint32_t SR;     // 状态寄存器,     偏移: 0x00
    volatile uint32_t DR;     // 数据寄存器,     偏移: 0x04
    volatile uint32_t BRR;    // 波特率寄存器,   偏移: 0x08
    volatile uint32_t CR1;    // 控制寄存器1,    偏移: 0x0C
    volatile uint32_t CR2;    // 控制寄存器2,    偏移: 0x10
    volatile uint32_t CR3;    // 控制寄存器3,    偏移: 0x14
    volatile uint32_t GTPR;   // 保护时间和预分频, 偏移: 0x18
} MyUSART_TypeDef;

// USART实例指针
#define MyUSART1        ((MyUSART_TypeDef *) USART1_BASE)
#define MyUSART2        ((MyUSART_TypeDef *) USART2_BASE)
#define MyUSART3        ((MyUSART_TypeDef *) USART3_BASE)
#define MyUART4         ((MyUSART_TypeDef *) UART4_BASE)
#define MyUART5         ((MyUSART_TypeDef *) UART5_BASE)
#define MyUSART6        ((MyUSART_TypeDef *) USART6_BASE)
*/

void USART1_Init(void);
void USART_SendChar(char c);
void USART_SendString(const char *str);

#ifdef __cplusplus
}
#endif

#endif /*  */