#include "MyI2S.h"

void I2S2_Init(void){
	//启用SPI2/I2S2时钟
	RCC->APB1ENR|=RCC_APB1ENR_SPI2EN;
	
	//禁用SPI2/I2S2
	SPI2->I2SCFGR&=~SPI_I2SCFGR_I2SE;
	
	//配置I2S
	SPI2->I2SCFGR=
	/*
		SPI_I2SCFGR_I2SMOD| //I2S模式 非SPI模式
		SPI_I2SCFGR_I2SCFG_1| //主发送
		SPI_I2SCFGR_I2SSTD_0| //Philips标准
		SPI_I2SCFGR_CKPOL| //时钟极性 低电平空闲
		//SPI_I2SCFGR_DATFMT_0| //32位数据扩展为16位
		//(0x2<<SPI_I2SCFGR_DATLEN_Pos)| //24位数据长度
		(0x0<<SPI_I2SCFGR_DATLEN)| //16位数据长度
		(0x0<<SPI_I2SCFGR_PCMSYNC)| //短帧同步
		SPI_I2SCFGR_CHLEN;//32位数据位
	*/
		SPI_I2SCFGR_I2SMOD |        // 启用I2S模式
		(0x2 << 8) |                // 主发送 (I2SCFG=10)
		(0x1 << 4) |                // Philips标准 (I2SSTD=01)
		SPI_I2SCFGR_CKPOL |         // 时钟极性
		(0x0 << 1) |                // 16位数据长度
		(0x0 << 7) |                // 短帧同步
		SPI_I2SCFGR_CHLEN;          // 32位通道长度
	// 配置I2S预分频器
	// I2S时钟=PLLI2S_VCO/PLLI2SR=192/5=38.4MHz
	// 目标采样率=44.1MHz
	// I2SDIV = I2SxCLK / (32 * 2 * SampleRate) = 38400000 / (64 * 44100) ≈ 13.61
    // 选择ODD=0, I2SDIV=13 → 实际采样率 = 38400000 / (32 * 2 * (2*13)) = 23076Hz
    // 调整PLLI2S参数以获得精确的44.1kHz
	
	SPI2->I2SPR=
		(10<<0)| //I2SDIV=13
		(1<<8)| //ODD=1
		SPI_I2SPR_MCKOE; //主时钟输出启用 可选

	// 启用I2S
	SPI2->I2SCFGR|=SPI_I2SCFGR_I2SE;
}

void I2S2_Start(void){
	// 启用DMA请求
	SPI2->CR2|=SPI_CR2_TXDMAEN;
}