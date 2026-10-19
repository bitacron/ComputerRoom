#ifndef __ACTUATORS_H
#define __ACTUATORS_H

#include "stm32f10x.h"

/* 
Relay input active level: 1 = high, 0 = low.
根据硬件实际接线定义继电器有效电平（1：高电平触发；0：低电平触发）
*/
#define LED_RELAY_ON_LEVEL                  1
#define COOLING_FAN_RELAY_ON_LEVEL          0
#define HEATER_RELAY_ON_LEVEL               0
#define HUMIDIFIER_RELAY_ON_LEVEL           0
#define DEHUMIDIFYING_FAN_RELAY_ON_LEVEL    0

void Actuators_Init(void);

void Led_Init(void);
void Led_ON(void);
void Led_OFF(void);
void Led_Turn(void);
uint8_t Led_GetState(void);

void CoolingFan_Init(void);
void CoolingFan_ON(void);
void CoolingFan_OFF(void);
void CoolingFan_Turn(void);
uint8_t CoolingFan_GetState(void);

void Heater_Init(void);
void Heater_ON(void);
void Heater_OFF(void);
void Heater_Turn(void);
uint8_t Heater_GetState(void);

void Humidifier_Init(void);
void Humidifier_ON(void);
void Humidifier_OFF(void);
void Humidifier_Turn(void);
uint8_t Humidifier_GetState(void);

void DehumidifyingFan_Init(void);
void DehumidifyingFan_ON(void);
void DehumidifyingFan_OFF(void);
void DehumidifyingFan_Turn(void);
uint8_t DehumidifyingFan_GetState(void);

#endif
