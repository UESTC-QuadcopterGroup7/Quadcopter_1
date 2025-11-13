#ifndef __HMC5883L_REG_H
#define __HMC5883L_REG_H

#include <stdint.h>

// 设备基本信息
#define HMC5883L_DEVICE_ADDR     0x1E    // I2C地址（7位地址，固定为0x1E）
#define HMC5883L_WHO_AM_I_VAL    0x48    // WHO_AM_I默认值（用于设备识别）

// 寄存器地址
#define HMC5883L_CONF_A          0x00    // 配置寄存器A（采样率、测量模式、平均采样数）
#define HMC5883L_CONF_B          0x01    // 配置寄存器B（磁场量程设置）
#define HMC5883L_MODE            0x02    // 模式寄存器（工作模式选择）
#define HMC5883L_DATA_X_MSB      0x03    // X轴磁场数据高8位
#define HMC5883L_DATA_X_LSB      0x04    // X轴磁场数据低8位
#define HMC5883L_DATA_Z_MSB      0x05    // Z轴磁场数据高8位
#define HMC5883L_DATA_Z_LSB      0x06    // Z轴磁场数据低8位
#define HMC5883L_DATA_Y_MSB      0x07    // Y轴磁场数据高8位（注意：Y轴地址在Z轴之后）
#define HMC5883L_DATA_Y_LSB      0x08    // Y轴磁场数据低8位
#define HMC5883L_STATUS          0x09    // 状态寄存器（数据就绪、锁定状态）
#define HMC5883L_ID_A            0x0A    // 识别寄存器A（默认0x48，即'H'）
#define HMC5883L_ID_B            0x0B    // 识别寄存器B（默认0x34，即'4'）
#define HMC5883L_ID_C            0x0C    // 识别寄存器C（默认0x33，即'3'）


// 配置寄存器A（0x00）位定义
// 平均采样数（MA1, MA0）：设置单次测量的平均采样数（减少噪声）
#define HMC5883L_CONF_A_MA_1     0x00    // 平均1次采样
#define HMC5883L_CONF_A_MA_2     0x20    // 平均2次采样
#define HMC5883L_CONF_A_MA_4     0x40    // 平均4次采样
#define HMC5883L_CONF_A_MA_8     0x60    // 平均8次采样（最高噪声抑制）

// 数据输出率（DO2, DO1, DO0）：连续模式下的采样频率（Hz）
#define HMC5883L_CONF_A_DO_0_75  0x00    // 0.75Hz
#define HMC5883L_CONF_A_DO_1_5   0x04    // 1.5Hz
#define HMC5883L_CONF_A_DO_3     0x08    // 3Hz
#define HMC5883L_CONF_A_DO_7_5   0x0C    // 7.5Hz
#define HMC5883L_CONF_A_DO_15    0x10    // 15Hz（常用）
#define HMC5883L_CONF_A_DO_30    0x14    // 30Hz
#define HMC5883L_CONF_A_DO_75    0x18    // 75Hz（最高速率）

// 测量模式（MS1, MS0）：设置磁场测量模式（用于自检或正常测量）
#define HMC5883L_CONF_A_MS_NORMAL 0x00   // 正常测量模式（默认）
#define HMC5883L_CONF_A_MS_POS    0x01   // 正偏置模式（自检：强制X、Y轴正向磁场）
#define HMC5883L_CONF_A_MS_NEG    0x02   // 负偏置模式（自检：强制X、Y轴负向磁场）


// 配置寄存器B（0x01）位定义：磁场量程设置（GN2, GN1, GN0）
#define HMC5883L_CONF_B_GN_0_88   0x00    // 量程±0.88Ga，分辨率0.73mGa/LSB
#define HMC5883L_CONF_B_GN_1_3    0x20    // 量程±1.3Ga，分辨率0.92mGa/LSB（常用）
#define HMC5883L_CONF_B_GN_1_9    0x40    // 量程±1.9Ga，分辨率1.22mGa/LSB
#define HMC5883L_CONF_B_GN_2_5    0x60    // 量程±2.5Ga，分辨率1.52mGa/LSB
#define HMC5883L_CONF_B_GN_4_0    0x80    // 量程±4.0Ga，分辨率2.27mGa/LSB
#define HMC5883L_CONF_B_GN_4_7    0xA0    // 量程±4.7Ga，分辨率2.56mGa/LSB
#define HMC5883L_CONF_B_GN_5_6    0xC0    // 量程±5.6Ga，分辨率3.03mGa/LSB
#define HMC5883L_CONF_B_GN_8_1    0xE0    // 量程±8.1Ga，分辨率4.35mGa/LSB


// 模式寄存器（0x02）位定义：工作模式（MD1, MD0）
#define HMC5883L_MODE_CONTINUOUS  0x00    // 连续测量模式（持续输出磁场数据）
#define HMC5883L_MODE_SINGLE      0x01    // 单次测量模式（触发一次测量后进入休眠）
#define HMC5883L_MODE_IDLE        0x02    // 空闲模式（低功耗，不测量）
#define HMC5883L_MODE_IDLE2       0x03    // 空闲模式2（同Idle，兼容旧版本）


// 状态寄存器（0x09）位定义
#define HMC5883L_STATUS_RDY       0x01    // 数据就绪标志（1：新数据可用）
#define HMC5883L_STATUS_LOCK      0x02    // 数据锁定标志（1：数据正在更新，不可读）


#endif