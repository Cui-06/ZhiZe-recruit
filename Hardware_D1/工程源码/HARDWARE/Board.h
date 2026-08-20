#ifndef HARDWARE_BOARD_H
#define HARDWARE_BOARD_H

#include "stm32f10x.h"

/* D153C/TB6612 接线图对应的 STM32 引脚定义。 */
/* 三个按键避开 PWM、编码器和 ADC 所使用的引脚。 */
#define KEY_PORT       GPIOA
#define KEY1_PIN       GPIO_Pin_2
#define KEY2_PIN       GPIO_Pin_3
#define KEY3_PIN       GPIO_Pin_4
#define KEY_MASK       (KEY1_PIN | KEY2_PIN | KEY3_PIN)

/* 电机 A：PWMA=PB1，AIN1=PB14，AIN2=PB15。 */
#define MOTOR_A_PORT   GPIOB
#define MOTOR_A_PWM    GPIO_Pin_1
#define MOTOR_AIN1     GPIO_Pin_14
#define MOTOR_AIN2     GPIO_Pin_15

/* STBY 按接线图外接 3.3 V 或 5 V，保持驱动板有效。 */

/* 电机 A 编码器：TIM4_CH1/CH2 = PB6/PB7。 */
#define ENCODER_A_PORT GPIOB
#define ENCODER_A1_PIN GPIO_Pin_6
#define ENCODER_A2_PIN GPIO_Pin_7

/* 电机电源电压采样：ADC1 通道 6 = PA6。 */
#define ADC_PORT       GPIOA
#define ADC_PIN        GPIO_Pin_6
#define ADC_CHANNEL    ADC_Channel_6

void Board_Init(void);

#endif
