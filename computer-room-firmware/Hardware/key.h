#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

// ========== 引脚定义（请根据实际连接修改）==========
// KEY1 原接 PB1，与 LDR AO（ADC1_IN9）冲突，改到 PB10（与 KEY2 同口相邻，USART3 未用）
#define KEY1_PORT       GPIOB
#define KEY1_PIN        GPIO_Pin_10
#define KEY1_RCC        RCC_APB2Periph_GPIOB

#define KEY2_PORT       GPIOB
#define KEY2_PIN        GPIO_Pin_11
#define KEY2_RCC        RCC_APB2Periph_GPIOB

#define KEY3_PORT       GPIOA
#define KEY3_PIN        GPIO_Pin_6
#define KEY3_RCC        RCC_APB2Periph_GPIOA

#define KEY4_PORT       GPIOA
#define KEY4_PIN        GPIO_Pin_7
#define KEY4_RCC        RCC_APB2Periph_GPIOA

#define KEY5_PORT       GPIOB
#define KEY5_PIN        GPIO_Pin_6
#define KEY5_RCC        RCC_APB2Periph_GPIOB

#define KEY6_PORT GPIOC
#define KEY6_PIN GPIO_Pin_13
#define KEY6_RCC RCC_APB2Periph_GPIOC

void Key_Init(void);       // 按键 GPIO 初始化（KEY1-PB10 / KEY2-PB11）
uint8_t Key_GetNum(void);  // Returns 1-6 for KEY1-KEY6; 0 when none is pressed.

#endif
