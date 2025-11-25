#ifndef __MY_ATTITUDE_CONTROL_H
#define __MY_ATTITUDE_CONTROL_H

#include "MyPID.h"
#include "MyMotorMixer.h"

typedef struct
{
    float roll;
    float pitch;
    float yaw;
} AttitudeEuler_t;

typedef struct
{
    float roll;
    float pitch;
    float yaw;
} RateVector_t;

typedef struct
{
    AttitudeEuler_t attitude;
    RateVector_t rates;
} AttitudeState_t;

typedef struct
{
    float roll;
    float pitch;
    float yaw;
    float throttle;
} AttitudeSetpoint_t;

typedef struct
{
    float angle_kp;
    float yaw_angle_kp;
    PIDController_t roll_rate_pid;
    PIDController_t pitch_rate_pid;
    PIDController_t yaw_rate_pid;
} AttitudeController_t;

void AttitudeController_Init(AttitudeController_t *controller, float angle_kp, float yaw_angle_kp,
                             const PIDController_t *roll_rate_pid, const PIDController_t *pitch_rate_pid, const PIDController_t *yaw_rate_pid);
void AttitudeController_Reset(AttitudeController_t *controller);
void AttitudeController_Update(AttitudeController_t *controller, const AttitudeSetpoint_t *setpoint,
                               const AttitudeState_t *state, float dt, MotorMixerInput_t *output);

#endif /* __MY_ATTITUDE_CONTROL_H */
