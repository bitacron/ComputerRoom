#ifndef __LEAK_SENSOR_H
#define __LEAK_SENSOR_H

#include "stm32f10x.h"

// ========== 引脚定义（请根据实际连接修改）==========
// 漏水/雨滴传感器数字输出引脚（DO）-- 如果不需要可以注释掉相关代码
#define LEAK_DO_PORT       GPIOA
#define LEAK_DO_PIN        GPIO_Pin_8
#define LEAK_DO_RCC        RCC_APB2Periph_GPIOA

// 漏水/雨滴传感器模拟输出引脚（AO）-- 使用 PA5 (ADC1_IN5)
#define LEAK_AO_PORT       GPIOA
#define LEAK_AO_PIN        GPIO_Pin_5
#define LEAK_AO_RCC        RCC_APB2Periph_GPIOA
#define LEAK_ADC           ADC1
#define LEAK_ADC_CHANNEL   ADC_Channel_5   // PA5 对应 ADC1_IN5

// 采样次数（用于模拟量平均滤波）
#define LEAK_ADC_SAMPLES   10

// 函数声明
void Leak_Init(void);                       // 初始化漏水传感器
void Leak_Init_DO(void);                    // 初始化数字输出引脚（DO）
void Leak_Init_AO_ADC(void);                // 初始化模拟输入引脚（AO）及 ADC
void Leak_Init_AO(void);                    // 初始化模拟输入引脚（AO）
uint8_t Leak_GetDigital(void);              // 读取数字量输出（0或1）
uint16_t Leak_GetAnalog(void);              // 读取单次模拟量原始值（0-4095）
uint16_t Leak_GetAnalogAverage(void);       // 读取多次平均后的模拟量原始值
uint8_t Leak_GetPercentage(void);           // 获取积水程度百分比（0-100%）

#endif
