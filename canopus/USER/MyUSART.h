#ifndef MY_USART_H
#define MY_USART_H

#include "stm32f4xx.h"
#include <stdarg.h>

typedef enum {
    USART1_PORT = 0,
    USART2_PORT,
    USART3_PORT,
    UART4_PORT,
    UART5_PORT,
    USART6_PORT,
    USART_PORT_COUNT
} USART_Port;

void MyUSART_Init(USART_Port port, uint32_t baudrate);
void MyUSART_DeInit(USART_Port port);
void MyUSART_SendChar(USART_Port port, char data);
void MyUSART_SendString(USART_Port port, const char *str);
void MyUSART_Printf(USART_Port port, const char *format, ...);
USART_TypeDef *USART_GetInstance(USART_Port port);

#endif
