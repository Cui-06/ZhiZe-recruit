#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f10x.h"

/* 初始化电机 A 的 TIM4 编码器接口。 */
void Encoder_Init(void);
/* 读取并清零最近一段时间的编码器计数。 */
int16_t Encoder_ReadA(void);
void Encoder_Reset(void);

#endif
