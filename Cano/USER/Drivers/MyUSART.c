#include "MyUSART.h"
#include <stdarg.h>
#include <stdio.h>

typedef struct
{
    USART_TypeDef *instance;
    uint32_t apb_periph;
    uint8_t use_apb2;
    GPIO_TypeDef *gpio_port;
    uint32_t gpio_rcc;
    uint16_t tx_pin;
    uint16_t rx_pin;
    uint8_t tx_source;
    uint8_t rx_source;
    uint8_t gpio_af;
} USART_HWConfig;

static const USART_HWConfig USART_Table[USART_PORT_COUNT] = {
    {USART1, RCC_APB2Periph_USART1, 1, GPIOA, RCC_AHB1Periph_GPIOA, GPIO_Pin_9,  GPIO_Pin_10, GPIO_PinSource9,  GPIO_PinSource10, GPIO_AF_USART1},
    {USART2, RCC_APB1Periph_USART2, 0, GPIOA, RCC_AHB1Periph_GPIOA, GPIO_Pin_2,  GPIO_Pin_3,  GPIO_PinSource2,  GPIO_PinSource3,  GPIO_AF_USART2},
};

static const USART_HWConfig *MyUSART_GetConfig(USART_Port port)
{
    if (port >= USART_PORT_COUNT)
    {
        return NULL;
    }

    return &USART_Table[port];
}

void MyUSART_Init(USART_Port port, uint32_t baudrate)
{
    const USART_HWConfig *cfg = MyUSART_GetConfig(port);
    if (cfg == NULL)
    {
        return;
    }

    GPIO_InitTypeDef gpio_init;
    USART_InitTypeDef usart_init;

    if (cfg->use_apb2)
    {
        RCC_APB2PeriphClockCmd(cfg->apb_periph, ENABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(cfg->apb_periph, ENABLE);
    }
    RCC_AHB1PeriphClockCmd(cfg->gpio_rcc, ENABLE);

    gpio_init.GPIO_Pin = cfg->tx_pin | cfg->rx_pin;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(cfg->gpio_port, &gpio_init);

    GPIO_PinAFConfig(cfg->gpio_port, cfg->tx_source, cfg->gpio_af);
    GPIO_PinAFConfig(cfg->gpio_port, cfg->rx_source, cfg->gpio_af);

    USART_StructInit(&usart_init);
    usart_init.USART_BaudRate = baudrate;
    usart_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(cfg->instance, &usart_init);
    USART_Cmd(cfg->instance, ENABLE);
}

void MyUSART_DeInit(USART_Port port)
{
    const USART_HWConfig *cfg = MyUSART_GetConfig(port);
    if (cfg == NULL)
    {
        return;
    }

    USART_Cmd(cfg->instance, DISABLE);

    if (cfg->use_apb2)
    {
        RCC_APB2PeriphClockCmd(cfg->apb_periph, DISABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(cfg->apb_periph, DISABLE);
    }
}

void USART_SendChar(USART_Port port, char c)
{
    const USART_HWConfig *cfg = MyUSART_GetConfig(port);
    if (cfg == NULL)
    {
        return;
    }

    while (USART_GetFlagStatus(cfg->instance, USART_FLAG_TXE) == RESET)
    {
    }
    USART_SendData(cfg->instance, (uint16_t)c);
    while (USART_GetFlagStatus(cfg->instance, USART_FLAG_TC) == RESET)
    {
    }
}

void USART_SendString(USART_Port port, const char *str)
{
    if (str == NULL)
    {
        return;
    }

    while (*str)
    {
        USART_SendChar(port, *str++);
    }
}

void USART_Printf(USART_Port port, const char *format, ...)
{
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    USART_SendString(port, buffer);
}
