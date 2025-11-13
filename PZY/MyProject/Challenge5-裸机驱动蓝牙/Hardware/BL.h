#ifndef __BL_H
#define __BL_H
void BL_Init(void);
void BL_SendByte(uint8_t Byte);
void BL_SendArray(uint8_t *Array,uint16_t Length);
void BL_SendString(char *String);
uint32_t BL_Pow(uint32_t X, uint32_t Y);
void BL_SendNumber(uint32_t Number, uint8_t Length);
void BL_Printf(char *format, ...);
uint8_t BL_GetRxData(void);
uint8_t BL_GetRxFlag(void);
void USART2_IRQHandler(void);
#endif

