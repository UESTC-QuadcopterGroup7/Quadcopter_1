#include "MyOneShot.h"

/*
 * TIM1 时钟 = 84MHz
 * 设置成 1MHz（1 tick = 1 us）
 * PSC = 84 - 1
 * ARR = 300（支持到 300us）
 */

void OneShot_Init(void)
{
    /* GPIO 时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* GPIO 配置为复用 */
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode  = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd  = GPIO_PuPd_DOWN;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;

    gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOA, &gpio);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8,  GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_TIM1);

    /* TIM1 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitTypeDef tim;
    tim.TIM_Prescaler         = 84 - 1;   // 84MHz → 1MHz
    tim.TIM_CounterMode       = TIM_CounterMode_Up;
    tim.TIM_Period            = 300;      // 最大 300us
    tim.TIM_ClockDivision     = 0;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &tim);

    /* PWM 配置 */
    TIM_OCInitTypeDef oc;
    oc.TIM_OCMode      = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OCPolarity  = TIM_OCPolarity_High;
    oc.TIM_Pulse       = 0;

    TIM_OC1Init(TIM1, &oc);
    TIM_OC2Init(TIM1, &oc);
    TIM_OC3Init(TIM1, &oc);
    TIM_OC4Init(TIM1, &oc);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM1, ENABLE);

    /* 高级定时器需要开启输出 */
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    TIM_Cmd(TIM1, ENABLE);
}

/* 单路写入 */
void OneShot_Write(Motor_t motor, uint16_t throttle_us)
{
    if (throttle_us < 1000) throttle_us = 1000;
    if (throttle_us > 2000) throttle_us = 2000;

    uint16_t oneshot = throttle_us / 8;  // 125~250 us

    switch (motor)
    {
        case MOTOR1: TIM1->CCR1 = oneshot; break;
        case MOTOR2: TIM1->CCR2 = oneshot; break;
        case MOTOR3: TIM1->CCR3 = oneshot; break;
        case MOTOR4: TIM1->CCR4 = oneshot; break;
    }
}

/* 同时写入四路 */
void OneShot_WriteAll(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4)
{
    OneShot_Write(MOTOR1, m1);
    OneShot_Write(MOTOR2, m2);
    OneShot_Write(MOTOR3, m3);
    OneShot_Write(MOTOR4, m4);
}
