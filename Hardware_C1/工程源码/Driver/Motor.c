#include "reg52.h"
#include "Motor.h"

sbit MOTOR_AIN1 = P1^0;
sbit MOTOR_AIN2 = P1^1;

static volatile unsigned char Motor_Speed = MOTOR_SPEED_STOP;
static volatile unsigned char Motor_Dir = MOTOR_DIR_FORWARD;
static volatile unsigned char Motor_PwmCount = 0;

static unsigned char Motor_GetDuty(unsigned char speed)   //根据速度档位返回对应的 PWM 占空比。
{
    // PWM 周期分为 10 份，三档速度对应不同占空比
    if (speed == MOTOR_SPEED_LOW)
    {
        return 2;
    }
    if (speed == MOTOR_SPEED_MID)
    {
        return 4;
    }
    if (speed == MOTOR_SPEED_HIGH)
    {
        return 7;
    }
    return 0;
}

void Motor_Init(void)   //初始化为停止、正转状态
{
    MOTOR_AIN1 = 0;
    MOTOR_AIN2 = 0;
    Motor_Speed = MOTOR_SPEED_STOP;
    Motor_Dir = MOTOR_DIR_FORWARD;
    Motor_PwmCount = 0;
}

void Motor_SetSpeed(unsigned char speed)   //设置电机速度档位，并清零 PWM 计数器
{
    Motor_Speed = speed;
    if (Motor_Speed > MOTOR_SPEED_HIGH)
    {
        Motor_Speed = MOTOR_SPEED_STOP;
    }
    Motor_PwmCount = 0;
}

void Motor_SetDir(unsigned char dir)  //设置电机旋转方向
{
    Motor_Dir = dir ? MOTOR_DIR_REVERSE : MOTOR_DIR_FORWARD;
}

void Motor_Tick(void)   //每次定时器中断执行一次，输出电机软件 PWM
{
    unsigned char duty = Motor_GetDuty(Motor_Speed);
    bit pwm_out;

    // 停止时两个驱动输入均输出低电平
    if (duty == 0)
    {
        MOTOR_AIN1 = 0;
        MOTOR_AIN2 = 0;
        return;
    }

    Motor_PwmCount++;
    if (Motor_PwmCount >= 10)
    {
        Motor_PwmCount = 0;
    }

    // 根据占空比生成软件 PWM。 
    pwm_out = (Motor_PwmCount < duty) ? 1 : 0;
    if (Motor_Dir == MOTOR_DIR_FORWARD)
    {
        MOTOR_AIN1 = pwm_out;
        MOTOR_AIN2 = 0;
    }
    else
    {
        MOTOR_AIN1 = 0;
        MOTOR_AIN2 = pwm_out;
    }
}
