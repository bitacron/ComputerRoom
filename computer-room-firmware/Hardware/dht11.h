#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"                  // Device header

void DHT11_GPIO_OUT(void);                          // DATA 脚配置为推挽输出
void DHT11_GPIO_IN(void);                           // DATA 脚配置为浮空输入
uint8_t DHT11_ReadByte(void);                       // 读取 1 字节
void DHT11_ReadData(uint8_t *temp, uint8_t *humi);  // 读取温湿度（校验失败则不更新）

#endif
