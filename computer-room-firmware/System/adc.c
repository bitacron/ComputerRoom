/**
 * @file adc.c
 * @brief STM32F103 通用 ADC 驱动模块（支持 ADC1 和 ADC2）
 * 
 * 本模块提供 ADC 的初始化和读取功能，特点：
 * - 自动防止重复初始化（通过静态标志）
 * - 提供统一的读取函数，可指定 ADC、通道和采样时间
 * - 适用于多传感器共享 ADC 的场景
 * 
 * 使用前需要在 adc.h 中声明以下函数：
 *   void ADC1_Init(void);
 *   void ADC2_Init(void);
 *   uint16_t ADC_Read(ADC_TypeDef* ADCx, uint8_t channel, uint8_t sampleTime);
 */

#include "adc.h"          // 包含函数声明和必要的宏
#include "Delay.h"          // 包含函数声明和必要的宏
#include "stm32f10x.h"    // STM32F10x 标准外设库头文件

// 静态标志，记录 ADC 是否已初始化
static uint8_t adc1_initialized = 0;   // ADC1 初始化标志
static uint8_t adc2_initialized = 0;   // ADC2 初始化标志

/**
 * @brief 初始化 ADC1（仅执行一次）
 * 
 * 该函数配置 ADC1 为独立模式、单次转换、数据右对齐，
 * 并使能 ADC1 并进行校准。内部使用静态标志确保无论
 * 被调用多少次，硬件初始化只执行一次。
 * 
 * @note 必须在调用 ADC_Read(ADC1, ...) 之前调用此函数。
 */
void ADC1_Init(void)
{
    if (adc1_initialized) {
        return;  // 已经初始化过，直接返回
    }
    // 1. 配置 ADC 时钟分频（72MHz / 6 = 12MHz，符合 ≤14MHz 要求）
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    // 使能 ADC1 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // ADC 结构体配置
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;          // 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;               // 单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;         // 单次转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      // 右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                     // 规则通道数量
    ADC_Init(ADC1, &ADC_InitStructure);

    // 使能 ADC1
    ADC_Cmd(ADC1, ENABLE);

    // 复位校准
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));

    // 开始校准
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    adc1_initialized = 1;  // 标记已初始化
}

/**
 * @brief 初始化 ADC2（仅执行一次）
 * 
 * 功能与 ADC1_Init 相同，但针对 ADC2。使用静态标志确保
 * 硬件只初始化一次。
 * 
 * @note 必须在调用 ADC_Read(ADC2, ...) 之前调用此函数。
 */
void ADC2_Init(void)
{
    if (adc2_initialized) {
        return;
    }
    // 1. 配置 ADC 时钟分频（72MHz / 6 = 12MHz，符合 ≤14MHz 要求）
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    // 使能 ADC2 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC2, &ADC_InitStructure);

    ADC_Cmd(ADC2, ENABLE);
    ADC_ResetCalibration(ADC2);
    while (ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while (ADC_GetCalibrationStatus(ADC2));

    adc2_initialized = 1;
}

/**
 * @brief 通用 ADC 读取函数
 * 
 * 该函数配置指定 ADC 的规则通道，启动一次软件转换，
 * 等待转换完成并返回 12 位结果。
 * 
 * @param ADCx        ADC 实例，可以是 ADC1 或 ADC2
 * @param channel     ADC 通道号，如 ADC_Channel_1, ADC_Channel_9 等
 * @param sampleTime  采样时间，如 ADC_SampleTime_55Cycles5
 * @return uint16_t   12位 ADC 转换值（0-4095）
 * 
 * @note 调用此函数前，必须确保对应的 ADC 已通过 ADC1_Init 或 ADC2_Init 初始化。
 * @note 对于共享同一 ADC 的多个通道，此函数会自动切换通道，无需额外操作。
 */
uint16_t ADC_GetAnalog(ADC_TypeDef* ADCx, uint8_t channel, uint8_t sampleTime)
{
    // 配置规则组通道：指定通道、采样时间、序列位置（此处为第一个序列）
    ADC_RegularChannelConfig(ADCx, channel, 1, sampleTime);
	
    Delay_us(10);   // 可选：等待通道稳定
	
    // 启动软件转换
    ADC_SoftwareStartConvCmd(ADCx, ENABLE);

    // 等待转换完成（EOC = End of Conversion）
    while (ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET);

    // 返回转换结果
    return ADC_GetConversionValue(ADCx);
}
