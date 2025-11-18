#include "SensorManager.h"
#include "MyMPU6050.h"
#include "MyHMC5883L.h"
#include <stdio.h>

int SensorManager_Init(void)
{
    if (MPU6050_Init() != 0) {
        return -1;
    }

    if (HMC5883L_Init() != 0) {
        return -2;
    }

    return 0;
}

void SensorManager_Read(SensorFrame *frame)
{
    if (frame == NULL) {
        return;
    }

    int16_t temperature_raw = 0;
    MPU6050_GetData(&frame->acc[0], &frame->acc[1], &frame->acc[2],
                    &frame->gyro[0], &frame->gyro[1], &frame->gyro[2],
                    &temperature_raw);
    frame->temperature_c = MPU6050_ConvertTemperature(temperature_raw);

    HMC5883L_GetData(&frame->mag[0], &frame->mag[1], &frame->mag[2]);
}

void SensorManager_Format(const SensorFrame *frame, char *buffer, size_t buffer_size)
{
    if (frame == NULL || buffer == NULL || buffer_size == 0) {
        return;
    }

    snprintf(buffer, buffer_size,
             "ACC:%d,%d,%d GYRO:%d,%d,%d MAG:%d,%d,%d TEMP:%.2f\r\n",
             frame->acc[0], frame->acc[1], frame->acc[2],
             frame->gyro[0], frame->gyro[1], frame->gyro[2],
             frame->mag[0], frame->mag[1], frame->mag[2],
             frame->temperature_c);
}
