#include "MyUSART.h"

static const struct {
    USART_TypeDef* instance;
    uint32_t rcc_apb_periph;
    GPIO_TypeDef* gpio_port;
    uint32_t gpio_rcc;
    uint16_t tx_pin;
    uint16_t rx_pin;
    uint8_t gpio_af;
} USART_Info[] = {
    {USART1, RCC_APB2Periph_USART1, GPIOA, RCC_AHB1Periph_GPIOA, GPIO_Pin_9,  GPIO_Pin_10, GPIO_AF_USART1},
    {USART2, RCC_APB1Periph_USART2, GPIOA, RCC_AHB1Periph_GPIOA, GPIO_Pin_2,  GPIO_Pin_3,  GPIO_AF_USART2},
    {USART3, RCC_APB1Periph_USART3, GPIOB, RCC_AHB1Periph_GPIOB, GPIO_Pin_10, GPIO_Pin_11, GPIO_AF_USART3},
    {UART4,  RCC_APB1Periph_UART4,  GPIOC, RCC_AHB1Periph_GPIOC, GPIO_Pin_10, GPIO_Pin_11, GPIO_AF_UART4},
    {UART5,  RCC_APB1Periph_UART5,  GPIOC, RCC_AHB1Periph_GPIOC, GPIO_Pin_12, GPIO_Pin_2,  GPIO_AF_UART5},
    {USART6, RCC_APB2Periph_USART6, GPIOC, RCC_AHB1Periph_GPIOC, GPIO_Pin_6,  GPIO_Pin_7,  GPIO_AF_USART6}
};

USART_TypeDef*USART_GetInstance(USART_Port port){
	if(port>USART6_PORT)return NULL;
	return USART_Info[port].instance;
} //获取USART实例

void MyUSART_Init(USART_Port port,uint32_t baudrate){
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	if(port>USART6_PORT)return;
	
	//使能时钟
	if(port==USART1_PORT||port==USART6_PORT){
		RCC_APB2PeriphClockCmd(USART_Info[port].rcc_apb_periph,ENABLE);
	}else{
		RCC_APB1PeriphClockCmd(USART_Info[port].rcc_apb_periph,ENABLE);
	}
	RCC_AHB1PeriphClockCmd(USART_Info[port].gpio_rcc,ENABLE);
	
	//配置GPIO
	GPIO_InitStruct.GPIO_Pin=USART_Info[port].tx_pin|USART_Info[port].rx_pin;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(USART_Info[port].gpio_port,&GPIO_InitStruct);
	
	//配置AF
	GPIO_PinAFConfig(USART_Info[port].gpio_port,
		(port==USART2_PORT)?GPIO_PinSource2:GPIO_PinSource9,
		USART_Info[port].gpio_af);
	GPIO_PinAFConfig(USART_Info[port].gpio_port,
		(port==USART2_PORT)?GPIO_PinSource3:GPIO_PinSource10,
		USART_Info[port].gpio_af);
	
	//配置USART
	USART_InitStruct.USART_BaudRate=baudrate;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART_Info[port].instance,&USART_InitStruct);

	USART_Cmd(USART_Info[port].instance,ENABLE);
	
	//配置NVIC 用于中断接收
	if(port==USART2_PORT){
		NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
		NVIC_Init(&NVIC_InitStruct);
		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	}
}

// USART反初始化
void MyUSART_DeInit(USART_Port port) {
    if (port > USART6_PORT) return;
    
    USART_Cmd(USART_Info[port].instance, DISABLE);
    
    if (port == USART1_PORT || port == USART6_PORT) {
        RCC_APB2PeriphClockCmd(USART_Info[port].rcc_apb_periph, DISABLE);
    } else {
        RCC_APB1PeriphClockCmd(USART_Info[port].rcc_apb_periph, DISABLE);
    }
}