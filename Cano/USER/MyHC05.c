#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stdio.h"
#include "string.h"

// 函数声明
void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART2_Configuration(uint32_t baudrate);
void NVIC_Configuration(void);
void USART2_SendString(char *str);
void USART2_SendByte(uint8_t data);
void Delay(__IO uint32_t nCount);
void HC05_ChangeBaudRate(uint32_t baudrate);
void HC05_SimpleInit(void);

// 全局变量
volatile uint8_t USART2_RxBuffer[100];
volatile uint8_t USART2_RxIndex = 0;
volatile uint8_t USART2_RxFlag = 0;

int main(void)
{
    // 系统初始化
    RCC_Configuration();
    GPIO_Configuration();
    USART2_Configuration(9600);  // 初始波特率9600
    NVIC_Configuration();
    
    USART2_SendString("STM32 HC-05 Bluetooth Test\r\n");
    USART2_SendString("Initializing HC-05...\r\n");
    
    // 使用默认设置并配置名称和密码
    HC05_SimpleInit();
    
    USART2_SendString("HC-05 Ready! Search for device in Bluetooth\r\n");
    
    while (1)
    {
        if(USART2_RxFlag)
        {
            // 处理接收到的蓝牙数据
            USART2_SendString("Received: ");
            USART2_SendString((char*)USART2_RxBuffer);
            
            // 根据命令执行操作
            if(strstr((char*)USART2_RxBuffer, "LED_ON"))
            {
                GPIO_SetBits(GPIOA, GPIO_Pin_5); // Nucleo LD2 LED
                USART2_SendString("LED ON\r\n");
            }
            else if(strstr((char*)USART2_RxBuffer, "LED_OFF"))
            {
                GPIO_ResetBits(GPIOA, GPIO_Pin_5);
                USART2_SendString("LED OFF\r\n");
            }
            else if(strstr((char*)USART2_RxBuffer, "AT"))
            {
                USART2_SendString("OK\r\n"); // 响应AT命令
            }
            
            // 清除接收标志和缓冲区
            USART2_RxFlag = 0;
            USART2_RxIndex = 0;
            memset((void*)USART2_RxBuffer, 0, sizeof(USART2_RxBuffer));
        }
        
        // 定期发送状态（可选）
        static uint32_t counter = 0;
        if(counter++ > 1000000) {
            // USART2_SendString("STM32 running...\r\n");
            counter = 0;
        }
    }
}

// 改变波特率函数
void HC05_ChangeBaudRate(uint32_t baudrate)
{
    // 禁用USART2
    USART_Cmd(USART2, DISABLE);
    
    // 重新配置USART2
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART2, &USART_InitStructure);
    
    // 重新使能USART2
    USART_Cmd(USART2, ENABLE);
    
    // 发送波特率改变信息
    char msg[50];
    sprintf(msg, "Baudrate changed to: %lu\r\n", baudrate);
    USART2_SendString(msg);
}

// 简化的HC-05初始化（配置自定义名称和密码）
void HC05_SimpleInit(void)
{
    USART2_SendString("HC-05 Configuration Start...\r\n");
    
    // 确保使用9600波特率
    HC05_ChangeBaudRate(9600);
    Delay(1000000);
    
    // 尝试进入AT模式并配置
    USART2_SendString("Entering AT mode...\r\n");
    
    // 方法1：拉高EN引脚进入AT模式
    GPIO_SetBits(GPIOA, GPIO_Pin_1);  // EN = HIGH
    Delay(2000000);  // 等待2秒进入AT模式
    
    // 尝试AT模式常用波特率38400
    HC05_ChangeBaudRate(38400);
    Delay(1000000);
    
    // 发送AT命令配置蓝牙
    USART2_SendString("Sending AT commands...\r\n");
    
    // 测试AT命令
    USART2_SendString("AT\r\n");
    Delay(500000);
    
    // 设置设备名称
    USART2_SendString("AT+NAME=STM32_BT\r\n");
    Delay(500000);
    
    // 设置配对密码
    USART2_SendString("AT+PSWD=4321\r\n");
    Delay(500000);
    
    // 设置波特率
    USART2_SendString("AT+UART=9600,0,0\r\n");
    Delay(500000);
    
    // 设置为从模式
    USART2_SendString("AT+ROLE=0\r\n");
    Delay(500000);
    
    // 任意地址连接模式
    USART2_SendString("AT+CMODE=1\r\n");
    Delay(500000);
    
    // 退出AT模式
    USART2_SendString("Exiting AT mode...\r\n");
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);  // EN = LOW
    Delay(2000000);  // 等待2秒重启
    
    // 恢复通信波特率9600
    HC05_ChangeBaudRate(9600);
    Delay(1000000);
    
    USART2_SendString("HC-05 Configuration Complete!\r\n");
    USART2_SendString("Device Name: STM32_BT\r\n");
    USART2_SendString("PIN Code: 4321\r\n");
    USART2_SendString("Search for 'STM32_BT' in Bluetooth list\r\n");
}

// 系统时钟配置
void RCC_Configuration(void)
{
    // 使能GPIOA时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    // 使能USART2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

// GPIO配置
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 配置USART2引脚 PA2(TX), PA3(RX)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置AF7 (USART2)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    
    // 配置LED引脚 PA5 (Nucleo板载LED)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置EN引脚 PA1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  // PA1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_1); // 默认低电平
}

// USART2配置
void USART2_Configuration(uint32_t baudrate)
{
    USART_InitTypeDef USART_InitStructure;
    
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(USART2, &USART_InitStructure);
    
    // 使能USART2接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    // 使能USART2
    USART_Cmd(USART2, ENABLE);
}

// NVIC配置
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 配置USART2中断
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// 发送字符串
void USART2_SendString(char *str)
{
    while(*str)
    {
        USART2_SendByte(*str++);
    }
}

// 发送单个字节
void USART2_SendByte(uint8_t data)
{
    // 等待发送缓冲区为空
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    
    // 发送数据
    USART_SendData(USART2, data);
}

// 简单的延时函数
void Delay(__IO uint32_t nCount)
{
    while(nCount--)
    {
        __NOP(); // 空操作，避免被优化
    }
}

// USART2中断服务函数
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        // 清除中断标志
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        
        // 读取接收到的数据
        uint8_t receivedData = USART_ReceiveData(USART2);
        
        // 存储数据到缓冲区
        if(USART2_RxIndex < sizeof(USART2_RxBuffer) - 1)
        {
            USART2_RxBuffer[USART2_RxIndex++] = receivedData;
            
            // 检测到换行符或缓冲区快满，表示命令结束
            if(receivedData == '\n' || USART2_RxIndex >= sizeof(USART2_RxBuffer)-1)
            {
                USART2_RxBuffer[USART2_RxIndex] = '\0'; // 字符串结束符
                USART2_RxFlag = 1;
            }
        }
        else
        {
            // 缓冲区溢出，重置
            USART2_RxIndex = 0;
            USART2_RxFlag = 1;
        }
    }
}