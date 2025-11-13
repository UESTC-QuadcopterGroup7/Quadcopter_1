#ifndef __MS5611_REG_H
#define __MS5611_REG_H

#include <stdint.h>

// 设备基本信息（保留原定义，补充I2C读写地址）
#define MS5611_DEVICE_ADDR       0x77    // I2C 7位基地址（ADR接地为0x77，接VCC为0x76）
#define MS5611_PROM_SIZE         8       // PROM校准参数总数（8个16位寄存器）
#define MS5611_ADC_DATA_BITS     24      // ADC转换结果位数（无符号24位）

// I2C读写地址（基于基地址计算，避免重复定义）
#define MS5611_I2C_W_ADDR        (MS5611_DEVICE_ADDR << 1)    // 写地址（左移1位+0）
#define MS5611_I2C_R_ADDR        (MS5611_DEVICE_ADDR << 1 | 1)// 读地址（左移1位+1）

// 核心命令（修正宏名一致性，避免C文件调用错误）
#define MS5611_RESET             0x1E    // 复位命令（需等待≥3ms）
#define MS5611_ADC_READ          0x00    // 读取ADC结果（3字节：MSB→LSB→XLSB）
#define MS5611_PROM_READ_BASE    0xA0    // PROM读取基地址
#define MS5611_PROM_READ         MS5611_PROM_READ_BASE  // 【补充】PROM读取命令（与基地址一致）

// PROM校准参数地址（原定义正确，保留）
#define MS5611_PROM_C0           0xA0    // 厂商保留（无实际意义）
#define MS5611_PROM_C1           0xA2    // 气压灵敏度（SENS_T1）
#define MS5611_PROM_C2           0xA4    // 气压偏移（OFF_T1）
#define MS5611_PROM_C3           0xA6    // 灵敏度温度系数（TCS）
#define MS5611_PROM_C4           0xA8    // 偏移温度系数（TCO）
#define MS5611_PROM_C5           0xAA    // 参考温度（T_REF）
#define MS5611_PROM_C6           0xAC    // 温度灵敏度系数（TEMPSENS）
#define MS5611_PROM_CRC          0xAE    // CRC校验值

// 温度转换命令（OSR=过采样率，原定义正确，补充注释）
#define MS5611_CONV_D2_OSR_256   0x50    // 转换时间~0.54ms
#define MS5611_CONV_D2_OSR_512   0x52    // 转换时间~1.06ms
#define MS5611_CONV_D2_OSR_1024  0x54    // 转换时间~2.08ms
#define MS5611_CONV_D2_OSR_2048  0x56    // 转换时间~4.13ms
#define MS5611_CONV_D2_OSR_4096  0x58    // 转换时间~8.22ms（最高精度，推荐用）

// 气压转换命令（OSR含义同温度，原定义正确）
#define MS5611_CONV_D1_OSR_256   0x40    // 转换时间~0.54ms
#define MS5611_CONV_D1_OSR_512   0x42    // 转换时间~1.06ms
#define MS5611_CONV_D1_OSR_1024  0x44    // 转换时间~2.08ms
#define MS5611_CONV_D1_OSR_2048  0x46    // 转换时间~4.13ms
#define MS5611_CONV_D1_OSR_4096  0x48    // 转换时间~8.22ms（最高精度，推荐用）

// 辅助宏定义（原定义正确，保留）
#define MS5611_PROM_ADDR(n)      (MS5611_PROM_READ_BASE + 2*(n))  // 第n个PROM地址（n=0~7）

#endif