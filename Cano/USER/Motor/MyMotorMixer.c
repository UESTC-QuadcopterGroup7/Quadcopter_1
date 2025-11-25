#include "MyMotorMixer.h"

#include "MyDShot.h"

/* ============================================================
   Betaflight 风格的 QUADX 混控矩阵
   参考 Betaflight mixer: thrust 始终为 1，其余系数决定姿态响应
   顺序对应：M1=右前，M2=左前，M3=右后，M4=左后
   ============================================================ */
static const float kQuadXRoll[MOTOR_MIXER_MOTOR_COUNT]  = {-1.0f, 1.0f, -1.0f, 1.0f};
static const float kQuadXPitch[MOTOR_MIXER_MOTOR_COUNT] = { 1.0f, 1.0f, -1.0f,-1.0f};
static const float kQuadXYaw[MOTOR_MIXER_MOTOR_COUNT]   = { 1.0f, 1.0f, -1.0f,-1.0f};

static void MotorMixer_Normalize(MotorMixerOutput_t *output);

float MotorMixer_Clamp(float value, float min, float max)
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

void MotorMixer_Init(void)
{
    DShot_Init_All();
}

void MotorMixer_ComputeQuadX(const MotorMixerInput_t *input, MotorMixerOutput_t *output)
{
    if (input == NULL || output == NULL)
    {
        return;
    }

    MotorMixerOutput_t mixed = {0};
    const float throttle = MotorMixer_Clamp(input->throttle, MOTOR_MIXER_OUTPUT_MIN, MOTOR_MIXER_OUTPUT_MAX);
    const float roll = MotorMixer_Clamp(input->roll, -1.0f, 1.0f);
    const float pitch = MotorMixer_Clamp(input->pitch, -1.0f, 1.0f);
    const float yaw = MotorMixer_Clamp(input->yaw, -1.0f, 1.0f);

    for (uint8_t i = 0; i < MOTOR_MIXER_MOTOR_COUNT; i++)
    {
        mixed.motors[i] = throttle + roll * kQuadXRoll[i] + pitch * kQuadXPitch[i] + yaw * kQuadXYaw[i];
    }

    MotorMixer_Normalize(&mixed);

    *output = mixed;
}

uint16_t MotorMixer_ToDShotValue(float normalized)
{
    float clamped = MotorMixer_Clamp(normalized, MOTOR_MIXER_OUTPUT_MIN, MOTOR_MIXER_OUTPUT_MAX);
    float span = (float)(MOTOR_MIXER_DSHOT_ACTIVE - MOTOR_MIXER_DSHOT_IDLE);
    return (uint16_t)(MOTOR_MIXER_DSHOT_IDLE + (clamped * span));
}

void MotorMixer_SendDShotAll(const MotorMixerOutput_t *output)
{
    if (output == NULL)
    {
        return;
    }

    uint16_t m1 = MotorMixer_ToDShotValue(output->motors[0]);
    uint16_t m2 = MotorMixer_ToDShotValue(output->motors[1]);
    uint16_t m3 = MotorMixer_ToDShotValue(output->motors[2]);
    uint16_t m4 = MotorMixer_ToDShotValue(output->motors[3]);

    DShot_Send_All(m1, m2, m3, m4);
}

static void MotorMixer_Normalize(MotorMixerOutput_t *output)
{
    float min_val = output->motors[0];
    float max_val = output->motors[0];

    for (uint8_t i = 1; i < MOTOR_MIXER_MOTOR_COUNT; i++)
    {
        if (output->motors[i] < min_val)
        {
            min_val = output->motors[i];
        }
        if (output->motors[i] > max_val)
        {
            max_val = output->motors[i];
        }
    }

    if (min_val < MOTOR_MIXER_OUTPUT_MIN)
    {
        float offset = -min_val;
        for (uint8_t i = 0; i < MOTOR_MIXER_MOTOR_COUNT; i++)
        {
            output->motors[i] += offset;
        }
        max_val += offset;
    }

    if (max_val > MOTOR_MIXER_OUTPUT_MAX)
    {
        float scale = MOTOR_MIXER_OUTPUT_MAX / max_val;
        for (uint8_t i = 0; i < MOTOR_MIXER_MOTOR_COUNT; i++)
        {
            output->motors[i] *= scale;
        }
    }

    for (uint8_t i = 0; i < MOTOR_MIXER_MOTOR_COUNT; i++)
    {
        output->motors[i] = MotorMixer_Clamp(output->motors[i], MOTOR_MIXER_OUTPUT_MIN, MOTOR_MIXER_OUTPUT_MAX);
    }
}
