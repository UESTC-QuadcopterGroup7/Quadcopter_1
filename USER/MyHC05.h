/**
  ******************************************************************************
  * @file    MyHC05.h
  * @brief   Simple helper interface for the HC-05 Bluetooth module demo.
  ******************************************************************************
  */

#ifndef __MYHC05_H
#define __MYHC05_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"

#define HC05_RX_BUFFER_SIZE   (100U)
#define HC05_DEFAULT_BAUDRATE (9600U)

extern volatile uint8_t USART2_RxBuffer[HC05_RX_BUFFER_SIZE];
extern volatile uint8_t USART2_RxIndex;
extern volatile uint8_t USART2_RxFlag;

void HC05_InitHardware(uint32_t baudrate);
void HC05_ChangeBaudRate(uint32_t baudrate);
void HC05_SimpleInit(void);
void USART2_SendString(char *str);
void USART2_SendByte(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* __MYHC05_H */
