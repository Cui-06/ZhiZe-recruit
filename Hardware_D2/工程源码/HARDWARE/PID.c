#include "PID.h"

#define PID_KP             250L
#define PID_KI             1L
#define PID_KD             80L
#define PID_SCALE          1000L
#define PID_ERROR_LIMIT    100000L
#define PID_INTEGRAL_LIMIT 200000L
#define PID_OUTPUT_LIMIT   999L
#define PID_DEADBAND       5L

static int32_t PID_Limit(int32_t value, int32_t limit)
{
    /* 对称限幅，同时适用于误差、积分和输出。 */
    if (value > limit) return limit;
    if (value < -limit) return -limit;
    return value;
}

void PositionPID_Init(PositionPID_TypeDef *pid)
{
    PositionPID_Reset(pid);
}

void PositionPID_Reset(PositionPID_TypeDef *pid)
{
    /* 停止或重新启动控制时清零历史项，防止积分残留造成冲击。 */
    pid->integral = 0L;
    pid->last_error = 0L;
}

int16_t PositionPID_Update(PositionPID_TypeDef *pid,
                           int32_t target_position,
                           int32_t current_position)
{
    int32_t error;
    int32_t derivative;
    int32_t output;

    /* 误差过大时先限幅，防止后续乘法和积分累加溢出。 */
    error = PID_Limit(target_position - current_position, PID_ERROR_LIMIT);
    /* 进入位置死区后停止输出，避免电机在目标附近反复抖动。 */
    if (error >= -PID_DEADBAND && error <= PID_DEADBAND)
    {
        pid->integral = 0L;
        pid->last_error = error;
        return 0;
    }

    /* 积分限幅用于抑制积分饱和；微分项反映本周期误差变化量。 */
    pid->integral = PID_Limit(pid->integral + error,
                              PID_INTEGRAL_LIMIT);
    derivative = error - pid->last_error;
    pid->last_error = error;

    /* 三项相加后除以缩放系数，恢复为实际 PWM 输出量。 */
    output = (PID_KP * error +
              PID_KI * pid->integral +
              PID_KD * derivative) / PID_SCALE;
    /* 输出限幅与 TIM3 的 PWM 周期 999 对应。 */
    output = PID_Limit(output, PID_OUTPUT_LIMIT);
    return (int16_t)output;
}
