#include "stm32f4xx.h"
#include "LED.h"
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "MS5611.h"
int16_t AccelX,AccelY,AccelZ,GyroX,GyroY,GyroZ;
uint8_t MagX,MagY,MagZ;
float Temprature,Press;
uint8_t id;
/*OLED测试*/
// int main(void)
// {
//     RCC_Configuration();
//     OLED_Init();
//     OLED_ShowString(1,1,"HELLO",OLED_8X16);
//     OLED_Update();
//     while (1)
//     {
//         /* code */
//     }
    
// }
/*MPU6050测试*/
int main(void) {
    // 1. 初始化系统时钟为84MHz（必须先执行，确保外设和延时正常）
    // RCC_Configuration();

    OLED_Init();

    MPU6050_Init();
    while (1) 
    {
        MPU6050_GetData(&AccelX,&AccelY,&AccelZ,&GyroX,&GyroY,&GyroZ);
        OLED_ShowSignedNum(1,1,AccelX,5,OLED_8X16);
        OLED_ShowSignedNum(2,1,AccelY,5,OLED_8X16);
        OLED_ShowSignedNum(3,1,AccelZ,5,OLED_8X16);

        OLED_ShowSignedNum(1,7,GyroX,5,OLED_8X16);
        OLED_ShowSignedNum(2,7,GyroY,5,OLED_8X16);
        OLED_ShowSignedNum(3,7,GyroZ,5,OLED_8X16);
        OLED_Update(); 
    }
}
