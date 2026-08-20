#include "Board.h"
#include "Adc.h"

void Adc_Init(void)
{
    ADC_InitTypeDef adc;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); /* ADC 时钟为 72 MHz / 6 = 12 MHz。 */

    ADC_DeInit(ADC1);
    adc.ADC_Mode = ADC_Mode_Independent;
    adc.ADC_ScanConvMode = DISABLE;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel = 1U;
    ADC_Init(ADC1, &adc);
    ADC_RegularChannelConfig(ADC1, ADC_CHANNEL, 1U, ADC_SampleTime_71Cycles5);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) != RESET) {}
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) != RESET) {}
}

uint16_t Adc_Read(void)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {}
    return ADC_GetConversionValue(ADC1);
}

uint16_t Adc_ReadAverage(unsigned char samples)
{
    unsigned long total = 0UL;
    unsigned char i;

    if (samples == 0U) samples = 1U;
    for (i = 0U; i < samples; i++) total += Adc_Read();
    return (uint16_t)(total / samples);
}

uint16_t Adc_ReadBatteryMillivolts(void)
{
    /* 驱动板分压比为 11:1：电池电压=ADC值×3.3×11/4096。 */
    return (uint16_t)(((uint32_t)Adc_ReadAverage(8U) * 36300UL) / 4096UL);
}
