#include "leak_sensor.h"
#include "delay.h"
#include "adc.h"

/*******************************************************************************
* 函数名：Leak_Init
* 描述  ：初始化漏水/雨滴传感器的 GPIO 和 ADC1，根据条件初始化DO还是AO
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Leak_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(LEAK_DO_RCC | LEAK_AO_RCC | RCC_APB2Periph_ADC1, ENABLE);

    // 2. 配置 DO 引脚为输入（如果定义了 DO 引脚）
#ifdef LEAK_DO_PORT
    GPIO_InitStructure.GPIO_Pin = LEAK_DO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_Init(LEAK_DO_PORT, &GPIO_InitStructure);
#endif

    // 3. 配置 AO 引脚为模拟输入
    GPIO_InitStructure.GPIO_Pin = LEAK_AO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(LEAK_AO_PORT, &GPIO_InitStructure);

    // 4. ADC1 配置
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(LEAK_ADC, &ADC_InitStructure);

    // 5. 配置 ADC1 规则组通道（采样时间 55.5 周期）
    ADC_RegularChannelConfig(LEAK_ADC, LEAK_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);

    // 6. 使能 ADC1 并校准
    ADC_Cmd(LEAK_ADC, ENABLE);
    ADC_ResetCalibration(LEAK_ADC);
    while (ADC_GetResetCalibrationStatus(LEAK_ADC));
    ADC_StartCalibration(LEAK_ADC);
    while (ADC_GetCalibrationStatus(LEAK_ADC));
}
/*******************************************************************************
* 函数名：Leak_Init_DO
* 描述  ：初始化漏水/雨滴传感器数字输出 DO（PA8）
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Leak_Init_DO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(LEAK_DO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LEAK_DO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
	GPIO_Init(LEAK_DO_PORT, &GPIO_InitStructure);
}
/*******************************************************************************
* 函数名：Leak_Init_AO_ADC
* 描述  ：初始化漏水/雨滴传感器模拟输入 AO 及 ADC1（PA5）
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Leak_Init_AO_ADC(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	// 1.使能 GPIO 和 ADC 时钟
	RCC_APB2PeriphClockCmd(LEAK_AO_RCC | RCC_APB2Periph_ADC1, ENABLE);

	// 2.配置 AO 引脚为模拟输入
	GPIO_InitStructure.GPIO_Pin = LEAK_AO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(LEAK_AO_PORT, &GPIO_InitStructure);

	// 3.ADC1 配置
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(LEAK_ADC, &ADC_InitStructure);
	// 4.配置 ADC1 规则组通道（采样时间 55.5 周期）
	ADC_RegularChannelConfig(LEAK_ADC, LEAK_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	// 5. 使能 ADC1 并校准
	ADC_Cmd(LEAK_ADC, ENABLE);
	ADC_ResetCalibration(LEAK_ADC);
	while (ADC_GetResetCalibrationStatus(LEAK_ADC));
	ADC_StartCalibration(LEAK_ADC);
	while (ADC_GetCalibrationStatus(LEAK_ADC));
}
/*******************************************************************************
* 函数名：Leak_Init_AO
* 描述  ：仅配置漏水/雨滴传感器 AO 引脚为模拟输入（PA5）
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Leak_Init_AO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// 2.配置 AO 引脚为模拟输入
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LEAK_AO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(LEAK_AO_PORT, &GPIO_InitStructure);
}
/*******************************************************************************
* 函数名：Leak_GetDigital
* 描述  ：读取漏水/雨滴传感器的数字量输出
* 输入  ：无
* 输出  ：0 = 无积水，1 = 检测到积水
* 注意  ：根据模块实际输出调整（通常感应板上有水时 DO 输出低电平）
*******************************************************************************/
uint8_t Leak_GetDigital(void)
{
	// 读取 DO 引脚电平，低电平表示有水（可根据需要取反）
	return (GPIO_ReadInputDataBit(LEAK_DO_PORT, LEAK_DO_PIN) == Bit_RESET) ? 1 : 0;
}

/*******************************************************************************
* 函数名：Leak_GetAnalog
* 描述  ：单次读取 ADC 值
* 输入  ：无
* 输出  ：12位 ADC 转换值（0-4095）
*******************************************************************************/
uint16_t Leak_GetAnalog(void)
{
		return ADC_GetAnalog(LEAK_ADC, LEAK_ADC_CHANNEL, ADC_SampleTime_55Cycles5);
}

/*******************************************************************************
* 函数名：Leak_GetAnalogAverage
* 描述  ：多次采样取平均，减少波动
* 输入  ：无
* 输出  ：平均后的 ADC 值
*******************************************************************************/
uint16_t Leak_GetAnalogAverage(void)
{
    uint32_t sum = 0;
    uint8_t i;

    for (i = 0; i < LEAK_ADC_SAMPLES; i++) {
        sum += Leak_GetAnalog();
        Delay_us(100);  // 两次采样间隔 100us
    }

    return (uint16_t)(sum / LEAK_ADC_SAMPLES);
}

/*******************************************************************************
* 函数名：Leak_GetPercentage
* 描述  ：获取积水程度百分比（0~100%）
* 输入  ：无
* 输出  ：百分比，值越大表示板上积水越多
* 说明  ：常见雨滴模块 ADC 值越小（电压越低）表示越湿，所以转换为 (1 - adc/4095) * 100
*******************************************************************************/
uint8_t Leak_GetPercentage(void)
{
    uint16_t adc_value = Leak_GetAnalogAverage();
    float percent = (1.0f - (float)adc_value / 4095.0f) * 100.0f;

    // 防止负值或超过100
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    return (uint8_t)percent;
}
