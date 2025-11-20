/**
  ******************************************************************************
  * @file    MyUSART.h
  * @brief   Minimal USART helper utilities
  ******************************************************************************
  */

#ifndef __MYUSART_H
#define __MYUSART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include <stdarg.h>

typedef enum
{
    USART1_PORT = 0,
    USART2_PORT,
    USART_PORT_COUNT
} USART_Port;

void MyUSART_Init(USART_Port port, uint32_t baudrate);
void MyUSART_DeInit(USART_Port port);
void USART_SendChar(USART_Port port, char c);
void USART_SendString(USART_Port port, const char *str);
void USART_Printf(USART_Port port, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* __MYUSART_H */
