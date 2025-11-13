#include "stm32f4xx.h"
#include "LED.h"
#include "Delay.h"
int main(void) {
    // 1. 初始化系统时钟为84MHz（必须先执行，确保外设和延时正常）
    RCC_Configuration();
    
    // 2. 初始化LED（PA5）
    LED_Init();
    
    // 3. 主循环：LED每500ms翻转一次（闪烁）
    while (1) {
        LED_Turn();       // 翻转LED状态
        Delay_ms(1000);    // 延时500ms
    }
}
