#include "stm32f4xx.h"
#include <stdio.h>

#include "Delay.h"
#include "MyBMP.h"
#include "MyHMC5883L.h"
#include "MyI2C.h"
#include "MyMPU6050.h"
#include "MyUSART.h"
#include "Mysystem_clock.h"

#define SENSOR_DELAY_MS   200U

static const USART_Port kDebugPort = USART2_PORT;
static const BMP180_OSS_t kBmpOversampling = BMP180_OSS_STANDARD;

static void System_Init(void);
static void FatalError(const char *message);
static void LogStartupInfo(void);
static void LogSensorData(void);

int main(void)
{
    System_Init();
    LogStartupInfo();

    if (MPU6050_Init() != 0)
    {
        FatalError("MPU6050 init failed!\r\n");
    }

    HMC5883L_Init();

    if (BMP180_Init() != BMP180_STATUS_OK)
    {
        USART_SendString(kDebugPort, "[WARN] BMP180 init failed, altitude data disabled.\r\n");
    }

    while (1)
    {
        LogSensorData();
        Delay_ms(SENSOR_DELAY_MS);
    }
}

static void System_Init(void)
{
    SystemClock_Config_HSI();
    SystemCoreClockUpdate();
    Delay_Init();
    MyI2C_Init();
    MyUSART_Init(kDebugPort, 115200);
}

static void FatalError(const char *message)
{
    USART_SendString(kDebugPort, message);
    while (1)
    {
        USART_SendString(kDebugPort, "System halted.\r\n");
        Delay_ms(1000);
    }
}

static void LogStartupInfo(void)
{
    USART_SendString(kDebugPort, "\r\n=== GY-87 Sensor Monitor ===\r\n");
    USART_Printf(kDebugPort, "System clock: %lu Hz\r\n", SystemCoreClock);
    USART_SendString(kDebugPort, "UART: USART2 @115200bps\r\n");
}

static void LogSensorData(void)
{
    int16_t acc_x = 0, acc_y = 0, acc_z = 0;
    int16_t gyro_x = 0, gyro_y = 0, gyro_z = 0;
    int16_t mag_x = 0, mag_y = 0, mag_z = 0;
    BMP180_Data_t bmp_data = {0};
    char buffer[192];

    MPU6050_GetData(&acc_x, &acc_y, &acc_z, &gyro_x, &gyro_y, &gyro_z);
    HMC5883L_GetData(&mag_x, &mag_y, &mag_z);

    if (BMP180_GetData(&bmp_data, kBmpOversampling) != BMP180_STATUS_OK)
    {
        bmp_data.temperature = 0.0f;
        bmp_data.pressure = 0.0f;
        bmp_data.altitude = 0.0f;
    }

    snprintf(buffer, sizeof(buffer),
             "ACC:%6d,%6d,%6d | GYRO:%6d,%6d,%6d | MAG:%6d,%6d,%6d | TEMP:%6.2fC | PRES:%9.2fPa | ALT:%7.2fm\r\n",
             acc_x, acc_y, acc_z,
             gyro_x, gyro_y, gyro_z,
             mag_x, mag_y, mag_z,
             bmp_data.temperature,
             bmp_data.pressure,
             bmp_data.altitude);

    USART_SendString(kDebugPort, buffer);
}
