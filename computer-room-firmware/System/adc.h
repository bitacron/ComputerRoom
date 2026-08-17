#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

// 初始化ADC1（自动确保只执行一次）
void ADC1_Init(void);

// 初始化ADC2（自动确保只执行一次）
void ADC2_Init(void);

// 通用读取函数：从指定的ADC和通道读取一次转换值
// 参数: ADCx - ADC1 或 ADC2
//       channel - 通道号 (如 ADC_Channel_1, ADC_Channel_9 等)
//       sampleTime - 采样时间 (如 ADC_SampleTime_55Cycles5)
// 返回: 12位ADC值 (0-4095)
uint16_t ADC_GetAnalog(ADC_TypeDef* ADCx, uint8_t channel, uint8_t sampleTime);

#endif
