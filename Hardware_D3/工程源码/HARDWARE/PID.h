#ifndef PID_H
#define PID_H

#include "stm32f10x.h"

typedef struct
{
    int32_t last_error;    /* 保存位置误差，便于观察和后续扩展。 */
} PositionPID_TypeDef;

typedef struct
{
    int32_t integral;      /* 速度误差累计值。 */
    int32_t last_target;   /* 上一次目标速度，用于识别换向。 */
} SpeedPID_TypeDef;

void PositionPID_Init(PositionPID_TypeDef *pid);
void PositionPID_Reset(PositionPID_TypeDef *pid);
/*
 * 改进位置环：比例项决定趋近速度，实际速度反馈作为微分阻尼。
 * 输入位置单位为 count、速度单位为 count/s，输出目标速度 count/s。
 */
int32_t PositionPID_Update(PositionPID_TypeDef *pid,
                           int32_t target_position,
                           int32_t current_position,
                           int32_t current_speed);

void SpeedPID_Init(SpeedPID_TypeDef *pid);
void SpeedPID_Reset(SpeedPID_TypeDef *pid);
/* 改进速度环：速度前馈 + PI + 条件积分抗饱和，输出带符号 PWM。 */
int16_t SpeedPID_Update(SpeedPID_TypeDef *pid,
                        int32_t target_speed,
                        int32_t current_speed);

#endif
