#include "MyDShot.h"

/* ============================================================
   内部缓冲：每一路 17 个计数值（16位数据 + 结尾 0）
   ============================================================ */
static uint16_t dshotBuf[4][DSHOT_FRAME_LEN];

/* ============================================================
   GPIO 初始化：TIM1_CH1/2/3 = PA8/PA9/PA10
                TIM3_CH3     = PB0
   ============================================================ */
static void DShot_GPIO_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
                           RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode  = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd  = GPIO_PuPd_DOWN;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;

    /* TIM1_CH1 = PA8, CH2 = PA9, CH3 = PA10 */
    gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOA, &gpio);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8,  GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);

    /* TIM3_CH3 = PB0 */
    gpio.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &gpio);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
}

/* ============================================================
   定时器初始化
   TIM1：CH1/CH2/CH3
   TIM3：CH3
   频率：600kHz（ARR=139）
   PWM：每 bit 使用 PWM1 模式改变 CCRx
   ============================================================ */
static void DShot_TIM_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef tim;
    tim.TIM_Prescaler         = 0;
    tim.TIM_CounterMode       = TIM_CounterMode_Up;
    tim.TIM_Period            = DSHOT_PERIOD_TICKS - 1;  // 140-1 = 139
    tim.TIM_ClockDivision     = 0;
    tim.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM1, &tim);
    TIM_TimeBaseInit(TIM3, &tim);

    TIM_OCInitTypeDef oc;
    oc.TIM_OCMode      = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OCPolarity  = TIM_OCPolarity_High;
    oc.TIM_Pulse       = 0;  // 初始为 0

    /* TIM1 各通道 */
    TIM_OC1Init(TIM1, &oc); TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2Init(TIM1, &oc); TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3Init(TIM1, &oc); TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

    /* TIM3_CH3 */
    TIM_OC3Init(TIM3, &oc); TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    /* 高级定时器 TIM1 必须使能输出 */
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

/* ============================================================
   构建一个 16-bit DShot 帧
   ============================================================ */
static uint16_t DShot_BuildFrame(uint16_t value)
{
    value <<= 1;   // LSB = telemetry = 0

    uint16_t csum = 0, d = value;
    for (int i = 0; i < 3; i++)
        csum ^= (d >> (i * 4)) & 0x0F;

    return (value << 4) | (csum & 0x0F);
}

/* ============================================================
   构建 buffer（17 个 CCR 脉宽）
   ============================================================ */
static void DShot_FillBuffer(uint16_t *buf, uint16_t value)
{
    uint16_t frame = DShot_BuildFrame(value);

    for (int i = 0; i < 16; i++)
    {
        if (frame & (1 << (15 - i)))
            buf[i] = DSHOT_BIT1_TICKS;
        else
            buf[i] = DSHOT_BIT0_TICKS;
    }
    buf[16] = 0;   // 收尾 bit (low)
}

/* ============================================================
   单路电机发送（DMA 方式）
   ============================================================ */
void DShot_Send(DShot_Motor_t motor, uint16_t value)
{
    /* 生成脉宽数组 */
    DShot_FillBuffer(dshotBuf[motor], value);
    DShot_DMA_Transmit(motor, dshotBuf[motor], DSHOT_FRAME_LEN);
}

/* ============================================================
   同时对 4 个电机发 DShot
   （内部是按顺序轮流发的）
   ============================================================ */
void DShot_Send_All(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4)
{
    DShot_Send(DSHOT_M1, m1);
    DShot_Send(DSHOT_M2, m2);
    DShot_Send(DSHOT_M3, m3);
    DShot_Send(DSHOT_M4, m4);
}

/* ============================================================
   总初始化
   ============================================================ */
void DShot_Init_All(void)
{
    DShot_GPIO_Init();
    DShot_TIM_Init();
    DShot_DMA_Init();
}
