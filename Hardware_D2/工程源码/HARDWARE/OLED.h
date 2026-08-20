#ifndef OLED_H
#define OLED_H

#include "stm32f10x.h"

/* 初始化 0.96 英寸 SSD1306 OLED，初始化后显示位置和速度。 */
void OLED_Init(void);
/* 更新实际位置、速度、目标位置和当前 PID/PWM 输出上限。 */
void OLED_ShowStatus(int32_t position,
                     int32_t speed,
                     int32_t target_position,
                     int32_t output_limit);

#endif
