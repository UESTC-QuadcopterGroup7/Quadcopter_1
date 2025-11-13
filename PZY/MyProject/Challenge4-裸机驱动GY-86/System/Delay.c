#include "Delay.h"

void RCC_Configuration(void) {
  RCC_DeInit();
  // 尝试启动HSE，失败则切换到HSI
  RCC_HSEConfig(RCC_HSE_ON);
  if (RCC_WaitForHSEStartUp() == SUCCESS) {
    // HSE模式：8MHz → PLL配置为84MHz
    RCC_PLLConfig(RCC_PLLSource_HSE, 8, 168, 2, 7);
  } else {
    // HSI模式：16MHz → PLL配置为84MHz
    RCC_HSEConfig(RCC_HSE_OFF);
    RCC_HSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
    RCC_PLLConfig(RCC_PLLSource_HSI, 16, 168, 2, 7);
  }
  
  // 使能PLL并等待就绪
  RCC_PLLCmd(ENABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  
  // 配置系统时钟为PLL输出（84MHz）
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  // 配置总线分频
  RCC_HCLKConfig(RCC_SYSCLK_Div1);    // AHB = 84MHz
  RCC_PCLK1Config(RCC_HCLK_Div2);     // APB1 = 42MHz
  RCC_PCLK2Config(RCC_HCLK_Div1);     // APB2 = 84MHz
  
  // 更新系统时钟变量
  SystemCoreClockUpdate();
}

// 微秒级延时（84MHz下精准实现）
void Delay_us(uint32_t us) {
  if (us == 0) return;
  
  // 配置SysTick时钟源为AHB（84MHz），1us = 84个周期
  SysTick->LOAD = (uint32_t)(us * 84 - 1);
  SysTick->VAL = 0;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
  
  // 等待计数结束
  while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
  
  // 关闭计数器并清零
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  SysTick->VAL = 0;
}

// 毫秒级延时（基于84MHz精准计算）
void Delay_ms(uint32_t ms) {
  if (ms == 0) return;
  
  uint32_t totalTicks = (uint32_t)ms * 84000; // 1ms = 84000个周期
  
  while (totalTicks > 0) {
    // 单次最大延时约200ms（避免24位计数器溢出）
    uint32_t ticks = (totalTicks > 16777215) ? 16777215 : totalTicks;
    
    SysTick->LOAD = ticks - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0;
    
    totalTicks -= ticks;
  }
}