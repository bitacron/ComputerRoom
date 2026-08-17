#ifndef __MQ2_H
#define __MQ2_H	
#include "sys.h"
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//ADC 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

void MQ2_Init_AO_ADC(void);		// 初始化模拟输入引脚（AO）及 ADC
void MQ2_Init_AO(void); 			// 初始化模拟输入引脚（AO）
u16 MQ2_GetAnalog(u8 ch);
u16 MQ2_GetAnalogAverage(u8 ch,u8 times);
float MQ2_GetGasPPM(void);
void MQ2_Calibrate(void);

void MQ2_Init_DO(void);				// 初始化数字输入引脚（DO）
uint8_t MQ2_GetDigital(void); // 读取数字输出状态（0/1）
#endif 
