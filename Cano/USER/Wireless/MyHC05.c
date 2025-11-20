#include "MyHC05.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "string.h"
#include "stdio.h"

volatile uint8_t HC05_RxBuffer[100];
volatile uint8_t HC05_RxIndex = 0;
volatile uint8_t HC05_RxFlag = 0;

// 内部函数声明
static void HC05_GPIO_Config(void);
static void HC05_USART_Config(uint32_t baudrate);
static void Delay(__IO uint32_t nCount);

void HC05_Init(uint32_t baudrate)
{
    HC05_GPIO_Config();
    HC05_USART_Config(baudrate);
}

static void HC05_GPIO_Config(void)
{
    GPIO_InitTypeDef gpio;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // PA2 = TX, PA3 = RX
    gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA, &gpio);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    // PA1 = EN 引脚
    gpio.GPIO_Pin = GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA, &gpio);

    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
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

void HC05_SendString(char *str)
{
    while (*str)
        HC05_SendByte(*str++);
}

void HC05_SendByte(uint8_t data)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, data);
}

void HC05_ChangeBaudRate(uint32_t baudrate)
{
    USART_Cmd(USART2, DISABLE);

    HC05_USART_Config(baudrate);

    USART_Cmd(USART2, ENABLE);
}

void HC05_SimpleInit(void)
{
    HC05_SendString("HC05 Simple Init...\r\n");

    HC05_ChangeBaudRate(9600);
    Delay(500000);

    GPIO_SetBits(GPIOA, GPIO_Pin_1);  // EN = HIGH
    Delay(2000000);

    HC05_ChangeBaudRate(38400);
    Delay(1000000);

    HC05_SendString("AT\r\n");
    Delay(500000);

    HC05_SendString("AT+NAME=STM32_BT\r\n");
    Delay(500000);

    HC05_SendString("AT+PSWD=4321\r\n");
    Delay(500000);

    HC05_SendString("AT+UART=9600,0,0\r\n");
    Delay(500000);

    HC05_SendString("AT+ROLE=0\r\n");
    Delay(500000);

    HC05_SendString("HC05 Config Done\r\n");

    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    Delay(1000000);

    HC05_ChangeBaudRate(9600);
}

static void Delay(__IO uint32_t n)
{
    while(n--) __NOP();
}

// ============ 中断回调（与主工程兼容） ============

void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        uint8_t d = USART_ReceiveData(USART2);

        if (HC05_RxIndex < sizeof(HC05_RxBuffer) - 1)
        {
            HC05_RxBuffer[HC05_RxIndex++] = d;

            if (d == '\n')
            {
                HC05_RxBuffer[HC05_RxIndex] = '\0';
                HC05_RxFlag = 1;
            }
        }
        else
        {
            HC05_RxIndex = 0;
            HC05_RxFlag = 1;
        }
    }
}
