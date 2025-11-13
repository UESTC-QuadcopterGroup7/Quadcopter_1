#include "stm32f4xx.h"
#include "MS5611.h"
#include "MS5611_Reg.h"
#include "Delay.h"  // 【新增】包含外部延时函数头文件

// 全局变量存储PROM校准系数（仅内部使用，static修饰）
static uint16_t MS5611_C1, MS5611_C2, MS5611_C3;
static uint16_t MS5611_C4, MS5611_C5, MS5611_C6;

/**
  * @brief  I2C事件等待函数（保留原逻辑，无修改）
  * @param  I2Cx：I2C外设（固定I2C2，与HMC5883L兼容）
  * @param  I2C_EVENT：等待的I2C事件
  * @retval 无
  */
void MS5611_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT){
    uint32_t Timeout = 10000;
    while(I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS){
        Timeout--;
        if(Timeout == 0) return;  // 超时退出，防止死循环
    }
}

/**
  * @brief  单字节写寄存器（使用Reg.h统一的I2C地址，删除冗余定义）
  * @param  RegAddress：寄存器/命令地址（MS5611命令即“命令地址”）
  * @param  Data：要写入的数据（无数据时传0）
  * @retval 无
  */
void MS5611_WriteReg(uint8_t RegAddress, uint8_t Data){
    // 1. 发送I2C起始信号
    I2C_GenerateSTART(I2C2, ENABLE);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

    // 2. 发送从机写地址（使用Reg.h定义的MS5611_I2C_W_ADDR，避免重复）
    I2C_Send7bitAddress(I2C2, MS5611_I2C_W_ADDR, I2C_Direction_Transmitter);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    // 3. 发送命令/寄存器地址
    I2C_SendData(I2C2, RegAddress);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    // 4. 发送数据（无数据时Data=0，不影响）
    I2C_SendData(I2C2, Data);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    // 5. 发送I2C停止信号
    I2C_GenerateSTOP(I2C2, ENABLE);
}

/**
  * @brief  单字节读寄存器（使用Reg.h统一地址，保留原逻辑）
  * @param  RegAddress：要读取的寄存器/命令地址
  * @retval 读取到的单字节数据
  */
uint8_t MS5611_ReadReg(uint8_t RegAddress){
    uint8_t Data;

    // 第一阶段：发送要读取的地址（写操作）
    I2C_GenerateSTART(I2C2, ENABLE);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2, MS5611_I2C_W_ADDR, I2C_Direction_Transmitter);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    I2C_SendData(I2C2, RegAddress);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    // 第二阶段：切换为读操作（使用Reg.h定义的MS5611_I2C_R_ADDR）
    I2C_GenerateSTART(I2C2, ENABLE);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2, MS5611_I2C_R_ADDR, I2C_Direction_Receiver);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);

    // 单字节读取：关应答+提前发停止位
    I2C_AcknowledgeConfig(I2C2, DISABLE);
    I2C_GenerateSTOP(I2C2, ENABLE);

    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);
    Data = I2C_ReceiveData(I2C2);

    // 恢复应答使能
    I2C_AcknowledgeConfig(I2C2, ENABLE);
    return Data;
}

/**
  * @brief  多字节读寄存器（读取PROM/ADC数据，无修改）
  * @param  RegAddress：命令地址（PROM读/ADC读）
  * @param  Len：读取字节数（2=PROM，3=ADC）
  * @retval 拼接后的多字节数据（高字节在前）
  */
uint32_t MS5611_ReadMultiReg(uint8_t RegAddress, uint8_t Len){
    uint32_t Data = 0;

    // 第一阶段：发送命令地址
    I2C_GenerateSTART(I2C2, ENABLE);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2, MS5611_I2C_W_ADDR, I2C_Direction_Transmitter);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    I2C_SendData(I2C2, RegAddress);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    // 第二阶段：多字节读取
    I2C_GenerateSTART(I2C2, ENABLE);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2, MS5611_I2C_R_ADDR, I2C_Direction_Receiver);
    MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);

    for(uint8_t i = 0; i < Len; i++){
        if(i == Len - 1){  // 最后1字节：关应答+发停止位
            I2C_AcknowledgeConfig(I2C2, DISABLE);
            I2C_GenerateSTOP(I2C2, ENABLE);
        }

        MS5611_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);
        Data = (Data << 8) | I2C_ReceiveData(I2C2);  // 高字节在前拼接
    }

    // 恢复应答使能
    I2C_AcknowledgeConfig(I2C2, ENABLE);
    return Data;
}

/**
  * @brief  读取PROM校准系数（无修改，Reg.h已补充MS5611_PROM_READ）
  * @param  PromIdx：PROM索引（0~7，1~6为有效系数）
  * @retval 16位校准系数（超出索引返回0）
  */
uint16_t MS5611_ReadProm(uint8_t PromIdx){
    if(PromIdx > 7) return 0;  // 仅0~7有效
    uint8_t PromCmd = MS5611_PROM_READ + (PromIdx << 1);  // 正确计算PROM命令地址
    return (uint16_t)MS5611_ReadMultiReg(PromCmd, 2);  // PROM为16位（2字节）
}

/**
  * @brief  读取ADC转换结果（无修改）
  * @retval 24位ADC原始数据（温度/压力）
  */
uint32_t MS5611_ReadAdc(void){
    return MS5611_ReadMultiReg(MS5611_ADC_READ, 3);  // ADC为24位（3字节）
}

/**
  * @brief  MS5611初始化（核心修改：用Delay_ms替代for循环，时序更精准）
  * @retval 无
  */
void MS5611_Init(void)
{
    // 1. 使能GPIO和I2C时钟（保留原逻辑，与HMC5883L兼容：PB9=SCL，PB10=SDA）
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    
    // 2. 配置I2C引脚（AF+OD+上拉，与HMC5883L完全一致）
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 3. 配置GPIO复用映射（PB9=I2C2_SCL，PB10=I2C2_SDA）
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);

    // 4. 初始化I2C2（100kHz，7位地址，使能ACK，与HMC5883L兼容）
    I2C_InitTypeDef I2C_InitStruct;
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_ClockSpeed = 100000;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;  // 主机地址（无意义，填0即可）
    I2C_Init(I2C2, &I2C_InitStruct);

    // 5. 使能I2C2
    I2C_Cmd(I2C2, ENABLE);

    // 6. 传感器复位（【修改】用Delay_ms(5)替代for循环，手册要求≥3ms）
    MS5611_WriteReg(MS5611_RESET, 0x00);  // 复位命令，无数据传0
    Delay_ms(5);  // 等待复位完成（5ms足够，避免复位不彻底）

    // 7. 读取PROM校准系数（1~6为温度/压力补偿系数）
    MS5611_C1 = MS5611_ReadProm(1);  // 气压灵敏度（SENS_T1）
    MS5611_C2 = MS5611_ReadProm(2);  // 气压偏移（OFF_T1）
    MS5611_C3 = MS5611_ReadProm(3);  // 灵敏度温度系数（TCS）
    MS5611_C4 = MS5611_ReadProm(4);  // 偏移温度系数（TCO）
    MS5611_C5 = MS5611_ReadProm(5);  // 参考温度（T_REF）
    MS5611_C6 = MS5611_ReadProm(6);  // 温度灵敏度系数（TEMPSENS）
}

/**
  * @brief  获取温度和压力数据（核心修改：用Delay_ms替代for循环，修正命令宏名）
  * @param  Temprature：温度结果指针（单位：℃）
  * @param  press：压力结果指针（单位：hPa）
  * @retval 无
  */
void MS5611_GetData(float *Temprature, float *Press)
{
    uint32_t D1, D2;  // D1=气压原始值，D2=温度原始值
    int32_t dT, TEMP;  // dT=温度偏差，TEMP=补偿后温度（单位：0.01℃）
    int64_t OFF, SENS;  // OFF=压力偏移补偿，SENS=压力灵敏度补偿

    // 1. 启动温度转换（【修正】用Reg.h的宏名MS5611_CONV_D2_OSR_4096，避免命名错误）
    MS5611_WriteReg(MS5611_CONV_D2_OSR_4096, 0x00);
    Delay_ms(10);  // 等待转换完成（最高精度需8.22ms，10ms足够）
    D2 = MS5611_ReadAdc();  // 读取温度原始数据

    // 2. 启动压力转换（【修正】宏名MS5611_CONV_D1_OSR_4096）
    MS5611_WriteReg(MS5611_CONV_D1_OSR_4096, 0x00);
    Delay_ms(10);  // 等待转换完成
    D1 = MS5611_ReadAdc();  // 读取压力原始数据

    // 3. 温度补偿计算（手册标准公式，无修改）
    dT = D2 - (uint32_t)MS5611_C5 * 256;  // 温度偏差 = 实际温度 - 参考温度
    TEMP = 2000 + (int32_t)(dT * MS5611_C6) / 8388608;  // 补偿后温度（0.01℃）

    // 4. 压力补偿计算（基础公式）
    OFF = (int64_t)MS5611_C2 * 65536 + (int64_t)MS5611_C4 * dT / 128;  // 偏移补偿
    SENS = (int64_t)MS5611_C1 * 32768 + (int64_t)MS5611_C3 * dT / 256;  // 灵敏度补偿

    // 5. 低温补偿（TEMP<20℃时启用，提升精度）
    if(TEMP < 2000){
        int32_t T2 = (dT * dT) >> 31;  // 温度二次补偿项
        int64_t OFF2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) >> 1;
        int64_t SENS2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) >> 2;

        // 超低温补偿（TEMP<-15℃时追加）
        if(TEMP < -1500){
            OFF2 += 7 * ((TEMP + 1500) * (TEMP + 1500));
            SENS2 += 11 * ((TEMP + 1500) * (TEMP + 1500)) >> 1;
        }

        TEMP -= T2;
        OFF -= OFF2;
        SENS -= SENS2;
    }

    // 6. 转换为实际单位（0.01℃→℃，原始值→hPa）
    *Temprature = (float)TEMP / 100;    // 温度：保留2位小数
    *Press = (float)((D1 * SENS / 2097152 - OFF) / 32768) / 100;  // 压力：单位hPa
}

/**
  * @brief  获取设备ID（无修改，基于PROM CRC标识）
  * @retval 设备ID（PROM CRC低8位，用于确认设备存在）
  */
uint8_t MS5611_GetID(void){
    uint16_t PromCRC = MS5611_ReadProm(7);  // 读取PROM CRC校验值
    return (uint8_t)(PromCRC & 0xFF);       // 返回低8位作为ID
}