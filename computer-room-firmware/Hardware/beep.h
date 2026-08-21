
#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"

// 根据硬件实际接线定义有效电平（1：高电平触发；0：低电平触发）
#define BEEP_ON_LEVEL  0   // 当前为低电平有效（响）；若高电平响则改为 1

#define BEEP   PBout(12)   // 保留直接操作方式，但推荐使用下方函数

void Beep_Init(void);      // 初始化
void Beep_On(void);        // 打开蜂鸣器
void Beep_Off(void);       // 关闭蜂鸣器
void Beep_Toggle(void);    // 翻转蜂鸣器状态（用于提示音等）
void Beep_ShortBeep(uint16_t duration_ms); // 蜂鸣器短鸣
uint8_t Beep_GetState(void);   // 蜂鸣器报警状态：返回 1-响，0-不响
#endif
