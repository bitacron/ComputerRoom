#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "key.h"

/*******************************************************************************
* 函数名：Key_Init
* 描述  ：按键初始化，KEY1(PB10) / KEY2(PB11) / KEY3(PA6) / KEY4(PA7) / KEY5(PB6) 上拉输入
*         低电平有效：按键一端接对应 GPIO，另一端接 GND
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(KEY1_RCC | KEY2_RCC | KEY3_RCC | KEY4_RCC | KEY5_RCC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN | KEY5_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = KEY3_PIN | KEY4_PIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名：Key_GetNum
* 描述  ：检测按键（低电平有效，带消抖）；未按下返回 0
* 输入  ：无
* 输出  ：1-KEY1(PB10)，2-KEY2(PB11)，0-无按键
*******************************************************************************/
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == 0);
		Delay_ms(20);
		KeyNum = 1;
	}
	if (GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == 0);
		Delay_ms(20);
		KeyNum = 2;
	}

	if (GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN) == 0);
		Delay_ms(20);
		KeyNum = 3;
	}
	if (GPIO_ReadInputDataBit(KEY4_PORT, KEY4_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY4_PORT, KEY4_PIN) == 0);
		Delay_ms(20);
		KeyNum = 4;
	}
	if (GPIO_ReadInputDataBit(KEY5_PORT, KEY5_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY5_PORT, KEY5_PIN) == 0);
		Delay_ms(20);
		KeyNum = 5;
	}

	return KeyNum;
}
