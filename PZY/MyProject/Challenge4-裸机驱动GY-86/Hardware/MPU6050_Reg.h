#ifndef __MPU6050_REG_H
#define __MPU6050_REG_H

// 设备识别
#define MPU6050_WHO_AM_I         0x75   // 设备ID寄存器（默认值0x68，用于识别芯片）

// 电源管理与复位
#define MPU6050_PWR_MGMT_1       0x6B   // 电源管理1（复位、睡眠模式控制、时钟源选择）
#define MPU6050_PWR_MGMT_2       0x6C   // 电源管理2（传感器关闭控制）
#define MPU6050_RESET            0x68   // 复位寄存器（写入0x80触发复位）
#define MPU6050_SIGNAL_PATH_RESET 0x68  // 信号路径复位（写入0x07复位所有传感器信号路径）

// 陀螺仪配置
#define MPU6050_GYRO_CONFIG      0x1B   // 陀螺仪量程配置（±250/500/1000/2000°/s）
#define MPU6050_GYRO_XOUT_H      0x43   // 陀螺仪X轴数据高8位
#define MPU6050_GYRO_XOUT_L      0x44   // 陀螺仪X轴数据低8位
#define MPU6050_GYRO_YOUT_H      0x45   // 陀螺仪Y轴数据高8位
#define MPU6050_GYRO_YOUT_L      0x46   // 陀螺仪Y轴数据低8位
#define MPU6050_GYRO_ZOUT_H      0x47   // 陀螺仪Z轴数据高8位
#define MPU6050_GYRO_ZOUT_L      0x48   // 陀螺仪Z轴数据低8位

// 加速度计配置
#define MPU6050_ACCEL_CONFIG     0x1C   // 加速度计量程配置（±2/4/8/16g）
#define MPU6050_ACCEL_XOUT_H     0x3B   // 加速度计X轴数据高8位
#define MPU6050_ACCEL_XOUT_L     0x3C   // 加速度计X轴数据低8位
#define MPU6050_ACCEL_YOUT_H     0x3D   // 加速度计Y轴数据高8位
#define MPU6050_ACCEL_YOUT_L     0x3E   // 加速度计Y轴数据低8位
#define MPU6050_ACCEL_ZOUT_H     0x3F   // 加速度计Z轴数据高8位
#define MPU6050_ACCEL_ZOUT_L     0x40   // 加速度计Z轴数据低8位

// 温度传感器
#define MPU6050_TEMP_OUT_H       0x41   // 温度数据高8位
#define MPU6050_TEMP_OUT_L       0x42   // 温度数据低8位

// I2C总线控制（用于访问HMC5883L等外部设备）
#define MPU6050_USER_CTRL        0x6A   // 用户控制（I2C主模式使能、FIFO控制等）
#define MPU6050_I2C_BYPASS_EN    0x20   // 旁路模式使能位（Bit5 of 0x6A，使能后外部I2C设备可直接访问MCU）
#define MPU6050_I2C_MST_CTRL     0x24   // I2C主机控制（主机模式速率、超时配置）
#define MPU6050_I2C_SLV0_ADDR    0x25   // I2C从机0地址寄存器
#define MPU6050_I2C_SLV0_REG     0x26   // I2C从机0寄存器地址
#define MPU6050_I2C_SLV0_CTRL    0x27   // I2C从机0控制（读写使能、数据长度）

// 中断相关配置
#define MPU6050_INT_PIN_CFG      0x37   // 中断引脚配置（中断极性、电平模式、同步等）
#define MPU6050_INT_ENABLE       0x38   // 中断使能（数据就绪、FIFO溢出等中断源使能）
#define MPU6050_INT_STATUS       0x3A   // 中断状态（当前触发的中断标志）

// FIFO缓冲区配置
#define MPU6050_FIFO_EN          0x23   // FIFO使能（选择写入FIFO的数据类型）
#define MPU6050_FIFO_COUNT_H     0x72   // FIFO数据计数高8位
#define MPU6050_FIFO_COUNT_L     0x73   // FIFO数据计数低8位
#define MPU6050_FIFO_R_W         0x74   // FIFO读写寄存器（读取时输出FIFO数据，写入时清空FIFO）

// 其他配置
#define MPU6050_SMPLRT_DIV       0x19   // 采样率分频器（采样率 = 陀螺仪输出率 / (1+分频值)）
#define MPU6050_CONFIG           0x1A   // 低通滤波器配置（数据平滑，影响加速度计、陀螺仪带宽）

#endif