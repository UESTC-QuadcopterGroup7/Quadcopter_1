#ifndef __MS5611_REG_H
#define __MS5611_REG_H

// MS5611命令与寄存器地址
#define MS5611_RESET         0x1E    // 复位命令
#define MS5611_ADC_READ      0x00    // ADC读取地址

// PROM校准系数地址（16位，每个地址间隔2字节）
#define MS5611_PROM_C1       0xA2    // 压力灵敏度系数
#define MS5611_PROM_C2       0xA4    // 压力偏移系数
#define MS5611_PROM_C3       0xA6    // 温度灵敏度系数
#define MS5611_PROM_C4       0xA8    // 温度偏移系数
#define MS5611_PROM_C5       0xAA    // 参考温度系数
#define MS5611_PROM_C6       0xAC    // 温度系数

// 转换命令（D1=气压，D2=温度，后缀为采样精度）
#define MS5611_CONV_D1_256   0x40    // 气压转换，256采样
#define MS5611_CONV_D1_512   0x42    // 气压转换，512采样
#define MS5611_CONV_D1_1024  0x44    // 气压转换，1024采样
#define MS5611_CONV_D1_2048  0x46    // 气压转换，2048采样
#define MS5611_CONV_D1_4096  0x48    // 气压转换，4096采样（最高精度）

#define MS5611_CONV_D2_256   0x50    // 温度转换，256采样
#define MS5611_CONV_D2_512   0x52    // 温度转换，512采样
#define MS5611_CONV_D2_1024  0x54    // 温度转换，1024采样
#define MS5611_CONV_D2_2048  0x56    // 温度转换，2048采样
#define MS5611_CONV_D2_4096  0x58    // 温度转换，4096采样（最高精度）

#endif