#include "MyAttitudeControl.h"

static float Attitude_Clamp(float value, float min, float max)
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

void AttitudeController_Init(AttitudeController_t *controller, float angle_kp, float yaw_angle_kp,
                             const PIDController_t *roll_rate_pid, const PIDController_t *pitch_rate_pid, const PIDController_t *yaw_rate_pid)
{
    if (controller == NULL || roll_rate_pid == NULL || pitch_rate_pid == NULL || yaw_rate_pid == NULL)
    {
        return;
    }

    controller->angle_kp = angle_kp;
    controller->yaw_angle_kp = yaw_angle_kp;
    controller->roll_rate_pid = *roll_rate_pid;
    controller->pitch_rate_pid = *pitch_rate_pid;
    controller->yaw_rate_pid = *yaw_rate_pid;
}

void AttitudeController_Reset(AttitudeController_t *controller)
{
    if (controller == NULL)
    {
        return;
    }

    PID_Reset(&controller->roll_rate_pid);
    PID_Reset(&controller->pitch_rate_pid);
    PID_Reset(&controller->yaw_rate_pid);
}

void AttitudeController_Update(AttitudeController_t *controller, const AttitudeSetpoint_t *setpoint,
                               const AttitudeState_t *state, float dt, MotorMixerInput_t *output)
{
    if (controller == NULL || setpoint == NULL || state == NULL || output == NULL || dt <= 0.0f)
    {
        return;
    }

    float roll_rate_target = controller->angle_kp * (setpoint->roll - state->attitude.roll);
    float pitch_rate_target = controller->angle_kp * (setpoint->pitch - state->attitude.pitch);
    float yaw_rate_target = controller->yaw_angle_kp * (setpoint->yaw - state->attitude.yaw);

    float roll_rate_error = roll_rate_target - state->rates.roll;
    float pitch_rate_error = pitch_rate_target - state->rates.pitch;
    float yaw_rate_error = yaw_rate_target - state->rates.yaw;

    output->roll = PID_Update(&controller->roll_rate_pid, roll_rate_error, state->rates.roll, dt);
    output->pitch = PID_Update(&controller->pitch_rate_pid, pitch_rate_error, state->rates.pitch, dt);
    output->yaw = PID_Update(&controller->yaw_rate_pid, yaw_rate_error, state->rates.yaw, dt);
    output->throttle = MotorMixer_Clamp(setpoint->throttle, MOTOR_MIXER_OUTPUT_MIN, MOTOR_MIXER_OUTPUT_MAX);

    output->roll = Attitude_Clamp(output->roll, -1.0f, 1.0f);
    output->pitch = Attitude_Clamp(output->pitch, -1.0f, 1.0f);
    output->yaw = Attitude_Clamp(output->yaw, -1.0f, 1.0f);
}
