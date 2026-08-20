#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f10x.h"

void Motor_Init(void);
/* 设置 A 路带符号 PWM，范围为 -999~999，符号同时决定方向。 */
void Motor_SetDuty(int16_t motor_a);
void Motor_Stop(void);

#endif
