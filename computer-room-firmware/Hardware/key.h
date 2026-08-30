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

void Key_Init(void);       // 按键 GPIO 初始化（KEY1-PB10 / KEY2-PB11）
uint8_t Key_GetNum(void);  // 读键值：1/2，无键返回 0

#endif
