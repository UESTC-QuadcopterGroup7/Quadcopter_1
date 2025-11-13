#include "stm32f4xx.h"
#include "HMC5883L.h"
#include "HMC5883L_Reg.h"
#define HMC5883L_BASE_ADDR 0x1E
#define HMC5883L_W_ADDR 0x3C
#define HMC5883L_R_ADDR 0x3D

void HMC5883L_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT){
    uint32_t Timeout = 10000;
    while(I2C_CheckEvent(I2Cx,I2C_EVENT) != SUCCESS){
        Timeout--;
        if(Timeout == 0) return;
    }
}
// 写入寄存器（修正函数调用和地址）
void HMC5883L_WriteReg(uint8_t RegAddress, uint8_t Data){
    I2C_GenerateSTART(I2C2, ENABLE);
    HMC5883L_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

    // 发送7位基础地址+写入操作
    I2C_Send7bitAddress(I2C2, HMC5883L_BASE_ADDR, I2C_Direction_Transmitter);
    HMC5883L_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    I2C_SendData(I2C2, RegAddress); // 发送寄存器地址
    HMC5883L_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED); // 等待发送完成（注意：不是TRANSMITTING）

    I2C_SendData(I2C2, Data); // 发送数据
    HMC5883L_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_GenerateSTOP(I2C2, ENABLE);
}
// 读取寄存器（修正设备地址和函数调用）
uint8_t HMC5883L_ReadReg(uint8_t RegAddress){
    uint8_t Data;
    // 第1步：发送寄存器地址（写操作）
    I2C_GenerateSTART(I2C2, ENABLE);
    HMC5883L_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2, HMC5883L_BASE_ADDR, I2C_Direction_Transmitter);
    HMC5883L_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    I2C_SendData(I2C2, RegAddress); // 告诉从机要读哪个寄存器
    HMC5883L_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    // 第2步：重新发送START，切换到读操作
    I2C_GenerateSTART(I2C2, ENABLE);
    HMC5883L_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

    // 发送7位基础地址+读方向（函数自动处理）
    I2C_Send7bitAddress(I2C2, HMC5883L_BASE_ADDR, I2C_Direction_Receiver);
    HMC5883L_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);

    // 接收单字节：关闭应答，提前发送停止位
    I2C_AcknowledgeConfig(I2C2, DISABLE);
    I2C_GenerateSTOP(I2C2, ENABLE);

    HMC5883L_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED); // 等待接收完成
    Data = I2C_ReceiveData(I2C2);

    I2C_AcknowledgeConfig(I2C2, ENABLE); // 恢复应答（为后续多字节读取准备）
    return Data;
}
void HMC5883L_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

    // 配置GPIO复用映射：PB9->I2C2_SCL，PB10->I2C2_SDA
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);

	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_ClockSpeed = 100000;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C2,&I2C_InitStruct);

	I2C_Cmd(I2C2,ENABLE);

    // 配置传感器（示例：连续测量模式）
    HMC5883L_WriteReg(HMC5883L_MODE, 0x00); // 连续测量模式

}
void HMC5883L_GetData(int16_t *MagX,int16_t *MagY,int16_t *MagZ)
{
    uint8_t DataH,DataL;
    DataH = HMC5883L_ReadReg(HMC5883L_DATA_X_MSB);
    DataL = HMC5883L_ReadReg(HMC5883L_DATA_X_LSB);
    *MagX = (DataH << 8) | DataL;

    DataH = HMC5883L_ReadReg(HMC5883L_DATA_Y_MSB);
    DataL = HMC5883L_ReadReg(HMC5883L_DATA_Y_LSB);
    *MagY = (DataH << 8) | DataL;

        
    DataH = HMC5883L_ReadReg(HMC5883L_DATA_Z_MSB);
    DataL = HMC5883L_ReadReg(HMC5883L_DATA_Z_LSB);
    *MagZ = (DataH << 8) | DataL;

    
}
uint8_t HMC5883L_GetID(void){
    return HMC5883L_ReadReg(HMC5883L_WHO_AM_I_VAL);
}
