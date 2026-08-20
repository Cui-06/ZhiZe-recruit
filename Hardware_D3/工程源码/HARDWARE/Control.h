#ifndef CONTROL_H
#define CONTROL_H

#include "stm32f10x.h"

typedef struct
{
    int32_t position;            /* 实际位置，单位 count。 */
    int32_t speed;               /* 滑动平均后的实际速度，单位 count/s。 */
    int32_t target_position;     /* 电位器设定的目标位置。 */
    int32_t target_speed;        /* 位置环输出的目标速度。 */
    int32_t pid_output;          /* 速度环输出的带符号 PWM。 */
    int32_t pid_error;           /* 位置环误差。 */
} ControlStatus_TypeDef;

/* 启动 10 ms 改进型位置-速度串级 PID 控制定时器。 */
void Control_Init(void);
/* 每约 20 ms 返回一次用于 OLED 和串口显示的状态快照。 */
unsigned char Control_ReadStatus(ControlStatus_TypeDef *status);

#endif
