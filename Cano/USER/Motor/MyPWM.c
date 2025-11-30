#include "MyPWM.h"

/*
 * TIM1_CH1  PA8
 * PWM 输出频率：50Hz（周期20ms）
 */
void PWM_Init_PA8(void)
{
    /* 开时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    /* PA8 复用 TIM1_CH1 */
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin   = GPIO_Pin_8;
    gpio.GPIO_Mode  = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);

    /* TIM1 = 84MHz 时钟 */
    /* 50Hz = 周期 20ms */
    /* 周期计数 = 84MHz / 50 = 1,680,000 */
    /* 为避免计数太大：分频 84 → 1MHz（1us） */
    TIM_TimeBaseInitTypeDef tim;
    tim.TIM_Prescaler         = 84 - 1;     // 1MHz
    tim.TIM_Period            = 20000 - 1;  // 20ms
    tim.TIM_ClockDivision     = 0;
    tim.TIM_CounterMode       = TIM_CounterMode_Up;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &tim);

    /* PWM 模式 */
    TIM_OCInitTypeDef oc;
    oc.TIM_OCMode      = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OCPolarity  = TIM_OCPolarity_High;
    oc.TIM_Pulse       = 1000;  // 默认 1000us
    TIM_OC1Init(TIM1, &oc);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM1, ENABLE);

    /* TIM1 是高级定时器，需要强制输出 */
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    /* 使能定时器 */
    TIM_Cmd(TIM1, ENABLE);
}

/* 设置 PWM 脉宽（单位：us） */
void PWM_SetDuty(uint16_t us)
{
    if (us < 1000) us = 1000;
    if (us > 2000) us = 2000;

    TIM1->CCR1 = us;
}
