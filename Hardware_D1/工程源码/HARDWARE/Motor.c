#include "Board.h"
#include "Motor.h"

#define MOTOR_PWM_PERIOD 999U /* 1 MHz 计数时钟下，PWM 频率约为 1 kHz。 */

static volatile unsigned char Motor_Speed = MOTOR_SPEED_STOP;
static volatile unsigned char Motor_Dir = MOTOR_DIR_FORWARD;

static uint16_t Motor_GetDuty(unsigned char speed)
{
    /* 速度档位分别对应 20%、40%、70% 占空比。 */
    if (speed == MOTOR_SPEED_LOW) return 200U;
    if (speed == MOTOR_SPEED_MID) return 400U;
    if (speed == MOTOR_SPEED_HIGH) return 700U;
    return 0U;
}

static void Motor_SetDirection(unsigned char dir)
{
    /* 设置 TB6612 A 路方向。 */
    if (dir == MOTOR_DIR_FORWARD)
    {
        GPIO_SetBits(MOTOR_A_PORT, MOTOR_AIN1);
        GPIO_ResetBits(MOTOR_A_PORT, MOTOR_AIN2);
    }
    else
    {
        GPIO_ResetBits(MOTOR_A_PORT, MOTOR_AIN1);
        GPIO_SetBits(MOTOR_A_PORT, MOTOR_AIN2);
    }
}

static void Motor_ApplyOutput(void)
{
    uint16_t duty = Motor_GetDuty(Motor_Speed);

    TIM_SetCompare4(TIM3, duty);

    /* 停止时同时关闭 PWM 和方向输出，避免驱动板保持有效状态。 */
    if (Motor_Speed == MOTOR_SPEED_STOP)
    {
        GPIO_ResetBits(MOTOR_A_PORT, MOTOR_AIN1 | MOTOR_AIN2);
    }
    else
    {
        Motor_SetDirection(Motor_Dir);
    }
}

void Motor_Init(void)
{
    TIM_TimeBaseInitTypeDef timer;
    TIM_OCInitTypeDef oc;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    timer.TIM_Prescaler = 71U; /* 72 MHz / 72 = 1 MHz。 */
    timer.TIM_Period = MOTOR_PWM_PERIOD;
    timer.TIM_ClockDivision = TIM_CKD_DIV1;
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &timer);

    TIM_OCStructInit(&oc);
    oc.TIM_OCMode = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OCPolarity = TIM_OCPolarity_High;
    oc.TIM_Pulse = 0U;
    TIM_OC4Init(TIM3, &oc); /* PB1：PWMA。 */
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);

    /* 初始化为停止、正转状态。 */
    Motor_Speed = MOTOR_SPEED_STOP;
    Motor_Dir = MOTOR_DIR_FORWARD;
    Motor_ApplyOutput();
}

void Motor_SetSpeed(unsigned char speed)
{
    /* 速度档位超出范围时按停止处理。 */
    Motor_Speed = speed;
    if (Motor_Speed > MOTOR_SPEED_HIGH)
    {
        Motor_Speed = MOTOR_SPEED_STOP;
    }

    Motor_ApplyOutput();
}

void Motor_SetDir(unsigned char dir)
{
    /* 非 0 表示反转，0 表示正转。 */
    Motor_Dir = dir ? MOTOR_DIR_REVERSE : MOTOR_DIR_FORWARD;
    Motor_ApplyOutput();
}

void Motor_SetDuty(int16_t motor_a)
{
    uint16_t duty;

    if (motor_a < 0)
    {
        GPIO_ResetBits(MOTOR_A_PORT, MOTOR_AIN1);
        GPIO_SetBits(MOTOR_A_PORT, MOTOR_AIN2);
        motor_a = (int16_t)-motor_a;
    }
    else
    {
        GPIO_SetBits(MOTOR_A_PORT, MOTOR_AIN1);
        GPIO_ResetBits(MOTOR_A_PORT, MOTOR_AIN2);
    }

    duty = (motor_a > MOTOR_PWM_PERIOD) ? MOTOR_PWM_PERIOD : (uint16_t)motor_a;
    TIM_SetCompare4(TIM3, duty);
}

void Motor_Stop(void)
{
    Motor_Speed = MOTOR_SPEED_STOP;
    Motor_ApplyOutput();
}
