#include "PID.h"

/* 所有参数采用 1000 倍定点数，避免 STM32F1 进行浮点运算。 */
#define PID_SCALE 1000L

/*
 * 位置环采用对测量速度做微分的 PD 形式：
 * target_speed = Kp * position_error - Kd * current_speed。
 * 微分项不直接作用于会跳变的目标位置，因此不会产生微分冲击。
 */
#define POSITION_KP           2500L
#define POSITION_KD           250L
#define POSITION_ERROR_LIMIT  100000L
#define POSITION_OUTPUT_LIMIT 10000L
#define POSITION_DEADBAND     5L

/*
 * 速度环采用前馈 PI。前馈提供维持目标速度所需的大部分 PWM，
 * PI 只修正负载和模型误差，因此响应更快、积分负担更小。
 */
#define SPEED_KP               60L
#define SPEED_KI               4L
#define SPEED_FEEDFORWARD      90L
#define SPEED_ERROR_LIMIT      30000L
#define SPEED_INTEGRAL_LIMIT   150000L
#define SPEED_OUTPUT_LIMIT     999L
#define SPEED_ERROR_DEADBAND   50L
#define SPEED_STOP_THRESHOLD   100L

static int32_t PID_Limit(int32_t value, int32_t limit)
{
    if (value > limit) return limit;
    if (value < -limit) return -limit;
    return value;
}

static int32_t PID_Abs(int32_t value)
{
    return (value < 0L) ? -value : value;
}

void PositionPID_Init(PositionPID_TypeDef *pid)
{
    PositionPID_Reset(pid);
}

void PositionPID_Reset(PositionPID_TypeDef *pid)
{
    pid->last_error = 0L;
}

int32_t PositionPID_Update(PositionPID_TypeDef *pid,
                           int32_t target_position,
                           int32_t current_position,
                           int32_t current_speed)
{
    int32_t error;
    int32_t output;

    error = PID_Limit(target_position - current_position,
                      POSITION_ERROR_LIMIT);
    pid->last_error = error;

    /* 到达目标位置后令目标速度为 0，由内环主动制动至静止。 */
    if (PID_Abs(error) <= POSITION_DEADBAND)
    {
        return 0L;
    }

    /* 速度反馈提供阻尼，使电机接近目标位置时提前减速，减小过冲。 */
    output = (POSITION_KP * error - POSITION_KD * current_speed) /
             PID_SCALE;
    return PID_Limit(output, POSITION_OUTPUT_LIMIT);
}

void SpeedPID_Init(SpeedPID_TypeDef *pid)
{
    SpeedPID_Reset(pid);
}

void SpeedPID_Reset(SpeedPID_TypeDef *pid)
{
    pid->integral = 0L;
    pid->last_target = 0L;
}

int16_t SpeedPID_Update(SpeedPID_TypeDef *pid,
                        int32_t target_speed,
                        int32_t current_speed)
{
    int32_t error;
    int32_t candidate_integral;
    int32_t candidate_output;
    int32_t output;
    unsigned char direction_changed;

    direction_changed = ((target_speed > 0L && pid->last_target < 0L) ||
                         (target_speed < 0L && pid->last_target > 0L)) ? 1U : 0U;

    /* 换向时清除旧方向的积分，避免电机反向响应迟缓。 */
    if (direction_changed != 0U)
    {
        pid->integral = 0L;
    }
    pid->last_target = target_speed;

    /* 进入制动阶段时立即释放原方向积分，避免积分残留拖慢停车。 */
    if (target_speed == 0L)
    {
        pid->integral = 0L;
        /* 电机已经基本停止后关闭输出，避免静止抖动和啸叫。 */
        if (PID_Abs(current_speed) <= SPEED_STOP_THRESHOLD)
        {
            return 0;
        }
    }

    error = PID_Limit(target_speed - current_speed, SPEED_ERROR_LIMIT);
    if (PID_Abs(error) <= SPEED_ERROR_DEADBAND)
    {
        /* 误差进入死区后保留积分和前馈，不能像普通死区那样输出 0。 */
        error = 0L;
    }

    candidate_integral = PID_Limit(pid->integral + error,
                                   SPEED_INTEGRAL_LIMIT);
    candidate_output = (SPEED_FEEDFORWARD * target_speed +
                        SPEED_KP * error +
                        SPEED_KI * candidate_integral) / PID_SCALE;

    /*
     * 条件积分抗饱和：输出已经正向饱和时不再累计正误差，
     * 负向饱和时不再累计负误差；反向误差仍可释放积分。
     */
    if (!((candidate_output > SPEED_OUTPUT_LIMIT && error > 0L) ||
          (candidate_output < -SPEED_OUTPUT_LIMIT && error < 0L)))
    {
        pid->integral = candidate_integral;
    }

    output = (SPEED_FEEDFORWARD * target_speed +
              SPEED_KP * error +
              SPEED_KI * pid->integral) / PID_SCALE;
    return (int16_t)PID_Limit(output, SPEED_OUTPUT_LIMIT);
}
