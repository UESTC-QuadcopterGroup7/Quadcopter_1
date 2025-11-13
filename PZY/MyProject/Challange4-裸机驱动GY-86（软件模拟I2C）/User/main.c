#include "stm32f4xx.h"
#include "LED.h"
#include "Delay.h"
#include "OLED.h"
#include "MyI2C.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "MS5611.h"
/*LED测试*/
// int main(void) {
//     // 1. 初始化系统时钟为84MHz（必须先执行，确保外设和延时正常）
//     RCC_Configuration();
    
//     // 2. 初始化LED（PA5）
//     LED_Init();
    
//     // 3. 主循环：LED每500ms翻转一次（闪烁）
//     while (1) {
//         LED_Turn();       // 翻转LED状态
//         Delay_ms(1000);    // 延时500ms
//     }
// }
/*OLED测试*/
// int main(void)
// {
//     RCC_Configuration();
//     OLED_Init();
//     OLED_ShowString(1,1,"TEST",OLED_8X16);
//     OLED_Update();
//     while (1)
//     {
//         /* code */
//     }
    
// }
/*MPU6050测试*/
// int16_t AccelX,AccelY,AccelZ,GyroX,GyroY,GyroZ;
// int main(void)
// {
//     RCC_Configuration();
//     OLED_Init();
//     MPU6050_Init();
//     uint8_t id = MPU6050_GetID();
//     OLED_ShowString(1,1,"ID:0x",OLED_8X16);
//     OLED_ShowHexNum(41,1,id,2,OLED_8X16);
//     OLED_Update();
//     while (1)
//     {
//         MPU6050_GetData(&AccelX,&AccelY,&AccelZ,&GyroX,&GyroY,&GyroZ);
//         OLED_ShowSignedNum(1,17,AccelX,5,OLED_8X16);
//         OLED_ShowSignedNum(1,33,AccelY,5,OLED_8X16);
//         OLED_ShowSignedNum(1,49,AccelZ,5,OLED_8X16);

//         OLED_ShowSignedNum(41,17,GyroX,5,OLED_8X16);
//         OLED_ShowSignedNum(41,33,GyroY,5,OLED_8X16);
//         OLED_ShowSignedNum(41,49,GyroZ,5,OLED_8X16);  
//         OLED_Update();
//         Delay_ms(10);      
//     }
    
// }
/*MS5611测试*/
// int32_t Pressure,Temperature;
// int main(void)
// {
//     RCC_Configuration();
//     OLED_Init();
//     MS5611_Init();
//     uint8_t id = MPU6050_GetID();
//     OLED_ShowString(1,1,"MS5611 TEST",OLED_8X16);
//     OLED_Update();
//     while (1)
//     {
//         MS5611_GetData(&Pressure,&Temperature);
//         OLED_ShowSignedNum(1,17,Pressure,8,OLED_8X16);
//         OLED_ShowSignedNum(1,33,Temperature,8,OLED_8X16);

//         OLED_Update();
//         Delay_ms(10);      
//     }
    
// }



int main(void) {
    OLED_Init();
    MPU6050_Init();
    // 新增：读取MPU6050的INT_PIN_CFG寄存器，验证旁路模式是否生效
    uint8_t int_pin_cfg = MPU6050_ReadReg(0x37);
    OLED_ShowString(1, 1, "MPU_CFG:", OLED_8X16);
    OLED_ShowHexNum(1, 17, int_pin_cfg, 2, OLED_8X16);  // 正常应显示0x02（BIT1=1）
    OLED_Update();
    Delay_ms(1000);
    
    HMC5883L_Init();
    uint32_t hmc_id = HMC5883L_GetID();
    OLED_ShowString(9, 1, "HMC ID:", OLED_8X16);
    OLED_ShowHexNum(9, 17, hmc_id, 6, OLED_8X16);
    OLED_Update();
    while(1);
}