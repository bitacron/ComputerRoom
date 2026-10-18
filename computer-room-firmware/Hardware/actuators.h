#ifndef __ACTUATORS_H
#define __ACTUATORS_H

// 根据硬件实际接线定义继电器有效电平（1：高电平触发；0：低电平触发）
#define LED_RELAY_ON_LEVEL           1   // High level active; default for direct LED
#define COOLING_FAN_RELAY_ON_LEVEL  0   // 假设当前代码为低电平触发（CoolingFan_ON拉低），若高电平触发则改为1

void Actuators_Init(void);
void Led_Init(void);        // 单独初始化 LED1
void Led_ON(void);
void Led_OFF(void);
void Led_Turn(void);
uint8_t Led_GetState(void);
void CoolingFan_Init(void);       // 单独初始化 继电器
void CoolingFan_ON(void);
void CoolingFan_OFF(void);
void CoolingFan_Turn(void);
uint8_t CoolingFan_GetState(void);
#endif
