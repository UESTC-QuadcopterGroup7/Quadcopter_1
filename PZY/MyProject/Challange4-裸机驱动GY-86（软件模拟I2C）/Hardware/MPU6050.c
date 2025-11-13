#include "stm32f4xx.h"
#include "MyI2C.h"
#include "MPU6050_Reg.h"
#define MPU6050_ADDRESS 0xD0
/*指定MPU6050的一个寄存器写一个字节*/
void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data){
    MyI2C_Start();
    MyI2C_SendByte(MPU6050_ADDRESS);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(RegAddress);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(Data);
    MyI2C_ReceiveAck();
    MyI2C_Stop();

}
uint8_t MPU6050_ReadReg(uint8_t RegAddress){
    uint8_t Data;
    MyI2C_Start();
    MyI2C_SendByte(MPU6050_ADDRESS);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(RegAddress);
    MyI2C_ReceiveAck();

    MyI2C_Start();
    MyI2C_SendByte(MPU6050_ADDRESS | 0x01);
    MyI2C_ReceiveAck();
    Data = MyI2C_ReceiveByte();
    MyI2C_SendAck(1);
    MyI2C_Stop();
    return Data;
}
void MPU6050_Init(void){
    MyI2C_Init();
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);

    MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);
    MPU6050_WriteReg(MPU6050_CONFIG,0x06);

    MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);
}
void MPU6050_GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,
                    int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ){
    uint8_t DataH,DataL;
    DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
    *AccX = (DataH << 8) | DataL;

    DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
    *AccY = (DataH << 8) | DataL;

    DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
    *AccZ = (DataH << 8) | DataL;

    DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
    *GyroX = (DataH << 8) | DataL;

    DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
    *GyroY = (DataH << 8) | DataL;

    DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
    *GyroZ =(DataH << 8) | DataL;
}
uint8_t MPU6050_GetID(void){
    return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}


// -------------------------- 新增：使能MPU6050 I2C旁路模式（核心！）--------------------------
// 功能：让MPU6050的辅助I2C总线（接HMC5883L）直通主I2C总线，允许STM32直接访问HMC5883L
void MPU6050_EnableBypassMode(void){
    // 1. 先唤醒MPU6050（确保模块未休眠，辅助总线有供电）
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);  // 与你原有初始化一致，避免冲突
    Delay_ms(5);  // 等待电源稳定

    // 2. 配置INT_PIN_CFG寄存器：BIT1=1（使能旁路模式），其他位默认0
    // 寄存器说明：INT_PIN_CFG[1] = I2C_BYPASS_EN，置1后辅助I2C与主I2C连通
    MPU6050_WriteReg(MPU6050_INT_PIN_CFG, 0x02);  
    Delay_ms(2);  // 等待旁路模式生效
}
