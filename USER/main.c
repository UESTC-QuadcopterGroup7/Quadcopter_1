#include "stm32f4xx.h"
#include "Mysystem_clock.h"
#include "Delay.h"
#include "MyUSART.h"
#include "SensorManager.h"

int main(void)
{
    SystemClock_Config_HSI();
    Delay_Init();
    MyUSART_Init(USART2_PORT, 57600);

    MyUSART_SendString(USART2_PORT, "=== GY87 Sensor System Start ===\r\n");

    if (SensorManager_Init() != 0) {
        MyUSART_SendString(USART2_PORT, "Sensor initialization failed!\r\n");
        while (1) {
        }
    }

    MyUSART_SendString(USART2_PORT, "Sensors ready. Streaming data...\r\n");

    SensorFrame frame;
    char line_buffer[160];

    while (1) {
        SensorManager_Read(&frame);
        SensorManager_Format(&frame, line_buffer, sizeof(line_buffer));
        MyUSART_SendString(USART2_PORT, line_buffer);
        Delay_ms(100);
    }
}
