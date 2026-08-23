#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include "stm32f10x.h"

#define Dev_Att_Rep  0  // 订阅设备命令 topic（room/{id}/command）
#define Dev_Att_Acq  1  // 订阅应答 topic（room/{id}/reply）

_Bool MQTT_Client_DevLink(void);                 // 连接 MQTT（含 Last Will）
void MQTT_Client_SendData(void);                 // 上报传感器/执行器 JSON
void MQTT_Client_SendOnline(void);               // 上线声明
void MQTT_Client_Publish(const char *topic, const char *msg); // 通用发布
void MQTT_Client_Subscribe(u8 Sub);              // 按类型订阅
void MQTT_Client_RevPro(unsigned char *cmd);     // 处理下行命令
void MQTT_Client_FlushCmdAck(void);              // 命令处理后再发 ACK，避开 IPD 后立刻 CIPSEND

#endif
