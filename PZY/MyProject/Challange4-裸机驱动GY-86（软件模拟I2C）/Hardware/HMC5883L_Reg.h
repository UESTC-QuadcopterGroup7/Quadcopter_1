#ifndef __HMC5883L_REG_H
#define __HMC5883L_REG_H

// -------------------------- HMC5883L 寄存器地址定义--------------------------
#define HMC5883L_CONFIG_A     0x00  // 配置寄存器A（采样/输出率/测量模式）
#define HMC5883L_CONFIG_B     0x01  // 配置寄存器B（增益选择）
#define HMC5883L_MODE         0x02  // 模式寄存器（工作模式）
#define HMC5883L_DATA_X_MSB   0x03  // X轴数据高位
#define HMC5883L_DATA_X_LSB   0x04  // X轴数据低位
#define HMC5883L_DATA_Z_MSB   0x05  // Z轴数据高位（默认顺序：X→Z→Y）
#define HMC5883L_DATA_Z_LSB   0x06  // Z轴数据低位
#define HMC5883L_DATA_Y_MSB   0x07  // Y轴数据高位
#define HMC5883L_DATA_Y_LSB   0x08  // Y轴数据低位
#define HMC5883L_STATUS       0x09  // 状态寄存器（数据就绪标志）
#define HMC5883L_ID_A         0x0A  // 身份寄存器A（默认0x48）
#define HMC5883L_ID_B         0x0B  // 身份寄存器B（默认0x34）
#define HMC5883L_ID_C         0x0C  // 身份寄存器C（默认0x33）

// -------------------------- 寄存器配置参数（与初始化函数匹配）--------------------------
// 配置寄存器A：采样平均次数（高2位）
#define HMC5883L_SAMPLES_8    0x60  // 8次采样平均（提高精度）
// 配置寄存器A：数据输出率（中2位）
#define HMC5883L_DATA_RATE_15 0x10  // 15Hz输出率（平衡响应速度与稳定性）
// 配置寄存器A：测量模式（低2位）
#define HMC5883L_MEAS_NORMAL  0x00  // 正常测量模式

// 配置寄存器B：增益选择（高3位）
#define HMC5883L_GAIN_130     0x20  // ±1.3Ga测量范围（分辨率0.92mG/LSB，常用高精度模式）

// 模式寄存器：工作模式
#define HMC5883L_MODE_SINGLE   0x01  // 单次测量模式（触发复位）
#define HMC5883L_MODE_CONTINUOUS 0x00  // 连续测量模式

#endif