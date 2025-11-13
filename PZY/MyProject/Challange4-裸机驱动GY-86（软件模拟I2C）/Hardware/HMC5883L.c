#include "stm32f4xx.h"
#include "MyI2C.h"
#include "MPU6050.h"  // 依赖MPU6050的旁路模式函数
#include "HMC5883L_Reg.h"
#include "Delay.h"

// -------------------------- HMC5883L 基础参数（默认地址，与GY86匹配）--------------------------
#define HMC5883L_W_ADDR 0x3C  // 7位地址0x1E（AD0接GND）→ 写地址=0x1E<<1=0x3C
#define HMC5883L_R_ADDR 0x3D  // 读地址=0x3C | 0x01=0x3D

// -------------------------- HMC5883L 寄存器读写函数--------------------------
// 向HMC5883L指定寄存器写1字节
void HMC5883L_WriteReg(uint8_t RegAddress, uint8_t Data) {
    MyI2C_Start();
    MyI2C_SendByte(HMC5883L_W_ADDR);  // 发送HMC5883L写地址
    MyI2C_ReceiveAck();
    MyI2C_SendByte(RegAddress);        // 发送寄存器地址
    MyI2C_ReceiveAck();
    MyI2C_SendByte(Data);              // 发送配置数据
    MyI2C_ReceiveAck();
    MyI2C_Stop();
}

// 从HMC5883L指定寄存器读1字节
uint8_t HMC5883L_ReadReg(uint8_t RegAddress) {
    uint8_t Data = 0;
    MyI2C_Start();
    MyI2C_SendByte(HMC5883L_W_ADDR);  // 先发送写地址（指定寄存器）
    MyI2C_ReceiveAck();
    MyI2C_SendByte(RegAddress);        // 指定要读取的寄存器
    MyI2C_ReceiveAck();
    
    MyI2C_Start();
    MyI2C_SendByte(HMC5883L_R_ADDR);  // 切换为读地址
    MyI2C_ReceiveAck();
    Data = MyI2C_ReceiveByte();        // 读取数据
    MyI2C_SendAck(1);                  // 非应答：告知传感器停止发送
    MyI2C_Stop();
    return Data;
}

// -------------------------- HMC5883L 初始化（依赖MPU6050旁路配置）--------------------------
void HMC5883L_Init(void) {
    MyI2C_Init();                      // 初始化I2C总线（与MPU6050共用，无需重复初始化）
    MPU6050_EnableBypassMode();        // 关键：先解锁MPU6050的辅助I2C总线
    
    // 配置HMC5883L核心寄存器（与你原有配置一致，确保兼容性）
    HMC5883L_WriteReg(HMC5883L_CONFIG_A, HMC5883L_SAMPLES_8 | HMC5883L_DATA_RATE_15 | HMC5883L_MEAS_NORMAL);
    Delay_ms(1);  // 等待配置生效
    
    HMC5883L_WriteReg(HMC5883L_CONFIG_B, HMC5883L_GAIN_130);  // ±1.3Ga高精度模式
    Delay_ms(1);
    
    HMC5883L_WriteReg(HMC5883L_MODE, HMC5883L_MODE_SINGLE);   // 单次测量触发复位
    Delay_ms(5);
    HMC5883L_WriteReg(HMC5883L_MODE, HMC5883L_MODE_CONTINUOUS);  // 切换为连续测量
    Delay_ms(10);  // 等待传感器稳定
}

// -------------------------- HMC5883L 数据读取函数--------------------------
// 获取三轴磁场数据（注意：HMC5883L默认输出顺序为X→Z→Y，需按实际布局调整）
void HMC5883L_GetData(int16_t *MagX, int16_t *MagY, int16_t *MagZ) {
    uint8_t DataXH, DataXL;
    uint8_t DataZH, DataZL;
    uint8_t DataYH, DataYL;
    
    // 读取X轴（16位，高位在前）
    DataXH = HMC5883L_ReadReg(HMC5883L_DATA_X_MSB);
    DataXL = HMC5883L_ReadReg(HMC5883L_DATA_X_LSB);
    *MagX = (int16_t)((DataXH << 8) | DataXL);
    
    // 读取Z轴
    DataZH = HMC5883L_ReadReg(HMC5883L_DATA_Z_MSB);
    DataZL = HMC5883L_ReadReg(HMC5883L_DATA_Z_LSB);
    *MagZ = (int16_t)((DataZH << 8) | DataZL);
    
    // 读取Y轴
    DataYH = HMC5883L_ReadReg(HMC5883L_DATA_Y_MSB);
    DataYL = HMC5883L_ReadReg(HMC5883L_DATA_Y_LSB);
    *MagY = (int16_t)((DataYH << 8) | DataYL);
}

// 获取HMC5883L ID（用于验证连接，正常应为0x483433）
uint32_t HMC5883L_GetID(void) {
    uint8_t idA = HMC5883L_ReadReg(HMC5883L_ID_A);
    uint8_t idB = HMC5883L_ReadReg(HMC5883L_ID_B);
    uint8_t idC = HMC5883L_ReadReg(HMC5883L_ID_C);
    return (idA << 16) | (idB << 8) | idC;
}