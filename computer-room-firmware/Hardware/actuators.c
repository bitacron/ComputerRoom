#include "stm32f10x.h"
#include "actuators.h"

/*******************************************************************************
* 函数名：Actuators_Init
* 描述  ：统一初始化执行器：依次初始化 LED 与继电器
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Actuators_Init(void)
{
    Led_Init();
    Relay_Init();
}

/*******************************************************************************
* 函数名：Led_Init
* 描述  ：初始化Led接入引脚PB5。
*         默认低电平，关闭。
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Led_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能 GPIOB 时钟

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_5); // 默认熄灭（低电平）
}
/*******************************************************************************
* 函数名：Led_ON
* 描述  ：开灯
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Led_ON(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
}
/*******************************************************************************
* 函数名：Led_OFF
* 描述  ：关灯
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Led_OFF(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}

/*******************************************************************************
* 函数名：Led_Turn
* 描述  ：切换灯的开关状态
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Led_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_5);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	}
}

/*******************************************************************************
* 函数名：Led_GetState
* 描述  ：获取灯的开关状态
* 输入  ：无
* 输出  ：无灯的开关状态
*******************************************************************************/
uint8_t Led_GetState(void)
{
    uint8_t pin_level = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5); // 读取输出寄存器
    return pin_level;
}

/*******************************************************************************
* 函数名：Relay_Init
* 描述  ：继电器初始化PA12。初始状态为关闭。
*         适配高低电平触发继电器的状态初始化。需根据RELAY_ON_LEVEL配置高低电平继电器。
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Relay_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能 GPIOA 时钟

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 根据有效电平设置初始关闭状态
#if RELAY_ON_LEVEL == 1
    GPIO_ResetBits(GPIOA, GPIO_Pin_12);   // 高电平有效，关闭需拉低
#else
    GPIO_SetBits(GPIOA, GPIO_Pin_12);     // 低电平有效，关闭需拉高
#endif
}

/*******************************************************************************
* 函数名：Relay_ON
* 描述  ：继电器开启。
*         适配高低电平触发继电器开启。需根据RELAY_ON_LEVEL配置高低电触发模式。
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Relay_ON(void)
{
#if RELAY_ON_LEVEL == 1
    GPIO_SetBits(GPIOA, GPIO_Pin_12);     // 高电平吸合
#else
    GPIO_ResetBits(GPIOA, GPIO_Pin_12);   // 低电平吸合
#endif
}

/*******************************************************************************
* 函数名：Relay_OFF
* 描述  ：继电器关闭。
*         适配高低电平触发继电器关闭。需根据RELAY_ON_LEVEL配置高低电平触发模式。
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Relay_OFF(void)
{
#if RELAY_ON_LEVEL == 1
    GPIO_ResetBits(GPIOA, GPIO_Pin_12);   // 高电平有效，关闭需低电平
#else
    GPIO_SetBits(GPIOA, GPIO_Pin_12);     // 低电平有效，关闭需高电平
#endif
}

/*******************************************************************************
* 函数名：Relay_Turn
* 描述  ：继电器状态转换。
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Relay_Turn(void)
{
    // 根据当前逻辑状态翻转
    if (Relay_GetState() == 1)
        Relay_OFF();
    else
        Relay_ON();
}


/*******************************************************************************
* 函数名：Relay_GetState
* 描述  ：获取继电器逻辑状态
*         适配高低电平触发继电器逻辑状态。根据RELAY_ON_LEVEL配置高低电平触发模式。
* 输入  ：无
* 输出  ：继电器逻辑状态（1-吸合，0-释放）
*******************************************************************************/
uint8_t Relay_GetState(void)
{
    uint8_t pin_level = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_12); // 读取输出寄存器

#if RELAY_ON_LEVEL == 1
    return (pin_level == Bit_SET) ? 1 : 0;   // 高电平表示吸合
#else
    return (pin_level == Bit_RESET) ? 1 : 0; // 低电平表示吸合
#endif
}
