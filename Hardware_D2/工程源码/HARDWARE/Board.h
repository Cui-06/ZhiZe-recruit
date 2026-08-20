#ifndef HARDWARE_BOARD_H
#define HARDWARE_BOARD_H

#include "stm32f10x.h"

/* K1 使用内部上拉，按下时将 PA2 接地。 */
#define KEY_PORT GPIOA
#define KEY1_PIN GPIO_Pin_2
#define KEY_MASK KEY1_PIN

/* D153C/TB6612 接线图对应的 STM32 引脚定义。 */
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

/* 目标位置电位器：中间抽头接 ADC1 通道 0 = PA0。 */
#define POT_PORT       GPIOA
#define POT_PIN        GPIO_Pin_0
#define POT_CHANNEL    ADC_Channel_0

/* 0.96 英寸 OLED：使用 PB10/PB11 模拟 IIC 的 SCL/SDA。 */
#define OLED_PORT      GPIOB
#define OLED_SCL_PIN   GPIO_Pin_10
#define OLED_SDA_PIN   GPIO_Pin_11

void Board_Init(void);

#endif
