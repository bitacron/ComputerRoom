#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "key.h"

/*******************************************************************************
* 函数名：Key_Init
* 描述  ：按键初始化，PB1 / PB11 上拉输入
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名：Key_GetNum
* 描述  ：检测按键（低电平有效，带消抖）；未按下返回 0
* 输入  ：无
* 输出  ：1-按键1(PB1)，2-按键2(PB11)，0-无按键
*******************************************************************************/
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0);
		Delay_ms(20);
		KeyNum = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0);
		Delay_ms(20);
		KeyNum = 2;
	}

	return KeyNum;
}
