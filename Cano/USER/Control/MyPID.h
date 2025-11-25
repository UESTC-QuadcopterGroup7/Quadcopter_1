#ifndef __MY_PID_H
#define __MY_PID_H

#include <stdbool.h>

typedef struct
{
    float kp;
    float ki;
    float kd;
    float integrator;
    float prev_measure;
    float prev_derivative;
    float integral_limit;
    float output_limit;
    float derivative_lpf;
    bool initialized;
} PIDController_t;

void PID_Init(PIDController_t *pid, float kp, float ki, float kd, float integral_limit, float output_limit, float derivative_lpf);
void PID_Reset(PIDController_t *pid);
float PID_Update(PIDController_t *pid, float error, float measurement, float dt);

#endif /* __MY_PID_H */
