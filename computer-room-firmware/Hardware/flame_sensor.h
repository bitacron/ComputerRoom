#ifndef __FLAME_SENSOR_H
#define __FLAME_SENSOR_H

#include "stm32f10x.h"

// ========== 引脚定义（请根据实际连接修改）==========
// 火焰传感器数字输出引脚（DO）—— 如果不需要可以注释掉相关代码
#define FLAME_DO_PORT       GPIOA
#define FLAME_DO_PIN        GPIO_Pin_0
#define FLAME_DO_RCC        RCC_APB2Periph_GPIOA

// 火焰传感器模拟输出引脚（AO）—— 使用 PB0 (ADC2_IN8)
#define FLAME_AO_PORT       GPIOB
#define FLAME_AO_PIN        GPIO_Pin_0
#define FLAME_AO_RCC        RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC2
#define FLAME_ADC           ADC2
#define FLAME_ADC_CHANNEL   ADC_Channel_8   // PB0 对应 ADC2_IN8

// 采样次数（用于模拟量平均滤波）
#define FLAME_ADC_SAMPLES   10

// 函数声明
void Flame_Init(void);                      // 初始化火焰传感器
void Flame_Init_DO(void);   // 初始化数字输出引脚（DO）
void Flame_Init_AO_ADC(void);  // 初始化模拟输入引脚（AO）及 ADC
void Flame_Init_AO(void);   // 初始化模拟输入引脚（AO）
uint8_t Flame_GetDigital(void);              // 读取数字量输出（0或1）
uint16_t Flame_GetAnalog(void);               // 读取单次模拟量原始值（0-4095）
uint16_t Flame_GetAnalogAverage(void);        // 读取多次平均后的模拟量原始值
uint8_t Flame_GetPercentage(void);            // 获取火焰强度百分比（0-100%）

#endif
