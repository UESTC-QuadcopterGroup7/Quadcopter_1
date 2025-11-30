#include "stm32f4xx.h"
#include <stdio.h>

#include "Delay.h"
#include "MyBMP.h"
#include "MyHMC5883L.h"
#include "MyI2C.h"
#include "MyMPU6050.h"
#include "MyUSART.h"
#include "Mysystem_clock.h"
#include "MyPPM_temp.h"      // ★ 加入 PPM

#define SENSOR_DELAY_MS   200U

static const USART_Port kDebugPort = USART2_PORT;
static const BMP180_OSS_t kBmpOversampling = BMP180_OSS_STANDARD;

static void System_Init(void);
static void FatalError(const char *message);
static void LogStartupInfo(void);
static void LogSensorData(void);
static void LogPPMData(void);   // ★ 新增

int main(void)
{
    System_Init();
    LogStartupInfo();

    //if (MPU6050_Init() != 0)
        //FatalError("MPU6050 init failed!\r\n");

    //HMC5883L_Init();

    //if (BMP180_Init() != BMP180_STATUS_OK)
        //USART_SendString(kDebugPort, "[WARN] BMP180 init failed.\r\n");

    while (1)
    {
        //LogSensorData();
        LogPPMData();         // ★ 新增：显示 PPM
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

    PPM_Init();               // ★ 初始化 PPM（PA0 + TIM2）
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
    USART_SendString(kDebugPort, "\r\n=== GY-87 Sensor Monitor + PPM ===\r\n");
    USART_Printf(kDebugPort, "System clock: %lu Hz\r\n", SystemCoreClock);
    USART_SendString(kDebugPort, "UART: USART2 @115200bps\r\n");
    USART_SendString(kDebugPort, "PPM Input: TIM2_CH1 @PA0\r\n");   // ★ 显示信息
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
        bmp_data.temperature = bmp_data.pressure = bmp_data.altitude = 0;

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

// ★★★ PPM 输出函数（核心） ★★★
static void LogPPMData(void)
{
    if (!PPM_FrameReady())
        return;  // 没有新一帧就不输出，避免刷屏太快

    char buffer[160];
    snprintf(buffer, sizeof(buffer),
             "PPM: CH1=%4d CH2=%4d CH3=%4d CH4=%4d "
             "CH5=%4d CH6=%4d CH7=%4d CH8=%4d\r\n",
             PPM_GetChannel(0), PPM_GetChannel(1),
             PPM_GetChannel(2), PPM_GetChannel(3),
             PPM_GetChannel(4), PPM_GetChannel(5),
             PPM_GetChannel(6), PPM_GetChannel(7));

    USART_SendString(kDebugPort, buffer);
}
