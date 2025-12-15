#include "stm32f4xx.h"
#include <stdio.h>

#include "Delay.h"
#include "MyUSART.h"
#include "Mysystem_clock.h"
#include "MyPPM_temp.h"   // PPM 输入（PA0 / TIM2_CH1）

/* ===== PWM ===== */
void PWM_Init_PA8(void);
void PWM_Write(uint16_t us);

#define kDebugPort USART2_PORT

static void System_Init(void);
static void LogPPM(void);
static uint16_t MapThrottle(uint16_t ppm);

int main(void)
{
    System_Init();
    PWM_Init_PA8();

    USART_SendString(kDebugPort, "=== PWM + PPM initialized ===\r\n");

    while (1)
    {
        if (PPM_FrameReady())
        {
            uint16_t ch3 = PPM_GetChannel(2);   // RC 油门
            uint16_t pwm_us = MapThrottle(ch3);

            PWM_Write(pwm_us);
            LogPPM();
        }

        Delay_ms(20);
    }
}

/* ================================ */
/* 系统初始化                       */
/* ================================ */
static void System_Init(void)
{
    SystemClock_Config_HSI();
    SystemCoreClockUpdate();
    Delay_Init();
    MyUSART_Init(kDebugPort, 115200);

    PPM_Init();   // TIM2_CH1 (PA0)
}

/* ================================ */
/* 显示 PPM 值                       */
/* ================================ */
static void LogPPM(void)
{
    char buf[128];
    snprintf(buf, sizeof(buf),
             "CH1=%4d CH2=%4d CH3=%4d CH4=%4d\r\n",
             PPM_GetChannel(0), PPM_GetChannel(1),
             PPM_GetChannel(2), PPM_GetChannel(3));

    USART_SendString(kDebugPort, buf);
}

/* ================================ */
/* PPM → PWM（1000–2000us）         */
/* ================================ */
static uint16_t MapThrottle(uint16_t ppm)
{
    if (ppm < 990) return 1000;
    if (ppm > 2000) return 2000;
    return ppm;
}


/* ================================ */
/* 写入 PWM 脉宽（单位：us）        */
/* ================================ */
void PWM_Write(uint16_t us)
{
    TIM1->CCR1 = us;
}
