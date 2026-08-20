#include "Board.h"
#include "Motor.h"

/* TIM3 计数频率为 1 MHz，周期 999 对应约 1 kHz PWM。 */
#define MOTOR_PWM_PERIOD 999U

void Motor_Init(void)
{
    TIM_TimeBaseInitTypeDef timer;
    TIM_OCInitTypeDef oc;

    /* TIM3_CH4 通过 PB1 输出 TB6612 A 通道的 PWMA。 */
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

    Motor_Stop();
}

void Motor_SetDuty(int16_t motor_a)
{
    uint16_t duty;

    if (motor_a == 0)
    {
        Motor_Stop();
        return;
    }

    /* PWM 的正负号决定 AIN1/AIN2 电平，绝对值决定占空比。 */
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

    /* 防止比较值超过自动重装值，确保占空比不超过 100%。 */
    duty = (motor_a > MOTOR_PWM_PERIOD) ? MOTOR_PWM_PERIOD : (uint16_t)motor_a;
    TIM_SetCompare4(TIM3, duty);
}

void Motor_Stop(void)
{
    /* PWM=0 且 AIN1/AIN2 均为低电平，TB6612 A 通道停止输出。 */
    TIM_SetCompare4(TIM3, 0U);
    GPIO_ResetBits(MOTOR_A_PORT, MOTOR_AIN1 | MOTOR_AIN2);
}
