#include "MyPPM_temp.h"

/* =============================
   全局变量定义
   =============================*/
volatile uint16_t PPM_Channels[PPM_MAX_CHANNELS] = {0};
volatile uint8_t  PPM_ChannelIndex = 0;
volatile uint32_t PPM_LastCapture = 0;

volatile uint8_t  PPM_NewFrameFlag = 0;


/* =============================
   初始化定时器 TIM2 + PA0
   =============================*/
void PPM_Init(void)
{
    /* 开启时钟 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   // TIM2 时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // GPIOA 时钟

    /* PA0 设置为 TIM2_CH1（AF1） */
    GPIOA->MODER &= ~(3U << (0 * 2));
    GPIOA->MODER |=  (2U << (0 * 2));    // AF 模式
    GPIOA->AFR[0]  &= ~(0xF << (0 * 4));
    GPIOA->AFR[0]  |=  (0x1 << (0 * 4)); // AF1 = TIM2_CH1

    /* 配置 TIM2 输入捕获 */
    TIM2->PSC = 84 - 1;    // 84MHz/84 = 1MHz（1us 每计一次）
    TIM2->ARR = 0xFFFFFFFF; // 32 位计数器最大

    /* 输入捕获 CH1 配置 */
    TIM2->CCMR1 &= ~TIM_CCMR1_CC1S;
    TIM2->CCMR1 |=  TIM_CCMR1_CC1S_0;  // CC1S = 01 → IC1 映射到 TI1

    TIM2->CCER &= ~TIM_CCER_CC1P;      // 上升沿捕获
    TIM2->CCER &= ~TIM_CCER_CC1NP;     // 非反向

    TIM2->CCER |= TIM_CCER_CC1E;       // 开启捕获

    /* 清标志 & 开启中断 */
    TIM2->SR = 0;
    TIM2->DIER |= TIM_DIER_CC1IE;      // 允许 CH1 捕获中断

    /* 使能 TIM2 */
    TIM2->CR1 |= TIM_CR1_CEN;

    /* NVIC 配置 */
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);
}


/* =============================
   TIM2 捕获中断（解析 PPM）
   =============================*/
void PPM_TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_CC1IF)   // 捕获事件
    {
        TIM2->SR &= ~TIM_SR_CC1IF; // 清中断标志

        uint32_t now = TIM2->CCR1;
        uint32_t width = (now >= PPM_LastCapture) ?
                         (now - PPM_LastCapture) :
                         (0xFFFFFFFF - PPM_LastCapture + now);

        PPM_LastCapture = now;

        /* 判断同步脉冲（>3ms） */
        if (width > PPM_SYNC_US)
        {
            PPM_ChannelIndex = 0;   // 新一帧开始
        }
        else
        {
            /* 保存通道脉宽 */
            if (PPM_ChannelIndex < PPM_MAX_CHANNELS)
            {
                /* 脉宽有效性判断 */
                if (width >= PPM_MIN_US && width <= PPM_MAX_US)
                    PPM_Channels[PPM_ChannelIndex] = width;
            }
            PPM_ChannelIndex++;

            /* 接收到至少 8 通道，说明收到完整一帧 */
            if (PPM_ChannelIndex >= PPM_MAX_CHANNELS)
                PPM_NewFrameFlag = 1;
        }
    }
}


/* =============================
   返回某通道的脉宽
   =============================*/
uint16_t PPM_GetChannel(uint8_t ch)
{
    if (ch >= PPM_MAX_CHANNELS) return 1500;
    return PPM_Channels[ch];
}


/* =============================
   是否收到新的 PPM 帧
   =============================*/
uint8_t PPM_FrameReady(void)
{
    if (PPM_NewFrameFlag)
    {
        PPM_NewFrameFlag = 0;
        return 1;
    }
    return 0;
}

void TIM2_IRQHandler(void)
{
    // 调用你写好的解析函数
    PPM_TIM2_IRQHandler();
}