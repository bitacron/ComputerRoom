#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include "stm32f10x.h"

#define	Dev_Att_Rep  0 //设备属性上报
#define Dev_Att_Acq  1  //设备属性获取

_Bool MQTT_Client_DevLink(void);
void MQTT_Client_SendData(void);
void MQTT_Client_SendHeartbeat(void);
void MQTT_Client_RevPro(unsigned char *cmd);

//void MQTT_Client_Subscribe(const char *topics[], unsigned char topic_cnt);
//void MQTT_Client_Publish(const char *topic, const char *msg);
void MQTT_Client_Subscribe(u8 Sub);
#endif
