/**
  ******************************************************************************
  * @file    MyBMP.h
  * @brief   BMP180 barometer driver based on software I2C
  ******************************************************************************
  */

#ifndef __MYBMP_H
#define __MYBMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "MyI2C.h"

#define BMP180_WRITE_ADDR         0xEE
#define BMP180_READ_ADDR          0xEF

#define BMP180_REG_AC1_MSB        0xAA
#define BMP180_REG_CTRL_MEAS      0xF4
#define BMP180_REG_OUT_MSB        0xF6
#define BMP180_REG_OUT_LSB        0xF7
#define BMP180_REG_OUT_XLSB       0xF8

#define BMP180_CMD_TEMP           0x2E
#define BMP180_CMD_PRESSURE_0     0x34
#define BMP180_CMD_PRESSURE_1     0x74
#define BMP180_CMD_PRESSURE_2     0xB4
#define BMP180_CMD_PRESSURE_3     0xF4

typedef enum
{
    BMP180_OSS_ULTRA_LOW_POWER = 0,
    BMP180_OSS_STANDARD        = 1,
    BMP180_OSS_HIGH_RES        = 2,
    BMP180_OSS_ULTRA_HIGH_RES  = 3
} BMP180_OSS_t;

typedef enum
{
    BMP180_STATUS_OK = 0,
    BMP180_STATUS_ERROR = -1
} BMP180_Status;

typedef struct
{
    int16_t AC1;
    int16_t AC2;
    int16_t AC3;
    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;
    int16_t B1;
    int16_t B2;
    int16_t MB;
    int16_t MC;
    int16_t MD;
} BMP180_CalibData_t;

typedef struct
{
    float temperature;
    float pressure;
    float altitude;
} BMP180_Data_t;

BMP180_Status BMP180_Init(void);
BMP180_Status BMP180_GetData(BMP180_Data_t *data, BMP180_OSS_t oss);
float BMP180_CalculateAltitude(float pressure, float sea_level_pressure);

#ifdef __cplusplus
}
#endif

#endif /* __MYBMP_H */
