#ifndef __MY_MOTOR_MIXER_H
#define __MY_MOTOR_MIXER_H

#include <stdint.h>

#define MOTOR_MIXER_MOTOR_COUNT 4
#define MOTOR_MIXER_OUTPUT_MIN 0.0f
#define MOTOR_MIXER_OUTPUT_MAX 1.0f

#define MOTOR_MIXER_DSHOT_IDLE   48U
#define MOTOR_MIXER_DSHOT_ACTIVE 2000U

/**
 * 输入命令：[-1, 1] 的姿态叠加 0~1 的油门。
 * 数值范围与 Betaflight 的 mixer 逻辑保持一致：
 *  - 油门范围 [0, 1]
 *  - 翻滚/俯仰/偏航为对称的 [-1, 1]
 */
typedef struct
{
    float roll;
    float pitch;
    float yaw;
    float throttle;
} MotorMixerInput_t;

/**
 * 归一化输出：每路电机 0~1，后续转换为 DShot 命令。
 */
typedef struct
{
    float motors[MOTOR_MIXER_MOTOR_COUNT];
} MotorMixerOutput_t;

void MotorMixer_Init(void);
void MotorMixer_ComputeQuadX(const MotorMixerInput_t *input, MotorMixerOutput_t *output);
void MotorMixer_SendDShotAll(const MotorMixerOutput_t *output);
uint16_t MotorMixer_ToDShotValue(float normalized);
float MotorMixer_Clamp(float value, float min, float max);

#endif /* __MY_MOTOR_MIXER_H */
