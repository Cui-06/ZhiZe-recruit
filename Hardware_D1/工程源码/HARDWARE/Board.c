#include "Board.h"

void Board_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_GPIOB, ENABLE);

    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;

    /* 配置 TB6612 A 路方向输出和 PWM 复用推挽输出。 */
    gpio.GPIO_Pin = MOTOR_AIN1 | MOTOR_AIN2;
    GPIO_Init(GPIOB, &gpio);
    gpio.GPIO_Pin = MOTOR_A_PWM;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &gpio);

    /* 按键使用内部上拉，按下时为低电平。 */
    gpio.GPIO_Pin = KEY_MASK;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY_PORT, &gpio);

    /* 配置电机 A 编码器输入。 */
    gpio.GPIO_Pin = ENCODER_A1_PIN | ENCODER_A2_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(ENCODER_A_PORT, &gpio);

    /* ADC 引脚必须配置为模拟输入，不能配置成普通数字输入。 */
    gpio.GPIO_Pin = ADC_PIN;
    gpio.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC_PORT, &gpio);

    /* 上电时关闭电机 A，避免初始化过程中误转。 */
    GPIO_ResetBits(MOTOR_A_PORT, MOTOR_AIN1 | MOTOR_AIN2);
}
