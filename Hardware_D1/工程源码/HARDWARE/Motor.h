#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f10x.h"

/* 四个速度状态：停止、低速、中速、高速。 */
#define MOTOR_SPEED_STOP 0U
#define MOTOR_SPEED_LOW  1U
#define MOTOR_SPEED_MID  2U
#define MOTOR_SPEED_HIGH 3U

#define MOTOR_DIR_FORWARD 0U
#define MOTOR_DIR_REVERSE 1U

void Motor_Init(void);
void Motor_SetSpeed(unsigned char speed); /* 设置停止、低速、中速或高速。 */
void Motor_SetDir(unsigned char dir);    /* 设置正转或反转方向。 */
/* 设置 A 路带符号 PWM，占空比符号同时决定方向。 */
void Motor_SetDuty(int16_t motor_a);
void Motor_Stop(void);

#endif
