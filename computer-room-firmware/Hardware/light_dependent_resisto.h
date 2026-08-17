#ifndef __LIGHT_DEPENDENT_RESISTO_H
#define __LIGHT_DEPENDENT_RESISTO_H

// 光敏电阻 模拟输出引脚 PB1 -> ADC1 通道9
#define LDR_ADC          ADC1
#define LDR_CHANNEL      ADC_Channel_9
#define LDR_SAMPLE_TIME  ADC_SampleTime_55Cycles5

// 采样次数（用于模拟量平均滤波）
#define LDRE_ADC_SAMPLES   10

void LDR_Init_DO(void);
uint8_t LDR_GetDigital(void);
void LDR_Init_AO(void);
uint16_t LDR_GetAnalog(void);
uint16_t LDR_GetAnalogAverage(void);
uint8_t LDR_GetPercentage(void);

#endif
