/*
 * @Author: zhmy
 * @Date: 2020-04-06 02:16:49
 * @LastEditTime: 2026-02-08 22:40:47
 * @LastEditors: zhmy
 * @Description: 蜂鸣器相关函数
 */

#include "beep.h"
#include "delay.h"
		    
//BEEP IO初始化

/*******************************************************************************
* 函数名：Beep_Init
* 描述  ：蜂鸣器初始化，接入引脚PB12。默认关闭。
*         适配高低电平触发继电器的状态初始化。需根据RELAY_ON_LEVEL配置高低电平继电器。
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Beep_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //BEEP-->PA.0 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.0
	// 初始化为关闭状态（根据有效电平设置）
#if BEEP_ON_LEVEL == 1
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);   // 高电平触发，初始化低电平，蜂鸣器关闭
#else
	GPIO_SetBits(GPIOB, GPIO_Pin_12);     // 低电平触发，初始化高电平，蜂鸣器关闭
#endif
}

/*******************************************************************************
* 函数名：Beep_On
* 描述  ：打开蜂鸣器。
*         适配高低电平触发蜂鸣器的开启。需根据BEEP_ON_LEVEL配置高低电平触发模式。
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Beep_On(void)
{
#if BEEP_ON_LEVEL == 1
    GPIO_SetBits(GPIOB, GPIO_Pin_12);     // 输出高电平
#else
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);   // 输出低电平
#endif
}

/*******************************************************************************
* 函数名：Beep_Off
* 描述  ：关闭蜂鸣器。
*         适配高低电平触发蜂鸣器的关闭。需根据BEEP_ON_LEVEL配置高低电平触发模式。
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Beep_Off(void)
{
#if BEEP_ON_LEVEL == 1
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);   // 输出低电平
#else
    GPIO_SetBits(GPIOB, GPIO_Pin_12);     // 输出高电平
#endif
}

/*******************************************************************************
* 函数名：Beep_Toggle
* 描述  ：翻转蜂鸣器状态（可用于产生短鸣效果）。
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Beep_Toggle(void)
{
    if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12) == Bit_SET)
        GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    else
        GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

/*******************************************************************************
* 函数名：Beep_ShortBeep
* 描述  ： 蜂鸣器短鸣。
* 输入  ：duration_ms 短鸣的持续毫秒数
* 输出  ：无
*******************************************************************************/
void Beep_ShortBeep(uint16_t duration_ms)
{
    Beep_On();              // 开启
    Delay_ms(duration_ms);  // 延时
    Beep_Off();             // 关闭
}

/*******************************************************************************
* 函数名：Relay_GetState
* 描述  ：获取蜂鸣器逻辑状态（1-响，0-不响）
*         适配高低电平触发蜂鸣器逻辑状态。根据RELAY_ON_LEVEL配置高低电平触发模式。
* 输入  ：无
* 输出  ：蜂鸣器逻辑状态（1-响，0-不响）
*******************************************************************************/
uint8_t Beep_GetState(void)
{
    // 读取当前输出引脚电平
    uint8_t pin_level = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12);

    // 根据有效电平宏转换为统一逻辑
#if BEEP_ON_LEVEL == 1
    return (pin_level == Bit_SET) ? 1 : 0;
#else
    return (pin_level == Bit_RESET) ? 1 : 0;
#endif
}
