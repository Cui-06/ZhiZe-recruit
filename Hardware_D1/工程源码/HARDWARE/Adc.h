#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"

/* 初始化 PA6 对应的 ADC1 通道 6。 */
void Adc_Init(void);
uint16_t Adc_Read(void);
uint16_t Adc_ReadAverage(unsigned char samples);
/* 按驱动板 11:1 分压关系返回电源电压，单位为 mV。 */
uint16_t Adc_ReadBatteryMillivolts(void);

#endif
