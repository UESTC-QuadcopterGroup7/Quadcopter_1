#ifndef __MS5611_H
#define __MS5611_H

#include "stm32f4xx.h"

// 设备I2C地址宏定义（与.c文件一致，供外部调用）
#define MS5611_BASE_ADDR 0x77    // 7位基础地址（ADR引脚接地时）
#define MS5611_W_ADDR    0xEE    // 写地址（基础地址左移1位 + 0）
#define MS5611_R_ADDR    0xEF    // 读地址（基础地址左移1位 + 1）


/**
  * @brief  等待I2C事件完成
  * @param  I2Cx：I2C外设（固定为I2C2）
  * @param  I2C_EVENT：需等待的I2C事件（参考stm32f4xx_i2c.h定义）
  * @retval 无
  */
void MS5611_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);

/**
  * @brief  向MS5611指定寄存器/命令地址写入单字节数据
  * @param  RegAddress：寄存器/命令地址（参考MS5611_Reg.h）
  * @param  Data：要写入的单字节数据（无数据时传0）
  * @retval 无
  */
void MS5611_WriteReg(uint8_t RegAddress, uint8_t Data);

/**
  * @brief  从MS5611指定寄存器/命令地址读取单字节数据
  * @param  RegAddress：寄存器/命令地址（参考MS5611_Reg.h）
  * @retval 读取到的单字节数据
  */
uint8_t MS5611_ReadReg(uint8_t RegAddress);

/**
  * @brief  从MS5611指定寄存器/命令地址读取多字节数据
  * @param  RegAddress：寄存器/命令地址（参考MS5611_Reg.h）
  * @param  Len：需读取的字节数（2=PROM，3=ADC）
  * @retval 拼接后的多字节数据（高字节在前）
  */
uint32_t MS5611_ReadMultiReg(uint8_t RegAddress, uint8_t Len);

/**
  * @brief  读取MS5611的PROM校准系数
  * @param  PromIdx：PROM索引（0~7，1~6为有效校准系数）
  * @retval 对应索引的16位校准系数
  */
uint16_t MS5611_ReadProm(uint8_t PromIdx);

/**
  * @brief  读取MS5611的ADC转换结果（温度/压力原始数据）
  * @retval 24位ADC转换结果
  */
uint32_t MS5611_ReadAdc(void);

/**
  * @brief  初始化MS5611传感器（含GPIO、I2C配置及传感器复位）
  * @retval 无
  */
void MS5611_Init(void);

/**
  * @brief  获取MS5611测量的温度和压力数据
  * @param  temp：温度结果指针（单位：℃，精度0.01℃）
  * @param  press：压力结果指针（单位：hPa，精度0.01hPa）
  * @retval 无
  */
void MS5611_GetData(float *temp, float *press);

/**
  * @brief  获取MS5611的设备ID（基于PROM的CRC值）
  * @retval 设备ID（单字节，PROM CRC的低8位）
  */
uint8_t MS5611_GetID(void);

#endif /* __MS5611_H */