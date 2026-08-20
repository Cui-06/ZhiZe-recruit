#ifndef CONTROL_H
#define CONTROL_H

#include "stm32f10x.h"

typedef struct
{
    int32_t position;              /* 实际位置，单位 count。 */
    int32_t speed;                 /* 实际速度，单位 count/s。 */
    int32_t pid_target_position;   /* 电位器设定并直接送入 PID 的目标位置。 */
    int32_t pid_error;             /* PID 位置误差。 */
    int32_t pid_output;            /* PID 输出，即带符号 PWM。 */
    int32_t target_position;       /* 电位器设置的最终目标位置。 */
    int32_t output_limit;          /* 按钮选择的 PID/PWM 输出上限。 */
} ControlStatus_TypeDef;

/* 启动 10 ms 单环位置 PID 控制定时器。 */
void Control_Init(void);
/* 每约 20 ms 返回一次用于 OLED 和串口显示的状态快照。 */
unsigned char Control_ReadStatus(ControlStatus_TypeDef *status);

#endif
