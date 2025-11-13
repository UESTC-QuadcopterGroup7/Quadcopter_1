#include "stm32f4xx.h"
#include "LED.h"
#include "Delay.h"
#include "OLED.h"
#include "BL.h"
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
/*串口收发测试*/
// extern uint8_t BL_TxPacket[4];				
// extern uint8_t BL_RxPacket[4];				
// int main(void)
// {
//     RCC_Configuration();
//     OLED_Init();
//     BL_Init();
//     BL_SendByte(1);
//     while(1)
//     {
//         if(BL_GetRxFlag() == SET)
//         {
//             OLED_ShowHexNum(1,1,BL_RxPacket[0],2,OLED_8X16);
//             OLED_ShowHexNum(17,1,BL_RxPacket[1],2,OLED_8X16);
//             OLED_ShowHexNum(33,1,BL_RxPacket[2],2,OLED_8X16);
//             OLED_ShowHexNum(48,1,BL_RxPacket[3],2,OLED_8X16);
//             OLED_Update();
//         }
//     }
// }
/*蓝牙测试*/
extern uint8_t BL_RxPacket[4];	
int main()
{
    OLED_Init();
    BL_Init();

    while(1)
    {
        if(BL_GetRxFlag == SET)
        {
            OLED_ShowHexNum(1,1,BL_RxPacket[0],2,OLED_8X16);
            OLED_ShowHexNum(17,1,BL_RxPacket[1],2,OLED_8X16);
            OLED_ShowHexNum(33,1,BL_RxPacket[2],2,OLED_8X16);
            OLED_ShowHexNum(48,1,BL_RxPacket[3],2,OLED_8X16);            
        }
    }
}