#include "Board.h"
#include "Adc.h"

void Adc_Init(void)
{
    ADC_InitTypeDef adc;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); /* ADC 时钟为 72 MHz / 6 = 12 MHz。 */

    /* 使用单次、软件触发转换；读取时再选择 PA6 或 PA0 对应通道。 */
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
    /* 上电校准可减小 ADC 零点和增益误差。 */
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) != RESET) {}
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) != RESET) {}
}

static uint16_t Adc_ReadChannel(uint8_t channel)
{
    /* ADC1 由电源采样和电位器共用，因此每次转换前重新选择通道。 */
    ADC_RegularChannelConfig(ADC1, channel, 1U, ADC_SampleTime_71Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {}
    return ADC_GetConversionValue(ADC1);
}

uint16_t Adc_Read(void)
{
    return Adc_ReadChannel(ADC_CHANNEL);
}

uint16_t Adc_ReadAverage(unsigned char samples)
{
    unsigned long total = 0UL;
    unsigned char i;

    /* 多次采样取平均值，以降低电机干扰造成的 ADC 波动。 */
    if (samples == 0U) samples = 1U;
    for (i = 0U; i < samples; i++) total += Adc_Read();
    return (uint16_t)(total / samples);
}

uint16_t Adc_ReadBatteryMillivolts(void)
{
    /* 驱动板分压比为 11:1：电池电压=ADC值×3.3×11/4096。 */
    return (uint16_t)(((uint32_t)Adc_ReadAverage(8U) * 36300UL) / 4096UL);
}

uint16_t Adc_ReadPotentiometer(void)
{
    uint32_t total = 0UL;
    unsigned char i;

    /* 连续取 4 次平均，减小电位器触点噪声。 */
    for (i = 0U; i < 4U; i++)
    {
        total += Adc_ReadChannel(POT_CHANNEL);
    }
    return (uint16_t)(total / 4UL);
}
