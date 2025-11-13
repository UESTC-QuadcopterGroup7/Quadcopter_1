#include "stm32f4xx.h"
#include <stdio.h>
#include <stdarg.h>

uint8_t BL_RxPacket[4];		//定义接收数据包数组
uint8_t BL_RxFlag = 0;		//定义串口接收的标志位变量

//使用USART2资源
//PA2-D1-TX,PA3-D0-RX 
void BL_Init(void)
{   /*1.开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    
    /*2.TX和RX对应引脚配置*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

    /*3.配置USART参数，并打开接收中断,并使能*/
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2,&USART_InitStructure);

    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

    USART_Cmd(USART2,ENABLE);
    /*4.配置NVIC*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);
}

void BL_SendByte(uint8_t Byte)
{
    USART_SendData(USART2,Byte);
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) != SET);
}

void BL_SendArray(uint8_t *Array,uint16_t Length)
{
    for(uint16_t i = 0;i < Length;i++)
    {
        BL_SendByte(Array[i]);
    }
}

void BL_SendString(char *String)
{
    for(uint8_t i = 0;String[i] != '\0';i++)
    {
        BL_SendByte(String[i]);
    }
}

uint32_t BL_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将X累乘到结果
	}arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	BL_SendString(String);		//串口发送字符数组（字符串）
}

uint8_t BL_GetRxData(void)
{
    return USART_ReceiveData(USART2);
}

uint8_t BL_GetRxFlag(void)
{
	if (BL_RxFlag == SET)			//如果标志位为1
	{
		BL_RxFlag = RESET;
		return SET;					//则返回1，并自动清零标志位
	}
	return RESET;						//如果标志位为0，则返回0
}
/*固定hex数据包格式为0xFF+四位数据+0xEE*/
void USART2_IRQHandler(void)
{   
    static uint8_t BL_RxState = 0;
    static uint8_t pRxPacket;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)		
	{
        uint8_t BL_RxData = USART_ReceiveData(USART2);
        if(BL_RxState == 0)
        {
            if(BL_RxData == 0xFF)
            {
                BL_RxState = 1;
                pRxPacket = 0;
            }
        }
        else if(BL_RxState == 1)
        {   
            BL_RxPacket[pRxPacket++] = BL_RxData;
            if(pRxPacket >= 4)
            {
                BL_RxState = 2;
            }
        }
        else if(BL_RxState == 2)
        {
            if(BL_RxData == 0xEE)
            {
                BL_RxFlag = SET;
                BL_RxState = 0;
            }
        }
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);		//清除标志位
    }
																	
}
	return Result;
} 

void BL_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		//根据数字长度遍历数字的每一位
	{
		BL_SendByte(Number / BL_Pow(10, Length - i - 1) % 10 + '0');	//依次调用BL_SendByte发送每位数字
	}
}

int fputc(int ch, FILE *f)
{
	BL_SendByte(ch);			//将printf的底层重定向到自己的发送字节函数
	return ch;
}
void BL_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list 