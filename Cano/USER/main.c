/*
#include "MyMPU6050.h"
#include "Mysystem_clock.h"
#include <stdio.h>

int16_t AccX, AccY, AccZ;
int16_t GyroX, GyroY, GyroZ;
int16_t MagX, MagY, MagZ;
float Temperature;

#define HMC5883L_ADDRESS 0x3C
#define HMC5883L_CONFIG_A 0x00
#define HMC5883L_CONFIG_B 0x01
#define HMC5883L_MODE 0x02
#define HMC5883L_DATA_X_MSB 0x03

// I2C写函数
void I2C_Write(uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
    // 发送起始条件
    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));
    
    // 发送设备地址(写模式)
    I2C1->DR = devAddr;
    while(!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;
    
    // 发送寄存器地址
    while(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = regAddr;
    
    // 发送数据
    while(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = data;
    
    // 等待传输完成
    while(!(I2C1->SR1 & I2C_SR1_BTF));
    
    // 发送停止条件
    I2C1->CR1 |= I2C_CR1_STOP;
}

// I2C读函数
uint8_t I2C_Read(uint8_t devAddr, uint8_t regAddr)
{
    uint8_t data;
    
    // 发送起始条件
    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));
    
    // 发送设备地址(写模式)
    I2C1->DR = devAddr;
    while(!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;
    
    // 发送寄存器地址
    while(!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = regAddr;
    
    // 重新发送起始条件
    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));
    
    // 发送设备地址(读模式)
    I2C1->DR = devAddr | 0x01;
    while(!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;
    
    // 设置停止条件
    I2C1->CR1 &= ~I2C_CR1_ACK;
    I2C1->CR1 |= I2C_CR1_STOP;
    
    // 读取数据
    while(!(I2C1->SR1 & I2C_SR1_RXNE));
    data = I2C1->DR;
    
    return data;
}


// HMC5883L初始化
void HMC5883L_Init(void)
{
    MPU6050_EnableBypass();
    
    // 设置配置寄存器A
    I2C_Write(HMC5883L_ADDRESS, HMC5883L_CONFIG_A, 0x78);
    
    // 设置配置寄存器B
    I2C_Write(HMC5883L_ADDRESS, HMC5883L_CONFIG_B, 0xA0);
    
    // 设置模式寄存器
    I2C_Write(HMC5883L_ADDRESS, HMC5883L_MODE, 0x00);
    
    MPU6050_DisableBypass();
}

// 读取HMC5883L数据
void HMC5883L_GetData(int16_t *MagX, int16_t *MagY, int16_t *MagZ)
{
    uint8_t buffer[6];
    
    MPU6050_EnableBypass();
    
    // 读取6个字节的磁力计数据
    for(int i = 0; i < 6; i++)
    {
        buffer[i] = I2C_Read(HMC5883L_ADDRESS, HMC5883L_DATA_X_MSB + i);
    }
    
    *MagX = (buffer[0] << 8) | buffer[1];
    *MagZ = (buffer[2] << 8) | buffer[3];
    *MagY = (buffer[4] << 8) | buffer[5];
    
    MPU6050_DisableBypass();
}


// USART初始化(用于串口通信)

void USART2_Init(void)
{
    // 使能GPIOA和USART2时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    
    // 配置PA2(TX), PA3(RX)
    GPIOA->MODER |= (2 << 4) | (2 << 6);
    GPIOA->AFR[0] |= (7 << 8) | (7 << 12);
    
    // 配置USART2
    USART2->BRR = 42000000  / 57600;
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void USART2_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    // 2. 配置GPIO引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;  // PA9(TX), PA10(RX)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;             // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             // 上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 3. 引脚复用映射
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);   // PA9 -> USART1_TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);  // PA10 -> USART1_RX
    
    // 4. USART参数配置
    USART_InitStructure.USART_BaudRate = 9600;                    // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;     // 8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;          // 1位停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;             // 无校验
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    
    USART_Init(USART1, &USART_InitStructure);
    
    // 5. 使能USART
    USART_Cmd(USART1, ENABLE);
}

void SystemClock_Config_PLL(void)
{
    // 1. 使能HSI
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));
    
    // 2. 配置PLL
    // HSI = 16MHz, 目标系统时钟 = 84MHz
    // PLL配置: 
    // - PLL_M = 16 (HSI先16分频得到1MHz)
    // - PLL_N = 336 (倍频到336MHz)
    // - PLL_P = 4 (4分频得到84MHz系统时钟)
    RCC->PLLCFGR = (16 << 0)  |  // PLL_M = 16
                   (336 << 6) |  // PLL_N = 336
                   (1 << 16)  |  // PLL_P = 4 (01=4分频)
                   (7 << 24)  |  // PLL_Q = 7
                   RCC_PLLCFGR_PLLSRC_HSI;  // 使用HSI作为PLL源
    
    // 3. 使能PLL
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));
    
    // 4. 配置预分频器
    RCC->CFGR &= ~(0xFFFFFFFF);  // 清除所有配置
    RCC->CFGR |= (0 << 4)    |   // HPRE = 0 (不分频) AHB = 84MHz
                 (4 << 10)   |   // PPRE1 = 4 (2分频) APB1 = 42MHz
                 (0 << 13);      // PPRE2 = 0 (不分频) APB2 = 84MHz
    
    // 5. Flash延迟配置（84MHz需要2个等待状态）
    FLASH->ACR = FLASH_ACR_LATENCY_2WS | FLASH_ACR_ICEN | FLASH_ACR_DCEN;
    
    // 6. 切换到PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
    
    // 更新系统时钟变量
    SystemCoreClock = 84000000;  // 84MHz
}

// USART发送字符
void USART_SendChar(char c)
{
    while(!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

// USART发送字符串
void USART_SendString(char *str)
{
    while(*str)
    {
        USART_SendChar(*str++);
    }
}

// 格式化发送传感器数据
void SendSensorData(void)
{
    char buffer[128];
    
    // 读取传感器数据
    MPU6050_GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);
    HMC5883L_GetData(&MagX, &MagY, &MagZ);
    
    // 格式化数据
    sprintf(buffer, "ACC:%d,%d,%d GYRO:%d,%d,%d MAG:%d,%d,%d\n", 
            AccX, AccY, AccZ, GyroX, GyroY, GyroZ, MagX, MagY, MagZ);
    
    // 发送数据
    USART_SendString(buffer);
}

// 系统时钟配置
void SystemClock_Config(void)
{
    // 设置HSE为系统时钟源
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));
    
    // 配置PLL
    RCC->PLLCFGR = (8 << 0) | (336 << 6) | (1 << 16) | (7 << 24) | RCC_PLLCFGR_PLLSRC_HSE;
    
    // 使能PLL
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));
    
    // 设置分频
    //RCC->CFGR = (4 << 10) | (0 << 13);
	
    RCC->CFGR &= ~(RCC_CFGR_HPRE_Msk | RCC_CFGR_PPRE1_Msk | RCC_CFGR_PPRE2_Msk);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1  |  // AHB = 84MHz
                 RCC_CFGR_PPRE1_DIV2 |  // APB1 = 42MHz
                 RCC_CFGR_PPRE2_DIV1;   // APB2 = 84MHz
	
	RCC->CFGR &= ~(0xF0F0000);  // 清除HPRE, PPRE1, PPRE2位
    RCC->CFGR |= (0 << 4)    |  // HPRE = 0 (不分频) AHB = 84MHz
                 (4 << 10)   |  // PPRE1 = 4 (2分频) APB1 = 42MHz
                 (0 << 13);     // PPRE2 = 0 (不分频) APB2 = 84MHz
	
    // 选择PLL作为系统时钟
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	
	// 配置Flash延迟
    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_2WS;

	// 切换到PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

// 延时函数
void Delay(uint32_t time)
{
    while(time--)
    {
        __NOP();
    }
}

int main(void)
{
    // 系统初始化
    SystemClock_Config_HSI();
	MPU6050_Init();
    USART2_Init();
    
        USART_SendString("=== GY87 Sensor System Start ===\r\n");
    
    // 检查MPU6050 ID
    uint8_t id = MPU6050_GetID();
    char buffer[50];
    sprintf(buffer, "MPU6050 ID: 0x%02X\r\n", id);
    USART_SendString(buffer);
    
    if(id != 0x68)
    {
        USART_SendString("MPU6050 Init Failed!\r\n");
        while(1);
    }
    
    // 初始化HMC5883L
    HMC5883L_Init();
    USART_SendString("HMC5883L Init Success!\r\n");
    
    USART_SendString("=== Starting Sensor Data Acquisition ===\r\n");
    
    while(1)
    {
        SendSensorData();
        Delay(100);  // 延时约100ms
    }
}

#include "stm32f4xx.h"
#include "Mysystem_clock.h"
#include "Delay.h"
#include "MyI2C.h"
//#include "MyUSART.h"
//#include "MyMPU6050.h"
#include <stdio.h>

// 函数声明
void SystemClock_Config_HSI(void);
void Delay_ms(uint32_t ms);
void I2C1_Init(void);
uint8_t I2C_Write(uint8_t devAddr, uint8_t regAddr, uint8_t data);
uint8_t I2C_Read(uint8_t devAddr, uint8_t regAddr);
void I2C_ReadMultiple(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t length);

// 系统时钟配置 - 使用HSI
void SystemClock_Config_HSI(void) {
    // 复位RCC配置
    RCC_DeInit();
    
    // 使能HSI
    RCC_HSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
    
    // 配置AHB、APB分频
    RCC_HCLKConfig(RCC_SYSCLK_Div1);    // AHB = 16MHz
    RCC_PCLK1Config(RCC_HCLK_Div2);     // APB1 = 8MHz
    RCC_PCLK2Config(RCC_HCLK_Div1);     // APB2 = 16MHz
    
    // 选择HSI作为系统时钟
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
    while(RCC_GetSYSCLKSource() != 0x00);
    
    // 更新SystemCoreClock变量
    SystemCoreClockUpdate();
}

// 简单延时函数
void Delay_ms(uint32_t ms) {
    for(uint32_t i = 0; i < ms; i++) {
        for(uint32_t j = 0; j < 4000; j++) {
            __NOP();
        }
    }
}

// I2C1初始化
void I2C1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    
    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
    // 2. 配置GPIO引脚 PB6(SCL), PB7(SDA)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 3. 引脚复用
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
    
    // 4. I2C配置
    I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;  // 400kHz
    
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
}

// USART1初始化 - 115200波特率
void USART1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    // 2. 配置GPIO引脚 PA9(TX), PA10(RX)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 3. 引脚复用
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
    
    // 4. USART配置 - 115200波特率
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

// USART发送字符
void USART_SendChar(char c) {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (uint8_t)c);
}

// USART发送字符串
void USART_SendString(const char *str) {
    while(*str) {
        USART_SendChar(*str++);
    }
}

// I2C写函数
uint8_t I2C_Write(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    // 等待I2C空闲
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    
    // 发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    // 发送设备地址(写)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
    // 发送寄存器地址
    I2C_SendData(I2C1, regAddr);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    // 发送数据
    I2C_SendData(I2C1, data);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    // 发送停止条件
    I2C_GenerateSTOP(I2C1, ENABLE);
    
    return 0;
}

// I2C读单个寄存器
uint8_t I2C_Read(uint8_t devAddr, uint8_t regAddr) {
    uint8_t data;
    
    // 等待I2C空闲
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    
    // 发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    // 发送设备地址(写)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
    // 发送寄存器地址
    I2C_SendData(I2C1, regAddr);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    // 重新发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    // 发送设备地址(读)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
    // 禁用ACK
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    
    // 发送停止条件
    I2C_GenerateSTOP(I2C1, ENABLE);
    
    // 读取数据
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    data = I2C_ReceiveData(I2C1);
    
    // 重新启用ACK
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    
    return data;
}

// I2C连续读取多个寄存器
void I2C_ReadMultiple(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t length) {
    // 等待I2C空闲
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    
    // 发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    // 发送设备地址(写)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
    // 发送起始寄存器地址
    I2C_SendData(I2C1, regAddr);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    // 重新发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    // 发送设备地址(读)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
    // 连续读取数据
    for(uint8_t i = 0; i < length; i++) {
        if(i == length - 1) {
            // 最后一个字节，禁用ACK并发送停止
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
        data[i] = I2C_ReceiveData(I2C1);
    }
    
    // 重新启用ACK
    I2C_AcknowledgeConfig(I2C1, ENABLE);
}

// MPU6050初始化
void MPU6050_Init(void) {
    // 唤醒MPU6050
    I2C_Write(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, 0x00);
    Delay_ms(100);
    
    // 配置陀螺仪 ±500°/s
    I2C_Write(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, 0x08);
    
    // 配置加速度计 ±4g
    I2C_Write(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, 0x08);
    
    // 配置DLPF
    I2C_Write(MPU6050_ADDRESS, MPU6050_CONFIG, 0x03);
    
    Delay_ms(100);
}

// 获取MPU6050 ID
uint8_t MPU6050_GetID(void) {
    return I2C_Read(MPU6050_ADDRESS, MPU6050_WHO_AM_I);
}

// 读取MPU6050数据
void MPU6050_GetData(int16_t* AccX, int16_t* AccY, int16_t* AccZ, 
                     int16_t* GyroX, int16_t* GyroY, int16_t* GyroZ, 
                     int16_t* Temp) {
    uint8_t buffer[14];
    
    // 连续读取14个字节的传感器数据
    I2C_ReadMultiple(MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_H, buffer, 14);
    
    // 组合数据 (高位在前)
    *AccX = (buffer[0] << 8) | buffer[1];
    *AccY = (buffer[2] << 8) | buffer[3];
    *AccZ = (buffer[4] << 8) | buffer[5];
    *Temp = (buffer[6] << 8) | buffer[7];
    *GyroX = (buffer[8] << 8) | buffer[9];
    *GyroY = (buffer[10] << 8) | buffer[10];
    *GyroZ = (buffer[12] << 8) | buffer[13];
}

// 发送传感器数据
void SendSensorData(void) {
    char buffer[128];
    
    // 读取传感器数据
    MPU6050_GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ, &Temperature);
    
    // 格式化数据
    sprintf(buffer, "ACC: X=%6d, Y=%6d, Z=%6d | GYRO: X=%6d, Y=%6d, Z=%6d | TEMP=%6d\r\n", 
            AccX, AccY, AccZ, GyroX, GyroY, GyroZ, Temperature);
    
    // 发送数据
    USART_SendString(buffer);
}

int main(void) {
    // 系统初始化
    SystemClock_Config_HSI();
    I2C1_Init();
    USART1_Init();
    
    // 启动信息
    USART_SendString("\r\n=== MPU6050 Sensor System ===\r\n");
    USART_SendString("Clock: HSI 16MHz\r\n");
    USART_SendString("Baudrate: 115200\r\n");
    USART_SendString("Initializing MPU6050...\r\n");
    
    // 初始化MPU6050
    MPU6050_Init();
    Delay_ms(100);
    
    // 检查MPU6050 ID
    uint8_t id = MPU6050_GetID();
    char id_msg[50];
    sprintf(id_msg, "MPU6050 ID: 0x%02X", id);
    USART_SendString(id_msg);
    
    if(id == 0x68) {
        USART_SendString(" - MPU6050 Found!\r\n");
    } else {
        USART_SendString(" - MPU6050 Not Found!\r\n");
        while(1) {
            USART_SendString("Error: Check MPU6050 connection!\r\n");
            Delay_ms(1000);
        }
    }
    
    USART_SendString("=== Starting Data Acquisition ===\r\n\r\n");
    
    // 主循环
    while(1) {
        SendSensorData();
        Delay_ms(100);  // 100ms间隔
    }
}
*/
#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include <stdio.h>
#include <string.h>

// 传感器数据变量
int16_t AccX, AccY, AccZ;
int16_t GyroX, GyroY, GyroZ;
int16_t MagX, MagY, MagZ;  // 添加磁力计变量
int16_t Temperature;

// MPU6050寄存器定义
#define MPU6050_ADDRESS 0xD0
#define MPU6050_WHO_AM_I 0x75
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_CONFIG 0x1A
#define MPU6050_GYRO_CONFIG 0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_TEMP_OUT_H 0x41
#define MPU6050_GYRO_XOUT_H 0x43
#define MPU6050_INT_PIN_CFG 0x37
#define MPU6050_USER_CTRL 0x6A

// HMC5883L寄存器定义
#define HMC5883L_ADDRESS 0x3C
#define HMC5883L_CONFIG_A 0x00
#define HMC5883L_CONFIG_B 0x01
#define HMC5883L_MODE 0x02
#define HMC5883L_DATA_X_MSB 0x03

// 简单延时函数
void Delay_ms(uint32_t ms) {
    for(uint32_t i = 0; i < ms; i++) {
        for(uint32_t j = 0; j < 4000; j++) {
            __NOP();
        }
    }
}

// 系统时钟配置 - 使用HSI驱动PLL到84MHz

void SystemClock_Config_HSI(void) {
    // 复位RCC配置
    RCC_DeInit();
    
    // 1. 使能HSI (16MHz)
    RCC_HSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
    
    // 2. 配置PLL参数
    // HSI = 16MHz, PLL_M = 8, PLL_N = 84, PLL_P = 2
    // PLL输出频率 = (16/8) * 84 / 2 = 84MHz
    RCC_PLLConfig(RCC_PLLSource_HSI, 8, 84, 2, 7);
    
    // 3. 使能PLL
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    
    // 4. 配置Flash
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(ENABLE);
    FLASH_InstructionCacheCmd(ENABLE);
    FLASH_DataCacheCmd(ENABLE);
    
    // 5. 配置总线分频器
    RCC_HCLKConfig(RCC_SYSCLK_Div1);    // AHB = 84MHz
    RCC_PCLK1Config(RCC_HCLK_Div2);     // APB1 = 42MHz
    RCC_PCLK2Config(RCC_HCLK_Div1);     // APB2 = 84MHz
    
    // 6. 选择PLL作为系统时钟源
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08);
    
    // 7. 更新系统时钟变量
    SystemCoreClockUpdate();
}


/**
  * @brief  使用HSE配置系统时钟为84MHz
  * @param  无
  * @retval 无
  */
void MySystemClock_Config_HSI_Fallback(void);
  
void SystemClock_Config_HSE(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    
    // 1. 复位RCC时钟配置为默认复位状态
    RCC_DeInit();
    
    // 2. 使能HSE
    RCC_HSEConfig(RCC_HSE_ON);
    
    // 3. 等待HSE准备就绪，并做超时处理
    HSEStatus = RCC_WaitForHSEStartUp();
    
    if (HSEStatus == SUCCESS)
    {
        // 4. 配置Flash预取指、缓存和等待状态
        FLASH_PrefetchBufferCmd(ENABLE);
        FLASH_InstructionCacheCmd(ENABLE);
        FLASH_DataCacheCmd(ENABLE);
        FLASH_SetLatency(FLASH_Latency_2);
        
        // 5. 配置AHB、APB1、APB2分频器
        RCC_HCLKConfig(RCC_SYSCLK_Div1);      // AHB = 84MHz
        RCC_PCLK1Config(RCC_HCLK_Div2);       // APB1 = 42MHz
        RCC_PCLK2Config(RCC_HCLK_Div1);       // APB2 = 84MHz
        
        // 6. 配置PLL
        // HSE = 8MHz, PLL_M = 8, PLL_N = 336, PLL_P = 4, PLL_Q = 7
        // Fvco = 8MHz * 336 / 8 = 336MHz
        // Fpll_out = Fvco / 4 = 84MHz
        // Fpll_48M = Fvco / 7 = 48MHz (用于USB、RNG等)
        RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 4, 7);
        
        // 7. 使能PLL
        RCC_PLLCmd(ENABLE);
        
        // 8. 等待PLL就绪
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }
        
        // 9. 选择PLL作为系统时钟源
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        // 10. 等待PLL被选为系统时钟源
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    {
        // HSE启动失败处理
        // 可以在这里切换到HSI作为后备时钟源
        MySystemClock_Config_HSI_Fallback();
    }
    
    // 11. 更新SystemCoreClock变量
    SystemCoreClockUpdate();
}

/**
  * @brief  HSE失败时的HSI后备配置
  * @param  无
  * @retval 无
  */
void MySystemClock_Config_HSI_Fallback(void)
{
	RCC_DeInit();
    // 1. 配置Flash
    FLASH_PrefetchBufferCmd(ENABLE);
    FLASH_InstructionCacheCmd(ENABLE);
    FLASH_DataCacheCmd(ENABLE);
    FLASH_SetLatency(FLASH_Latency_2);
    
    // 2. 配置总线分频器
    RCC_HCLKConfig(RCC_SYSCLK_Div1);      // AHB = 84MHz
    RCC_PCLK1Config(RCC_HCLK_Div2);       // APB1 = 42MHz
    RCC_PCLK2Config(RCC_HCLK_Div1);       // APB2 = 84MHz
    
    // 3. 配置PLL: HSI = 16MHz, PLL_M = 16, PLL_N = 336, PLL_P = 4
    // Fvco = 16MHz * 336 / 16 = 336MHz
    // Fpll_out = 336MHz / 4 = 84MHz
    RCC_PLLConfig(RCC_PLLSource_HSI, 16, 336, 4, 7);
    
    // 4. 使能PLL
    RCC_PLLCmd(ENABLE);
    
    // 5. 等待PLL就绪
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    
    // 6. 选择PLL作为系统时钟源
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    
    // 7. 等待时钟切换完成
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
	
	SystemCoreClockUpdate();
}

// I2C1初始化 - Nucleo板的I2C引脚
void I2C1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    
    // 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
    // 配置GPIO引脚 - Nucleo板的I2C: PB8(SCL/D15), PB9(SDA/D14)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 引脚复用
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
    
    // I2C配置
    I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;
    
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
}

// USART2初始化 - Nucleo板使用USART2连接到ST-LINK虚拟串口
void USART2_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    // 配置GPIO引脚 - Nucleo板的USART: PA2(TX/D1), PA3(RX/D0)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 引脚复用
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    
    // USART配置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_Odd;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);
}

// USART发送字符
void USART_SendChar(char c) {
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, (uint8_t)c);
}

// USART发送字符串
void USART_SendString(const char *str) {
    while(*str) {
        USART_SendChar(*str++);
    }
}

// I2C写函数
uint8_t I2C_Write(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    // 等待I2C空闲
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    
    // 发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    // 发送设备地址(写)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
    // 发送寄存器地址
    I2C_SendData(I2C1, regAddr);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    // 发送数据
    I2C_SendData(I2C1, data);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    // 发送停止条件
    I2C_GenerateSTOP(I2C1, ENABLE);
    
    return 0;
}

// I2C读单个寄存器
uint8_t I2C_Read(uint8_t devAddr, uint8_t regAddr) {
    uint8_t data;
    
    // 等待I2C空闲
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    
    // 发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    // 发送设备地址(写)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
    // 发送寄存器地址
    I2C_SendData(I2C1, regAddr);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    // 重新发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    // 发送设备地址(读)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
    // 禁用ACK
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    
    // 发送停止条件
    I2C_GenerateSTOP(I2C1, ENABLE);
    
    // 读取数据
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    data = I2C_ReceiveData(I2C1);
    
    // 重新启用ACK
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    
    return data;
}

// I2C连续读取多个寄存器
void I2C_ReadMultiple(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t length) {
    // 等待I2C空闲
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    
    // 发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    // 发送设备地址(写)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
    // 发送起始寄存器地址
    I2C_SendData(I2C1, regAddr);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    // 重新发送起始条件
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    // 发送设备地址(读)
    I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
    // 连续读取数据
    for(uint8_t i = 0; i < length; i++) {
        if(i == length - 1) {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
        data[i] = I2C_ReceiveData(I2C1);
    }
    
    I2C_AcknowledgeConfig(I2C1, ENABLE);
}

// MPU6050初始化
void MPU6050_Init(void) {
    // 唤醒MPU6050
    I2C_Write(MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, 0x00);
    Delay_ms(100);
    
    // 配置陀螺仪 ±500°/s
    I2C_Write(MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, 0x08);
    
    // 配置加速度计 ±4g
    I2C_Write(MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, 0x08);
    
    // 配置DLPF
    I2C_Write(MPU6050_ADDRESS, MPU6050_CONFIG, 0x03);
    
    Delay_ms(100);
}

// 获取MPU6050 ID
uint8_t MPU6050_GetID(void) {
    return I2C_Read(MPU6050_ADDRESS, MPU6050_WHO_AM_I);
}

#define ACCEL_SENSITIVITY  8192.0    // for ±4g
#define GYRO_SENSITIVITY   65.5      // for ±500°/s
#define MAG_SENSITIVITY    1090.0    // HMC5883L ±1.3 Ga (假设)

// 读取MPU6050数据
void MPU6050_GetData(int16_t* AccX, int16_t* AccY, int16_t* AccZ, 
                     int16_t* GyroX, int16_t* GyroY, int16_t* GyroZ, 
                     int16_t* Temp) {
    uint8_t buffer[14];
    
    I2C_ReadMultiple(MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_H, buffer, 14);
    
    *AccX = ((buffer[0] << 8) | buffer[1])/ ACCEL_SENSITIVITY;
    *AccY = ((buffer[2] << 8) | buffer[3])/ ACCEL_SENSITIVITY;
    *AccZ = ((buffer[4] << 8) | buffer[5])/ ACCEL_SENSITIVITY;
    *Temp = (buffer[6] << 8) | buffer[7];
    *GyroX = ((buffer[8] << 8) | buffer[9])/ GYRO_SENSITIVITY;
    *GyroY = ((buffer[10] << 8) | buffer[11])/ GYRO_SENSITIVITY;
    *GyroZ = ((buffer[12] << 8) | buffer[13])/ GYRO_SENSITIVITY;
}

// 启用MPU6050的I2C旁路模式
void MPU6050_EnableBypass(void) {
    I2C_Write(MPU6050_ADDRESS, MPU6050_INT_PIN_CFG, 0x02);
    I2C_Write(MPU6050_ADDRESS, MPU6050_USER_CTRL, 0x00);
    Delay_ms(10);
}

// 禁用MPU6050的I2C旁路模式
void MPU6050_DisableBypass(void) {
    I2C_Write(MPU6050_ADDRESS, MPU6050_INT_PIN_CFG, 0x00);
    Delay_ms(10);
}

// HMC5883L初始化
void HMC5883L_Init(void) {
    MPU6050_EnableBypass();
    
    // 设置配置寄存器A：8样本平均，15Hz输出速率，正常测量
    I2C_Write(HMC5883L_ADDRESS, HMC5883L_CONFIG_A, 0x78);
    
    // 设置配置寄存器B：增益为±1.3Ga
    I2C_Write(HMC5883L_ADDRESS, HMC5883L_CONFIG_B, 0x20);
    
    // 设置模式寄存器：连续测量模式
    I2C_Write(HMC5883L_ADDRESS, HMC5883L_MODE, 0x00);
    
    Delay_ms(100);
    MPU6050_DisableBypass();
}

// 读取HMC5883L数据
void HMC5883L_GetData(int16_t* MagX, int16_t* MagY, int16_t* MagZ) {
    uint8_t buffer[6];
    
    MPU6050_EnableBypass();
    
    // 连续读取6个字节的磁力计数据
    I2C_ReadMultiple(HMC5883L_ADDRESS, HMC5883L_DATA_X_MSB, buffer, 6);
    
    // 组合数据 (高位在前)，注意HMC5883L的数据顺序是X, Z, Y
    *MagX = ((buffer[0] << 8) | buffer[1])/MAG_SENSITIVITY*100;
    *MagZ = ((buffer[2] << 8) | buffer[3])/MAG_SENSITIVITY*100;
    *MagY = ((buffer[4] << 8) | buffer[5])/MAG_SENSITIVITY*100;
    
    MPU6050_DisableBypass();
}

// 发送传感器数据到电脑
void SendSensorData(void) {
    char buffer[256];
    
    // 读取所有传感器数据
    MPU6050_GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ, &Temperature);
    HMC5883L_GetData(&MagX, &MagY, &MagZ);
    
    // 格式化数据
    sprintf(buffer, 
        "ACC:%6d,%6d,%6d | "
        "GYRO:%6d,%6d,%6d | "
        "MAG:%6d,%6d,%6d | "
        "TEMP:%6d\r\n", 
        AccX, AccY, AccZ, 
        GyroX, GyroY, GyroZ,
        MagX, MagY, MagZ,
        Temperature);
    
    // 发送数据
    USART_SendString(buffer);
}

int main(void) {
    // 系统初始化
    MySystemClock_Config_HSI_Fallback();
    I2C1_Init();
    USART2_Init();
    
    // 启动信息
    USART_SendString("\r\n=== GY87 Sensor System ===\r\n");
    USART_SendString("Board: STM32F401 Nucleo\r\n");
    USART_SendString("Clock: 84MHz (HSI->PLL)\r\n");
    USART_SendString("Baudrate: 115200\r\n");
    USART_SendString("I2C: PB8(SCL/D15), PB9(SDA/D14)\r\n");
    USART_SendString("UART: PA2(TX/D1), PA3(RX/D0)\r\n");
    
    // 初始化MPU6050
    USART_SendString("Initializing MPU6050...\r\n");
    MPU6050_Init();
    Delay_ms(100);
    
    // 检查MPU6050 ID
    uint8_t mpu_id = MPU6050_GetID();
    char id_msg[50];
    sprintf(id_msg, "MPU6050 ID: 0x%02X", mpu_id);
    USART_SendString(id_msg);
    
    if(mpu_id == 0x68) {
        USART_SendString(" - OK\r\n");
    } else {
        USART_SendString(" - FAILED!\r\n");
        while(1) {
            USART_SendString("Error: Check GY87 connection!\r\n");
            Delay_ms(1000);
        }
    }
    
    // 初始化HMC5883L
    USART_SendString("Initializing HMC5883L...\r\n");
    HMC5883L_Init();
    USART_SendString("HMC5883L - OK\r\n");
    
    USART_SendString("=== Starting Data Acquisition ===\r\n\r\n");
    
    // 主循环
    uint32_t counter = 0;
    while(1) {
        SendSensorData();
        
        // 每10次发送一个状态信息
        counter++;
        if(counter % 10 == 0) {
            char status[50];
            sprintf(status, "[Status] Data packets sent: %lu\r\n", counter);
            USART_SendString(status);
        }
        
        Delay_ms(200);
    }
}
