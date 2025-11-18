/**
  ******************************************************************************
  * @file    MyBMP.h
  * @brief   Driver for Bosch BMP280 pressure sensor based on software I2C.
  ******************************************************************************
  */

#ifndef __MYBMP_H
#define __MYBMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "Delay.h"
#include "MyI2C.h"

#define BMP280_SEA_LEVEL_PRESSURE_PA   (101325.0f)
#define BMP280_DEFAULT_OVERSAMPLING    (0x27U)  /* osrs_t=x1, osrs_p=x1, normal mode */
#define BMP280_DEFAULT_CONFIG          (0x14U)  /* standby 62.5ms, filter x2 */
#define BMP280_REG_ID                  (0xD0U)
#define BMP280_CHIP_ID                 (0x58U)
#define BMP280_REG_RESET               (0xE0U)
#define BMP280_SOFT_RESET_VALUE        (0xB6U)
#define BMP280_REG_CTRL_MEAS           (0xF4U)
#define BMP280_REG_CONFIG              (0xF5U)
#define BMP280_REG_PRESS_MSB           (0xF7U)
#define BMP280_REG_TEMP_MSB            (0xFAU)
#define BMP280_REG_CALIB_START         (0x88U)
#define BMP280_CALIB_DATA_LEN          (24U)
#define BMP280_I2C_ADDRESS             (0x76U << 1)   /* 8-bit address */

/**
 * @brief Calibration parameters as defined in the BMP280 datasheet.
 */
typedef struct
{
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} BMP280_CalibData;

/**
 * @brief BMP280 driver context. Stores calibration data and state.
 */
typedef struct
{
    BMP280_CalibData calib;
    int32_t          t_fine;
    uint8_t          initialized;
} BMP280_HandleTypeDef;

void BMP280_HandleInit(BMP280_HandleTypeDef *handle);
int  BMP280_Init(BMP280_HandleTypeDef *handle);
int  BMP280_ReadChipID(uint8_t *chip_id);
int  BMP280_ReadTemperaturePressure(BMP280_HandleTypeDef *handle,
                                    float *temperature_c,
                                    float *pressure_pa);
float BMP280_CalcAltitude(float pressure_pa, float sea_level_pa);

#ifdef __cplusplus
}
#endif

#endif /* __MYBMP_H */
