#include "MyDSHOT.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"

/* DMA 通道映射结构 */
typedef struct {
    DMA_Stream_TypeDef *stream;
    uint32_t            channel;
    volatile uint32_t  *ccr;
    TIM_TypeDef        *timer;
    uint32_t            timerDmaReq;
} DShotDMAChannel_t;

/* 获取 TC 标志 */
static uint32_t dma_get_tc_flag(DMA_Stream_TypeDef *stream)
{
    if (stream == DMA2_Stream0) return DMA_FLAG_TCIF0;
    if (stream == DMA2_Stream1) return DMA_FLAG_TCIF1;
    if (stream == DMA2_Stream2) return DMA_FLAG_TCIF2;
    if (stream == DMA2_Stream3) return DMA_FLAG_TCIF3;
    if (stream == DMA2_Stream4) return DMA_FLAG_TCIF4;
    if (stream == DMA2_Stream5) return DMA_FLAG_TCIF5;
    if (stream == DMA2_Stream6) return DMA_FLAG_TCIF6;
    if (stream == DMA2_Stream7) return DMA_FLAG_TCIF7;
    return DMA_FLAG_TCIF0;
}

/* ---------------------------------------------------------------
   STM32F401RE 正确 DMA 映射（官方手册验证）
   M1 = TIM1_CH1 = DMA2 Stream1 Channel6
   M2 = TIM1_CH2 = DMA2 Stream2 Channel6
   M3 = TIM1_CH3 = DMA2 Stream7 Channel0
   M4 = TIM3_CH3 = DMA1 Stream7 Channel5
   --------------------------------------------------------------- */
static const DShotDMAChannel_t dmaChannels[4] = {
    /* DSHOT_M1 = TIM1_CH1 (PA8) */
    { DMA2_Stream1, DMA_Channel_6, &TIM1->CCR1, TIM1, TIM_DMA_CC1 },

    /* DSHOT_M2 = TIM1_CH2 (PA9) */
    { DMA2_Stream2, DMA_Channel_6, &TIM1->CCR2, TIM1, TIM_DMA_CC2 },

    /* DSHOT_M3 = TIM1_CH3 (PA10) */
    { DMA2_Stream7, DMA_Channel_0, &TIM1->CCR3, TIM1, TIM_DMA_CC3 },

    /* DSHOT_M4 = TIM3_CH3 (PB0) */
    { DMA1_Stream7, DMA_Channel_5, &TIM3->CCR3, TIM3, TIM_DMA_CC3 },
};

/* ---------------------------------------------------------------
   初始化 DMA 控制器
   --------------------------------------------------------------- */
void DShot_DMA_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 |
                           RCC_AHB1Periph_DMA2, ENABLE);
}

/* ---------------------------------------------------------------
   单路 DMA 配置
   --------------------------------------------------------------- */
static void DShot_DMA_Config(const DShotDMAChannel_t *cfg,
                             uint16_t *buffer,
                             uint32_t len)
{
    DMA_DeInit(cfg->stream);
    while (DMA_GetCmdStatus(cfg->stream) != DISABLE) {}

    DMA_InitTypeDef dma;
    DMA_StructInit(&dma);

    dma.DMA_Channel            = cfg->channel;
    dma.DMA_PeripheralBaseAddr = (uint32_t)cfg->ccr;
    dma.DMA_Memory0BaseAddr    = (uint32_t)buffer;
    dma.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    dma.DMA_BufferSize         = len;

    dma.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc          = DMA_MemoryInc_Enable;

    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;

    dma.DMA_Mode               = DMA_Mode_Normal;
    dma.DMA_Priority           = DMA_Priority_High;
    dma.DMA_FIFOMode           = DMA_FIFOMode_Disable;

    DMA_Init(cfg->stream, &dma);

    DMA_ClearFlag(cfg->stream, dma_get_tc_flag(cfg->stream));
    DMA_Cmd(cfg->stream, ENABLE);
}

/* ---------------------------------------------------------------
   等待 DMA 完成
   --------------------------------------------------------------- */
static void DShot_DMA_WaitComplete(const DShotDMAChannel_t *cfg)
{
    uint32_t tcFlag = dma_get_tc_flag(cfg->stream);
    while (DMA_GetFlagStatus(cfg->stream, tcFlag) == RESET) {}
    DMA_ClearFlag(cfg->stream, tcFlag);
    DMA_Cmd(cfg->stream, DISABLE);
}

/* ---------------------------------------------------------------
   对单路电机发送一帧 DShot
   --------------------------------------------------------------- */
void DShot_DMA_Transmit(DShot_Motor_t motor, uint16_t *buffer, uint32_t len)
{
    if (motor > DSHOT_M4) return;

    const DShotDMAChannel_t *cfg = &dmaChannels[motor];

    /* 开启定时器的 DMA 请求 */
    TIM_DMACmd(cfg->timer, cfg->timerDmaReq, ENABLE);

    /* 配置 DMA */
    DShot_DMA_Config(cfg, buffer, len);

    /* 等待完成 */
    DShot_DMA_WaitComplete(cfg);

    /* 关闭 DMA 请求 */
    TIM_DMACmd(cfg->timer, cfg->timerDmaReq, DISABLE);

    /* 清尾巴 */
    *cfg->ccr = 0;
}
