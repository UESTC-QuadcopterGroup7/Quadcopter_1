#include "MyDMA.h"

void MyDMA_Init(void){
	//启用DMA时钟
	RCC->AHB1ENR|=RCC_AHB1ENR_DMA1EN;

	//配置DMA1 Stream4 SPI2_TX
	DMA1_Stream4->CR&=~DMA_SxCR_EN; //禁用DMA
	
	//等待DMA禁用
	while(DMA1_Stream4->CR&DMA_SxCR_EN);
	
	DMA1_Stream4->CR=
		DMA_SxCR_CHSEL_0 | //使用Channel 0 SPI2_TX 
		DMA_SxCR_MSIZE_0 | //传输的数据位宽为8位 存储器端
		DMA_SxCR_PSIZE_0 | //外设端
		DMA_SxCR_MINC | //Memory increment 地址自增
		DMA_SxCR_DIR_0 | //传输方向 存储器到外设 01 
	//另外 00 外设到存储器 10 存储器到存储器 11 保留
		DMA_SxCR_CIRC | //循环模式
		DMA_SxCR_TCIE | //传输完成产生中断
		DMA_SxCR_PL; //设置DMA流优先级 PL是11最高
	
	DMA1_Stream4->PAR=(uint32_t)&(SPI2->DR); //Peripheral address
	DMA1_Stream4->M0AR=(uint32_t)audio_buffer; //Memory address
	DMA1_Stream4->NDTR=AUDIO_BUFFER_SIZE; //Number of data items
	
	//启用DMA传输完成中断
	NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	NVIC_SetPriority(DMA1_Stream4_IRQn,0);
}

void DMA_Start(void){
	DMA1_Stream4->CR|=DMA_SxCR_EN;
}
