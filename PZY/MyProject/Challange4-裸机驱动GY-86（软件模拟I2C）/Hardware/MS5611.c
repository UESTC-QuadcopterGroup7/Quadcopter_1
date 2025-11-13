#include "stm32f4xx.h"
#include "MyI2C.h"
#include "MS5611_Reg.h"
#include "Delay.h"  // 使用用户提供的延时头文件

#define MS5611_ADDRESS 0xEE  // I2C地址（根据硬件接线，可选0xEC或0xEE）

// 静态变量存储校准系数（内部使用）
static uint16_t C1, C2, C3, C4, C5, C6;

/* 向MS5611发送命令 */
static void MS5611_SendCmd(uint8_t Cmd) {
    MyI2C_Start();
    MyI2C_SendByte(MS5611_ADDRESS);  // 发送写地址
    MyI2C_ReceiveAck();
    MyI2C_SendByte(Cmd);             // 发送命令
    MyI2C_ReceiveAck();
    MyI2C_Stop();
}

/* 从MS5611读取PROM校准系数（16位） */
uint16_t MS5611_ReadPROM(uint8_t PromAddr) {
    uint8_t DataH, DataL;
    
    MyI2C_Start();
    MyI2C_SendByte(MS5611_ADDRESS);      // 写地址
    MyI2C_ReceiveAck();
    MyI2C_SendByte(PromAddr);            // 发送PROM地址
    MyI2C_ReceiveAck();
    
    MyI2C_Start();
    MyI2C_SendByte(MS5611_ADDRESS | 0x01);  // 读地址
    MyI2C_ReceiveAck();
    DataH = MyI2C_ReceiveByte();            // 读取高8位
    MyI2C_SendAck(0);                       // 继续接收
    DataL = MyI2C_ReceiveByte();            // 读取低8位
    MyI2C_SendAck(1);                       // 停止接收
    MyI2C_Stop();
    
    return (DataH << 8) | DataL;  // 组合为16位系数
}

/* 从MS5611读取ADC数据（24位） */
static int32_t MS5611_ReadADC(void) {
    uint8_t DataH, DataM, DataL;
    
    MyI2C_Start();
    MyI2C_SendByte(MS5611_ADDRESS);      // 写地址
    MyI2C_ReceiveAck();
    MyI2C_SendByte(MS5611_ADC_READ);     // 读取ADC命令
    MyI2C_ReceiveAck();
    
    MyI2C_Start();
    MyI2C_SendByte(MS5611_ADDRESS | 0x01);  // 读地址
    MyI2C_ReceiveAck();
    DataH = MyI2C_ReceiveByte();            // 高8位
    MyI2C_SendAck(0);
    DataM = MyI2C_ReceiveByte();            // 中8位
    MyI2C_SendAck(0);
    DataL = MyI2C_ReceiveByte();            // 低8位
    MyI2C_SendAck(1);
    MyI2C_Stop();
    
    return (DataH << 16) | (DataM << 8) | DataL;  // 组合为24位数据
}

/* 初始化MS5611 */
void MS5611_Init(void) {
    MyI2C_Init();               // 初始化I2C总线
    
    // 复位传感器（至少需要3ms复位时间）
    MS5611_SendCmd(MS5611_RESET);
    Delay_ms(10);               // 替换为用户的毫秒延时函数
    
    // 读取PROM校准系数（用于后续数据计算）
    C1 = MS5611_ReadPROM(MS5611_PROM_C1);
    C2 = MS5611_ReadPROM(MS5611_PROM_C2);
    C3 = MS5611_ReadPROM(MS5611_PROM_C3);
    C4 = MS5611_ReadPROM(MS5611_PROM_C4);
    C5 = MS5611_ReadPROM(MS5611_PROM_C5);
    C6 = MS5611_ReadPROM(MS5611_PROM_C6);
}

/* 获取气压和温度数据（带温度补偿） */
void MS5611_GetData(int32_t *Pressure, int32_t *Temperature) {
    int32_t D1, D2;             // 原始气压和温度数据
    int64_t dT, TEMP;           // 温度差和实际温度
    int64_t OFF, SENS, P;       // 偏移量、灵敏度、气压
    int64_t T2, OFF2, SENS2;    // 低温补偿参数

    // 1. 读取温度（最高精度：4096采样，转换时间最长8.22ms）
    MS5611_SendCmd(MS5611_CONV_D2_4096);
    Delay_ms(10);               // 等待转换完成，替换为用户的毫秒延时
    D2 = MS5611_ReadADC();

    // 2. 读取气压（最高精度：4096采样，转换时间最长8.22ms）
    MS5611_SendCmd(MS5611_CONV_D1_4096);
    Delay_ms(10);               // 等待转换完成，替换为用户的毫秒延时
    D1 = MS5611_ReadADC();

    // 3. 温度计算（参考MS5611数据手册公式）
    dT = D2 - (int64_t)C5 * 256;                  // 温度差
    TEMP = 2000 + (dT * (int64_t)C6) / 8388608;   // 实际温度（0.01℃）

    // 4. 计算偏移量和灵敏度（基础值）
    OFF = (int64_t)C2 * 131072 + (int64_t)C4 * dT / 64;  // 偏移量
    SENS = (int64_t)C1 * 65536 + (int64_t)C3 * dT / 128;  // 灵敏度

    // 5. 低温补偿（当温度低于20℃时）
    if (TEMP < 2000) {
        T2 = (dT * dT) / 2147483648;                   // 温度补偿项
        OFF2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 2; // 偏移补偿
        SENS2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 4; // 灵敏度补偿

        // 超低温额外补偿（低于-15℃）
        if (TEMP < -1500) {
            OFF2 += 7 * ((TEMP + 1500) * (TEMP + 1500));
            SENS2 += 11 * ((TEMP + 1500) * (TEMP + 1500)) / 2;
        }

        TEMP -= T2;
        OFF -= OFF2;
        SENS -= SENS2;
    }

    // 6. 气压计算（0.01hPa）
    P = (D1 * SENS / 2097152 - OFF) / 32768;

    // 输出结果
    *Temperature = (int32_t)TEMP;
    *Pressure = (int32_t)P;
}