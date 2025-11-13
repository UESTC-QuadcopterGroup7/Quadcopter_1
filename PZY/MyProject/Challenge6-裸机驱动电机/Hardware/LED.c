#include "stm32f4xx.h"

void LED_Init(void){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
}
/*定义：高电平点亮*/
void LED_ON(void){
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
}

void LED_OFF(void){
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}
void LED_Turn(void){
	GPIO_ToggleBits(GPIOA,GPIO_Pin_5);
}
