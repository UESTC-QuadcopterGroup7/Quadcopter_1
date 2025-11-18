#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int16_t acc[3];
    int16_t gyro[3];
    int16_t mag[3];
    float temperature_c;
} SensorFrame;

int SensorManager_Init(void);
void SensorManager_Read(SensorFrame *frame);
void SensorManager_Format(const SensorFrame *frame, char *buffer, size_t buffer_size);

#endif
