#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"

/* 初始化 ADC1；PA6 电源采样与 PA0 电位器共用该 ADC。 */
void Adc_Init(void);
/* 读取 PA6 电源采样通道的单次值或平均值。 */
uint16_t Adc_Read(void);
uint16_t Adc_ReadAverage(unsigned char samples);
/* 按驱动板 11:1 分压关系返回电源电压，单位为 mV。 */
uint16_t Adc_ReadBatteryMillivolts(void);
/* 读取 PA0 电位器，返回 0~4095。 */
uint16_t Adc_ReadPotentiometer(void);

#endif
