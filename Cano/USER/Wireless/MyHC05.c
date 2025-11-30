#include "MyHC05.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"

/* ===============================
   内部变量（不在头文件暴露）
   =============================== */

volatile uint8_t HC05_RxBuffer[100];
volatile uint8_t HC05_RxFlag = 0;

static volatile uint8_t rxIndex = 0;

/* ===============================
   内部函数（仅在本文件使用）
   =============================== */

// GPIO: PA2 = TX, PA3 = RX, PA1 = EN
static void HC05_GPIO_Config(void)
{
    GPIO_InitTypeDef gpio;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // PA2 / PA3 复用 USART2
    gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    // PA1 = EN
    gpio.GPIO_Pin = GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);

    GPIO_ResetBits(GPIOA, GPIO_Pin_1);  // 默认关闭 AT 模式
}

static void HC05_USART_Config(uint32_t baudrate)
{
    USART_InitTypeDef us;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    us.USART_BaudRate = baudrate;
    us.USART_WordLength = USART_WordLength_8b;
    us.USART_StopBits = USART_StopBits_1;
    us.USART_Parity = USART_Parity_No;
    us.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    us.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &us);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART2, ENABLE);
}

static void Delay(volatile uint32_t n)
{
    while(n--) __NOP();
}

/* ===============================
   对外 API 实现（与 HC05.h 匹配）
   =============================== */

void HC05_Init(uint32_t baudrate)
{
    HC05_GPIO_Config();
    HC05_USART_Config(baudrate);
}

void HC05_SendByte(uint8_t data)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, data);
}

void HC05_SendString(const char *str)
{
    while (*str)
        HC05_SendByte(*str++);
}

void HC05_ChangeBaudRate(uint32_t baudrate)
{
    USART_Cmd(USART2, DISABLE);
    HC05_USART_Config(baudrate);
    USART_Cmd(USART2, ENABLE);
}

void HC05_SimpleInit(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_1);  // EN = HIGH → 进入AT模式
    Delay(200000);

    HC05_ChangeBaudRate(38400);
    Delay(200000);

    HC05_SendString("AT\r\n");
    Delay(50000);

    HC05_SendString("AT+NAME=STM32_BT\r\n");
    Delay(50000);

    HC05_SendString("AT+PSWD=1234\r\n");
    Delay(50000);

    GPIO_ResetBits(GPIOA, GPIO_Pin_1);  // 退出 AT 模式
    Delay(200000);

    HC05_ChangeBaudRate(9600);
}

void HC05_UART_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint8_t d = USART_ReceiveData(USART2);

        if (rxIndex < sizeof(HC05_RxBuffer) - 1)
        {
            HC05_RxBuffer[rxIndex++] = d;

            if (d == '\n')
            {
                HC05_RxBuffer[rxIndex] = '\0';
                HC05_RxFlag = 1;
                rxIndex = 0;
            }
        }
        else
        {
            rxIndex = 0;
            HC05_RxFlag = 1;
        }
    }
}
