#include "stm32f10x.h"
#include "adc.h"
#include "light_dependent_resisto.h"
#include "delay.h"

/*******************************************************************************
* 函数名：LDR_Init_DO
* 描述  ：初始化光敏电阻数字输出（DO）引脚，配置为带上拉输入模式。
*         该引脚通常连接比较器输出，用于直接检测光照是否低于阈值。
*         DO引脚接PB13。
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void LDR_Init_DO(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名：LDR_GetDigital
* 描述  ：读取光敏电阻数字输出引脚的电平状态。
* 返回值：0 表示光照低于阈值（低电平），1 表示光照高于阈值（高电平）。
* 输入  ：无
* 输出  ：uint8_t 数字电平值
*******************************************************************************/
uint8_t LDR_GetDigital(void)
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
}

/*******************************************************************************
* 函数名：LDR_Init_AO
* 描述  ：初始化光敏电阻模拟输出（AO）引脚，配置为模拟输入模式。
*         该引脚直接连接光敏电阻分压电路，用于 ADC 采样。
*         AO引脚接PB1（ADC1_IN9）。
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void LDR_Init_AO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名：LDR_GetAnalog
* 描述  ：单次读取光敏电阻模拟输出引脚的 ADC 转换值。
*         使用预定义的 ADC 外设、通道和采样时间进行转换。
* 输入  ：无
* 输出  ：12位 ADC 值（范围 0~4095）
*******************************************************************************/
uint16_t LDR_GetAnalog(void)
{
    return ADC_GetAnalog(LDR_ADC, LDR_CHANNEL, LDR_SAMPLE_TIME);
}

/*******************************************************************************
* 函数名：LDR_GetAnalogAverage
* 描述  ：多次采样光敏电阻模拟值并取平均，以减小采样波动。
*         采样次数由宏 LDRE_ADC_SAMPLES 定义，两次采样间隔 100us。
* 输入  ：无
* 输出  ：平均后的 ADC 值（12位，范围 0~4095）
*******************************************************************************/
uint16_t LDR_GetAnalogAverage(void)
{
    uint32_t sum = 0;
    uint8_t i;

    for (i = 0; i < LDRE_ADC_SAMPLES; i++) {
        sum += LDR_GetAnalog();
        Delay_us(100);  // 两次采样间隔 100us
    }

    return (uint16_t)(sum / LDRE_ADC_SAMPLES);
}

/*******************************************************************************
* 函数名：LDR_GetPercentage
* 描述  ：根据平均 ADC 值计算当前光照强度百分比。
*         光照越强，返回值越接近 100%；光照越弱，返回值越接近 0%。
*         注：ADC 值假设为 12 位（0~4095），暗场最小值可能不为 0，但算法已做限幅处理。
* 输入  ：无
* 输出  ：光照强度百分比（0~100）
*******************************************************************************/
uint8_t LDR_GetPercentage(void)
{
    uint16_t adc = LDR_GetAnalogAverage();   // 获取平均 ADC 值
    // 假设 ADC 最大值 4095（12位），最小值 0（完全黑暗时可能不是 0，可实测调整）
    uint16_t percent = (uint16_t)((4095UL - adc) * 100UL / 4095UL);
    if (percent > 100) percent = 100;
    return (uint8_t)percent;
}
