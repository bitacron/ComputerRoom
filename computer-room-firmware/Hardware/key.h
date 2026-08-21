#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

void Key_Init(void);       // 按键 GPIO 初始化（PB1/PB11）
uint8_t Key_GetNum(void);  // 读键值：1/2，无键返回 0

#endif
