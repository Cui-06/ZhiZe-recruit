#ifndef MOTOR_H
#define MOTOR_H

#define MOTOR_SPEED_STOP 0
#define MOTOR_SPEED_LOW  1
#define MOTOR_SPEED_MID  2
#define MOTOR_SPEED_HIGH 3

#define MOTOR_DIR_FORWARD 0
#define MOTOR_DIR_REVERSE 1

void Motor_Init(void);
void Motor_SetSpeed(unsigned char speed);
void Motor_SetDir(unsigned char dir);
void Motor_Tick(void);

#endif
