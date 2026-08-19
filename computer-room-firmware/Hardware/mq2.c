#include "mq2.h"
#include "delay.h"
#include "usart.h"
#include "adc.h"
#include <math.h>
// --- 定义常量（标定与浓度计算共用，勿再另写一套）---
#define RL_VALUE        (5.0f)      // 模块负载电阻 RL，单位 KΩ，请按实际电路修改
#define VCC_VALUE       (5.0f)      // 传感器回路电压 / ADC 参考电压
#define ADC_MAX_VALUE   (4095.0f)   // 12 位 ADC 满量程
#define CLEAN_AIR_RATIO (9.8f)      // 洁净空气中 Rs/R0 的比值 (MQ-2 通常在 9-10 之间)
#define CAL_PPM         (613.9f)    // 拟合公式中的常数 (用于 LPG/烟雾)
#define CAL_EXPONENT    (-2.074f)   // 拟合公式中的指数常数
		   

/*******************************************************************************
* MQ2_Init_AO_ADC
* 描述  ：初始化火焰传感器的 ADC，模拟信号AO模块初始化
* 输入  ：无
* 输出  ：无
*******************************************************************************/															   
void MQ2_Init_AO_ADC(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}
/*******************************************************************************
* MQ2_Init_AO
* 描述  ：初始化MQ2烟雾传感器的 GPIO，模拟信号AO模块初始化
*         AO引脚接PA1
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void MQ2_Init_AO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//PA1 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );
}		

/*******************************************************************************
* 函数名：MQ2_GetAnalog
* 描述  ：单次读取 ADC 值
* 输入  ：无
* 输出  ：12位 ADC 转换值（0-4095）
*******************************************************************************/
u16 MQ2_GetAnalog(u8 ch)
{	
	return ADC_GetAnalog(ADC1, ch, ADC_SampleTime_239Cycles5);
}
/*******************************************************************************
* 函数名：MQ2_GetAnalogAverage
* 描述  ：多次采样取平均，减少波动
* 输入  ：无
* 输出  ：平均后的 ADC 值
*******************************************************************************/
u16 MQ2_GetAnalogAverage(u8 ch,u8 times)
{
	u32 sum=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		sum+=MQ2_GetAnalog(ch);
		Delay_ms(5);
	}
	return sum/times;
}

float R0 = 2.49f;  // 洁净空气标定得到的传感器电阻基准，可由 MQ2_Calibrate 更新
/*******************************************************************************
* 函数名：MQ2_Calibrate
* 描述  ：获取MQ2的R0值
* 输入  ：无
* 输出  ：R0值
* 说明  ：每个MQ2的R0值不相同，换新MQ2或者长期未启动时，需要先调用一次此函数获取R0值并修正。
*******************************************************************************/
void MQ2_Calibrate(void) {
    unsigned int adc_value;
    float Vrl, Rs;

    UsartPrintf(USART_DEBUG,"Calibrating MQ-2, please keep in clean air...\n");
    Delay_ms(20000); // 等待预热 20 秒

    // 获取 ADC 平均值
    adc_value = MQ2_GetAnalogAverage(ADC_Channel_1, 10);

    // 1. 根据 ADC 值计算负载电阻 RL 上的电压 Vrl
    Vrl = (VCC_VALUE * (float)adc_value) / ADC_MAX_VALUE;
    if (Vrl < 0.001f) {
        UsartPrintf(USART_DEBUG, "Calibration failed: Vrl too low\n");
        return;
    }

    // 2. 根据分压公式计算传感器电阻 Rs
    //    Rs = ((Vc / Vrl) - 1.0f) * RL
    Rs = ((VCC_VALUE / Vrl) - 1.0f) * RL_VALUE;

    // 3. 洁净空气中 Rs / R0 = CLEAN_AIR_RATIO
    R0 = Rs / CLEAN_AIR_RATIO;

    UsartPrintf(USART_DEBUG,"Calibration done! R0 = %.2f KOhm\n", R0);
}

/*******************************************************************************
* 函数名：MQ2_GetGasPPM
* 描述  ：获取烟雾浓度PPM值
* 输入  ：无
* 输出  ：PPM，值越大表示烟雾越浓
* 说明  ：与 MQ2_Calibrate 使用同一套 RL_VALUE / VCC_VALUE / ADC_MAX_VALUE。
*******************************************************************************/
float MQ2_GetGasPPM(void) {
    // 1. 获取滤波后的 ADC 值
    int adc = MQ2_GetAnalogAverage(ADC_Channel_1, 10);

    // 2. 计算 VRL (负载电阻上的电压)
    float VRL = ((float)adc / ADC_MAX_VALUE) * VCC_VALUE;
    if (VRL < 0.001f) {
        return 10000.0f; // 接近短路/异常，按超量程处理
    }

    // 3. 计算当前传感器电阻 RS
    float RS = (VCC_VALUE - VRL) / VRL * RL_VALUE;

    // 4. 计算比值 Rs/R0
    float ratio = RS / R0;

    // 5. 通过幂函数计算浓度（LPG/可燃气体常用拟合）
    float ppm = CAL_PPM * powf(ratio, CAL_EXPONENT);

    if (ppm < 0) ppm = 0;
    if (ppm > 10000) ppm = 10000;

    return ppm;
}

/*******************************************************************************
* 函数名：MQ2_Init_DO
* 描述  ：初始化 MQ2 的数字输出引脚（DO）
*         DO引脚接PA4
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void MQ2_Init_DO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能 DO 引脚所在的 GPIO 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置 DO 引脚为输入模式
    // 注意：大多数模块的 DO 是开漏输出，内部已上拉，因此配置为浮空输入或上拉输入均可
    // 这里使用上拉输入，以确保电平稳定
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // 上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名：MQ2_GetDigital
* 描述  ：读取 MQ2 的数字输出状态
* 输入  ：无
* 输出  ：0 = 未检测到气体（浓度低于阈值），1 = 检测到气体（浓度高于阈值）
* 注意  ：根据模块实际输出电平调整。通常浓度超过阈值时 DO 输出低电平。
*******************************************************************************/
uint8_t MQ2_GetDigital(void)
{
    // 读取 DO 引脚电平，低电平表示检测到气体
    // 如果你的模块是高电平有效，请将 Bit_RESET 改为 Bit_SET
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == Bit_RESET)
        return 1;   // 检测到气体
    else
        return 0;   // 未检测到气体
}
