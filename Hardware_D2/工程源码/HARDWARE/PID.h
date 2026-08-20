#ifndef PID_H
#define PID_H

#include "stm32f10x.h"

typedef struct
{
    int32_t integral;      /* 误差累计值，用于积分项。 */
    int32_t last_error;    /* 上一次误差，用于计算微分项。 */
} PositionPID_TypeDef;

/* 初始化/复位 PID 历史状态，不修改 PID 参数宏。 */
void PositionPID_Init(PositionPID_TypeDef *pid);
void PositionPID_Reset(PositionPID_TypeDef *pid);
/* 输入目标位置和实际位置，返回 -999~999 的带符号 PWM。 */
int16_t PositionPID_Update(PositionPID_TypeDef *pid,
                           int32_t target_position,
                           int32_t current_position);

#endif
