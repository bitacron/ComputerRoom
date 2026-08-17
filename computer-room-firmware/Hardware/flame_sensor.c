#include "flame_sensor.h"
#include "delay.h"
#include "adc.h"

/*******************************************************************************
* 函数名：Flame_Init
* 描述  ：初始化火焰传感器的 GPIO 和 ADC2，根据条件初始化DO还是AO
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Flame_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(FLAME_DO_RCC | FLAME_AO_RCC, ENABLE);

    // 2. 配置 DO 引脚为输入（如果定义了 DO 引脚）
#ifdef FLAME_DO_PORT
    GPIO_InitStructure.GPIO_Pin = FLAME_DO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_Init(FLAME_DO_PORT, &GPIO_InitStructure);
#endif

    // 3. 配置 AO 引脚为模拟输入
    GPIO_InitStructure.GPIO_Pin = FLAME_AO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(FLAME_AO_PORT, &GPIO_InitStructure);

    // 4. ADC2 配置
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(FLAME_ADC, &ADC_InitStructure);

    // 5. 配置 ADC2 规则组通道（采样时间 55.5 周期）
    ADC_RegularChannelConfig(FLAME_ADC, FLAME_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);

    // 6. 使能 ADC2 并校准
    ADC_Cmd(FLAME_ADC, ENABLE);
    ADC_ResetCalibration(FLAME_ADC);
    while (ADC_GetResetCalibrationStatus(FLAME_ADC));
    ADC_StartCalibration(FLAME_ADC);
    while (ADC_GetCalibrationStatus(FLAME_ADC));
}
/*******************************************************************************
* Flame_Init_DO
* 描述  ：初始化火焰传感器的 GPIO，数字信号DO模块初始化
*         DO引脚接PA0
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Flame_Init_DO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(FLAME_DO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = FLAME_DO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
	GPIO_Init(FLAME_DO_PORT, &GPIO_InitStructure);
}
/*******************************************************************************
* Flame_Init_AO_ADC
* 描述  ：初始化火焰传感器的 ADC，模拟信号AO模块初始化
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Flame_Init_AO_ADC(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	// 1.使能 GPIO 和 ADC 时钟
	RCC_APB2PeriphClockCmd(FLAME_AO_RCC, ENABLE);

	// 2.配置 AO 引脚为模拟输入
	GPIO_InitStructure.GPIO_Pin = FLAME_AO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(FLAME_AO_PORT, &GPIO_InitStructure);

	// 3.ADC2 配置
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(FLAME_ADC, &ADC_InitStructure);
	// 4.配置 ADC2 规则组通道（采样时间 55.5 周期）
	ADC_RegularChannelConfig(FLAME_ADC, FLAME_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	// 5. 使能 ADC2 并校准
	ADC_Cmd(FLAME_ADC, ENABLE);
	ADC_ResetCalibration(FLAME_ADC);
	while (ADC_GetResetCalibrationStatus(FLAME_ADC));
	ADC_StartCalibration(FLAME_ADC);
	while (ADC_GetCalibrationStatus(FLAME_ADC));
}
/*******************************************************************************
* Flame_Init_AO
* 描述  ：初始化火焰传感器的 GPIO，模拟信号AO模块初始化
*         AO引脚接PB0
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Flame_Init_AO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// 2.配置 AO 引脚为模拟输入
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = FLAME_AO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(FLAME_AO_PORT, &GPIO_InitStructure);
}
/*******************************************************************************
* 函数名：Flame_GetDigital
* 描述  ：读取火焰传感器的数字量输出
* 输入  ：无
* 输出  ：0 = 无火焰，1 = 检测到火焰
* 注意  ：根据模块实际输出调整（通常检测到火焰时 DO 输出低电平）
*******************************************************************************/
uint8_t Flame_GetDigital(void)
{
	// 读取 DO 引脚电平，低电平表示有火焰（可根据需要取反）
	return (GPIO_ReadInputDataBit(FLAME_DO_PORT, FLAME_DO_PIN) == Bit_RESET) ? 1 : 0;
}

/*******************************************************************************
* 函数名：Flame_GetAnalog
* 描述  ：单次读取 ADC 值
* 输入  ：无
* 输出  ：12位 ADC 转换值（0-4095）
*******************************************************************************/
/*
uint16_t Flame_GetAnalog(void)
{
    // 确保规则组配置正确
    ADC_RegularChannelConfig(FLAME_ADC, FLAME_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);

    // 启动转换
    ADC_SoftwareStartConvCmd(FLAME_ADC, ENABLE);

    // 等待转换完成
    while (ADC_GetFlagStatus(FLAME_ADC, ADC_FLAG_EOC) == RESET);

    // 返回转换结果
    return ADC_GetConversionValue(FLAME_ADC);
		
		return ADC_GetAnalog(FLAME_ADC, FLAME_ADC_CHANNEL, ADC_SampleTime_55Cycles5);
}
*/
uint16_t Flame_GetAnalog(void)
{	
		return ADC_GetAnalog(FLAME_ADC, FLAME_ADC_CHANNEL, ADC_SampleTime_55Cycles5);
}

/*******************************************************************************
* 函数名：Flame_GetAnalogAverage
* 描述  ：多次采样取平均，减少波动
* 输入  ：无
* 输出  ：平均后的 ADC 值
*******************************************************************************/
uint16_t Flame_GetAnalogAverage(void)
{
    uint32_t sum = 0;
    uint8_t i;

    for (i = 0; i < FLAME_ADC_SAMPLES; i++) {
        sum += Flame_GetAnalog();
        Delay_us(100);  // 两次采样间隔 100us
    }

    return (uint16_t)(sum / FLAME_ADC_SAMPLES);
}

/*******************************************************************************
* 函数名：Flame_GetPercentage
* 描述  ：获取火焰强度百分比（0~100%）
* 输入  ：无
* 输出  ：百分比，值越大表示火焰越强
* 说明  ：ADC 值越小（电压越低）表示火焰越强，所以转换为 (1 - adc/4095) * 100
*******************************************************************************/
uint8_t Flame_GetPercentage(void)
{
    uint16_t adc_value = Flame_GetAnalogAverage();
    float percent = (1.0f - (float)adc_value / 4095.0f) * 100.0f;

    // 防止负值或超过100
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    return (uint8_t)percent;
}
