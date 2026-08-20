#include "Board.h"
#include "Encoder.h"

static void Encoder_TimerInit(TIM_TypeDef *tim)
{
    TIM_TimeBaseInitTypeDef timer;
    TIM_ICInitTypeDef input;

    /* 编码器模式使用定时器外部输入计数，不分频。 */
    timer.TIM_Prescaler = 0U;
    timer.TIM_Period = 0xFFFFU;
    timer.TIM_ClockDivision = TIM_CKD_DIV1;
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(tim, &timer);

    TIM_EncoderInterfaceConfig(tim, TIM_EncoderMode_TI12,
                               TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICStructInit(&input);
    /* 输入滤波抑制电机编码器的毛刺干扰。 */
    input.TIM_ICFilter = 6U;
    input.TIM_Channel = TIM_Channel_1;
    TIM_ICInit(tim, &input);
    input.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(tim, &input);
    TIM_SetCounter(tim, 0U);
    TIM_Cmd(tim, ENABLE);
}

void Encoder_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    Encoder_TimerInit(TIM4); /* 电机 A：PB6/PB7。 */
}

static int16_t Encoder_Read(TIM_TypeDef *tim)
{
    /* 将 16 位计数器解释为有符号增量，读取后清零便于计算速度。 */
    int16_t count = (int16_t)TIM_GetCounter(tim);
    TIM_SetCounter(tim, 0U);
    return count;
}

int16_t Encoder_ReadA(void)
{
    return Encoder_Read(TIM4);
}

void Encoder_Reset(void)
{
    TIM_SetCounter(TIM4, 0U);
}
