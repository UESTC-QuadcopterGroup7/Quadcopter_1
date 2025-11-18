#include "MyUSART.h"
#include <stdio.h>

typedef struct {
    USART_TypeDef *instance;
    uint32_t rcc_apb_periph;
    GPIO_TypeDef *gpio_port;
    uint32_t gpio_rcc;
    uint16_t tx_pin;
    uint16_t rx_pin;
    uint8_t tx_pin_source;
    uint8_t rx_pin_source;
    uint8_t gpio_af;
    FunctionalState is_apb2;
} USART_PinConfig;

static const USART_PinConfig USART_Info[USART_PORT_COUNT] = {
    {USART1, RCC_APB2Periph_USART1, GPIOA, RCC_AHB1Periph_GPIOA, GPIO_Pin_9,  GPIO_Pin_10, 9, 10, GPIO_AF_USART1, ENABLE},
    {USART2, RCC_APB1Periph_USART2, GPIOA, RCC_AHB1Periph_GPIOA, GPIO_Pin_2,  GPIO_Pin_3,  2, 3,  GPIO_AF_USART2, DISABLE},
    {USART3, RCC_APB1Periph_USART3, GPIOB, RCC_AHB1Periph_GPIOB, GPIO_Pin_10, GPIO_Pin_11, 10, 11, GPIO_AF_USART3, DISABLE},
    {UART4,  RCC_APB1Periph_UART4,  GPIOC, RCC_AHB1Periph_GPIOC, GPIO_Pin_10, GPIO_Pin_11, 10, 11, GPIO_AF_UART4,  DISABLE},
    {UART5,  RCC_APB1Periph_UART5,  GPIOC, RCC_AHB1Periph_GPIOC, GPIO_Pin_12, GPIO_Pin_2,  12, 2,  GPIO_AF_UART5,  DISABLE},
    {USART6, RCC_APB2Periph_USART6, GPIOC, RCC_AHB1Periph_GPIOC, GPIO_Pin_6,  GPIO_Pin_7,  6, 7,  GPIO_AF_USART6, ENABLE}
};

USART_TypeDef *USART_GetInstance(USART_Port port)
{
    if (port >= USART_PORT_COUNT) {
        return NULL;
    }
    return USART_Info[port].instance;
}

void MyUSART_Init(USART_Port port, uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    if (port >= USART_PORT_COUNT) {
        return;
    }

    if (USART_Info[port].is_apb2 == ENABLE) {
        RCC_APB2PeriphClockCmd(USART_Info[port].rcc_apb_periph, ENABLE);
    } else {
        RCC_APB1PeriphClockCmd(USART_Info[port].rcc_apb_periph, ENABLE);
    }
    RCC_AHB1PeriphClockCmd(USART_Info[port].gpio_rcc, ENABLE);

    GPIO_InitStruct.GPIO_Pin = USART_Info[port].tx_pin | USART_Info[port].rx_pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(USART_Info[port].gpio_port, &GPIO_InitStruct);

    GPIO_PinAFConfig(USART_Info[port].gpio_port, USART_Info[port].tx_pin_source, USART_Info[port].gpio_af);
    GPIO_PinAFConfig(USART_Info[port].gpio_port, USART_Info[port].rx_pin_source, USART_Info[port].gpio_af);

    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART_Info[port].instance, &USART_InitStruct);

    USART_Cmd(USART_Info[port].instance, ENABLE);
}

void MyUSART_DeInit(USART_Port port)
{
    if (port >= USART_PORT_COUNT) {
        return;
    }

    USART_Cmd(USART_Info[port].instance, DISABLE);
    if (USART_Info[port].is_apb2 == ENABLE) {
        RCC_APB2PeriphClockCmd(USART_Info[port].rcc_apb_periph, DISABLE);
    } else {
        RCC_APB1PeriphClockCmd(USART_Info[port].rcc_apb_periph, DISABLE);
    }
}

void MyUSART_SendChar(USART_Port port, char data)
{
    USART_TypeDef *instance = USART_GetInstance(port);
    if (instance == NULL) {
        return;
    }

    while (USART_GetFlagStatus(instance, USART_FLAG_TXE) == RESET) {
    }
    USART_SendData(instance, (uint16_t)data);
    while (USART_GetFlagStatus(instance, USART_FLAG_TC) == RESET) {
    }
}

void MyUSART_SendString(USART_Port port, const char *str)
{
    if (str == NULL) {
        return;
    }

    while (*str) {
        MyUSART_SendChar(port, *str++);
    }
}

void MyUSART_Printf(USART_Port port, const char *format, ...)
{
    char buffer[160];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    MyUSART_SendString(port, buffer);
}
