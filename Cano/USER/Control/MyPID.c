#include "MyPID.h"

static float PID_Clamp(float value, float min, float max)
{
    if (value < min)
    {
        return min;
    }
    if (value > max)
    {
        return max;
    }
    return value;
}

void PID_Init(PIDController_t *pid, float kp, float ki, float kd, float integral_limit, float output_limit, float derivative_lpf)
{
    if (pid == NULL)
    {
        return;
    }

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integrator = 0.0f;
    pid->prev_measure = 0.0f;
    pid->prev_derivative = 0.0f;
    pid->integral_limit = integral_limit;
    pid->output_limit = output_limit;
    pid->derivative_lpf = derivative_lpf;
    pid->initialized = false;
}

void PID_Reset(PIDController_t *pid)
{
    if (pid == NULL)
    {
        return;
    }

    pid->integrator = 0.0f;
    pid->prev_measure = 0.0f;
    pid->prev_derivative = 0.0f;
    pid->initialized = false;
}

float PID_Update(PIDController_t *pid, float error, float measurement, float dt)
{
    if (pid == NULL || dt <= 0.0f)
    {
        return 0.0f;
    }

    if (!pid->initialized)
    {
        pid->prev_measure = measurement;
        pid->prev_derivative = 0.0f;
        pid->initialized = true;
    }

    pid->integrator += pid->ki * error * dt;
    pid->integrator = PID_Clamp(pid->integrator, -pid->integral_limit, pid->integral_limit);

    float derivative_raw = -(measurement - pid->prev_measure) / dt;
    float alpha = PID_Clamp(pid->derivative_lpf, 0.0f, 1.0f);
    float derivative = pid->prev_derivative + alpha * (derivative_raw - pid->prev_derivative);

    float output = pid->kp * error + pid->integrator + pid->kd * derivative;
    output = PID_Clamp(output, -pid->output_limit, pid->output_limit);

    pid->prev_measure = measurement;
    pid->prev_derivative = derivative;

    return output;
}
