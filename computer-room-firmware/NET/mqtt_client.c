// 单片机头文件
#include "stm32f10x.h"

// 网络设备
#include "esp8266.h"

// 协议文件
#include "mqtt_client.h"
#include "mqttkit.h"

// 硬件驱动
#include "usart.h"
#include "delay.h"

// C 库
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#include "actuators.h"

#define DEVICEID            "stm32_01"
#define MQTT_USERNAME       ""
#define MQTT_PASSWORD       ""
#define MQTT_KEEP_ALIVE     60
#define MQTT_WILL_MSG       "{\"dev\":\"stm32_01\",\"online\":0}"

extern unsigned char esp8266_buf[2048];

/*******************************************************************************
* 函数名：MQTT_Client_DevLink
* 描述  ：连接 MQTT 服务器（CONNECT 携带 Last Will：异常掉线代发 online:0）
* 输入  ：无
* 输出  ：0-成功，1-失败
*******************************************************************************/
_Bool MQTT_Client_DevLink(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};	// 协议包
	unsigned char *dataPtr;
	char will_topic[48];
	_Bool status = 1;

	UsartPrintf(USART_DEBUG,
		"Mqtt_Client_DevLink----USERNAME: %s,	PASSWORD: %s,	DEVICEID:%s	--- connecting....\r\n",
		MQTT_USERNAME, MQTT_PASSWORD, DEVICEID);
	sprintf(will_topic, "room/%s/status", DEVICEID);
	/* CONNECT Last Will：异常掉线由 broker 代发 online:0 */
	if (MQTT_PacketConnect(MQTT_USERNAME, MQTT_PASSWORD, DEVICEID, MQTT_KEEP_ALIVE, 1, MQTT_QOS_LEVEL0,
						will_topic, MQTT_WILL_MSG, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);	// 发送连接包

		dataPtr = ESP8266_GetIPD(250);	// 等待 CONNACK
		if (dataPtr != NULL)
		{
			if (MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch (MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0: UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n"); status = 0; break;
					case 1: UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n"); break;
					case 2: UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法的 clientId\r\n"); break;
					case 3: UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器不可用\r\n"); break;
					case 4: UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户名或密码错误\r\n"); break;
					case 5: UsartPrintf(USART_DEBUG, "WARN:	连接失败：未授权(如 token 非法)\r\n"); break;
					default: UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n"); break;
				}
			}
		}

		MQTT_DeleteBuffer(&mqttPacket);	// 释放组包缓冲
	}
	else
	{
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	}

	return status;
}

extern uint8_t humi;
extern uint8_t temp;
extern uint16_t gasPPM;
extern uint8_t gasDig;
extern uint8_t ldrDig;
extern uint8_t ldrPer;
extern uint8_t flameDig;
extern uint8_t flamePer;
extern uint8_t alarmFlag;
extern uint8_t fan;
extern uint8_t led;
extern volatile uint8_t report_asap;

/*******************************************************************************
* 函数名：MQTT_Client_FillBuf
* 描述  ：填充上报 JSON；始终包含 measureTime（校时失败则为 0000-00-00 00:00:00）
* 输入  ：buf-输出缓冲（建议不少于 320 字节）
* 输出  ：payload 字节长度
*******************************************************************************/
unsigned char MQTT_Client_FillBuf(char *buf)
{
	char measure_time[24];

	/* 始终带 measureTime，便于串口/云端统一观察；失败用占位时间 */
	if (ESP8266_GetTime(measure_time, sizeof(measure_time)) != 0)
	{
		snprintf(measure_time, sizeof(measure_time), "0000-00-00 00:00:00");
	}

	snprintf(buf, 320,
		"{\"dev\":\"" DEVICEID "\",\"temp\":%d,\"humi\":%d,\"gasPPM\":%d,\"gasDig\":%d,\"ldrDig\":%d,\"ldrPer\":%d,\"flameDig\":%d,\"flamePer\":%d,\"alarm\":%d,\"fan\":%d,\"led\":%d,\"measureTime\":\"%s\"}",
		temp, humi, gasPPM, gasDig, ldrDig, ldrPer, flameDig, flamePer, alarmFlag, fan, led, measure_time);
	return strlen(buf);
}

/*******************************************************************************
* 函数名：MQTT_Client_SendData
* 描述  ：向 room/{id}/report 周期上报传感器与执行器状态
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void MQTT_Client_SendData(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	char buf[320];
	char topic[64];
	short body_len = 0, i = 0;

	UsartPrintf(USART_DEBUG, "Tips:	MQTT_Client_SendData-MQTT\r\n");
	memset(buf, 0, sizeof(buf));
	sprintf(topic, "room/%s/report", DEVICEID);

	body_len = MQTT_Client_FillBuf(buf);
	UsartPrintf(USART_DEBUG, "topic: %s, payload: %s\r\n", topic, buf);
	if (body_len)
	{
		if (MQTT_PacketSaveData(topic, body_len, NULL, 5, &mqttPacket) == 0)	// 组 PUBLISH 包
		{
			for (; i < body_len; i++)
			{
				mqttPacket._data[mqttPacket._len++] = buf[i];
			}
			UsartPrintf(USART_DEBUG, "ESP8266_SendData\r\n");
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);	// 经 ESP8266 发出
			MQTT_DeleteBuffer(&mqttPacket);
		}
		else
		{
			UsartPrintf(USART_DEBUG, "WARN:EDP_NewBuffer Failed\r\n");
		}
	}
}

/*******************************************************************************
* 函数名：MQTT_Client_SendOnline
* 描述  ：上线声明（含设备信息），连上后调用一次；业务存活由 report 承担
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void MQTT_Client_SendOnline(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	char buf[160];
	char topic[48];
	short body_len = 0;

	UsartPrintf(USART_DEBUG, "Tips: MQTT_Client_SendOnline\r\n");
	sprintf(topic, "room/%s/status", DEVICEID);
	body_len = snprintf(buf, sizeof(buf),
		"{\"dev\":\"%s\",\"online\":1,\"product\":\"room\",\"name\":\"%s\"}",
		DEVICEID, DEVICEID);
	UsartPrintf(USART_DEBUG, "topic: %s, payload: %s\r\n", topic, buf);

	if (body_len > 0 && body_len < (short)sizeof(buf))
	{
		/* retain=1，覆盖遗嘱中可能残留的 online:0 */
		if (MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, buf, (uint32)body_len,
				MQTT_QOS_LEVEL0, 1, 1, &mqttPacket) == 0)
		{
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);
			MQTT_DeleteBuffer(&mqttPacket);
		}
		else
		{
			UsartPrintf(USART_DEBUG, "WARN: MQTT_PacketPublish Online Failed\r\n");
		}
	}
}

/*******************************************************************************
* 函数名：MQTT_Client_Publish
* 描述  ：发布任意 topic/消息
* 输入  ：topic-主题，msg-消息内容
* 输出  ：无
*******************************************************************************/
void MQTT_Client_Publish(const char *topic, const char *msg)
{
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};

	if (MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);
		MQTT_DeleteBuffer(&mqtt_packet);
	}
}

/*******************************************************************************
* 函数名：MQTT_Client_Subscribe
* 描述  ：按类型订阅服务端 topic
* 输入  ：Sub-Dev_Att_Rep 订阅 command；Dev_Att_Acq 订阅 reply
* 输出  ：无
*******************************************************************************/
void MQTT_Client_Subscribe(u8 Sub)
{
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};
	char topic_buf[56];
	const char *topic = topic_buf;

	switch (Sub)
	{
		case Dev_Att_Rep:
			snprintf(topic_buf, sizeof(topic_buf), "room/%s/command", DEVICEID);
			UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topic_buf);
			break;
		case Dev_Att_Acq:
			snprintf(topic_buf, sizeof(topic_buf), "room/%s/reply", DEVICEID);
			UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topic_buf);
			break;
		default:
			break;
	}

	if (MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, &topic, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);	// 发送订阅请求
		MQTT_DeleteBuffer(&mqtt_packet);
	}
}

/*******************************************************************************
* 函数名：MQTT_Client_RevPro
* 描述  ：处理 MQTT 下行：解析反控指令（fan/led）并回复
* 输入  ：cmd-ESP8266 收到的 MQTT 原始载荷指针
* 输出  ：无
*******************************************************************************/
void MQTT_Client_RevPro(unsigned char *cmd)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	unsigned short req_len = 0;
	unsigned char type = 0;
	short result = 0;

	type = MQTT_UnPacketRecv(cmd);

	switch (type)
	{
		case MQTT_PKT_CMD:	// 命令下发
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	// 解出 topic 与消息体

			if (result == 0)
			{
				UsartPrintf(USART_DEBUG, "cmdid_topic: %s; req_payload: %s; req_len: %d.\r\n",
					cmdid_topic, req_payload, req_len);

				char *dp = strchr(req_payload, ':');
				if (dp != NULL)
				{
					dp++;
					int val = 0;
					while (*dp >= '0' && *dp <= '9')
					{
						val = val * 10 + (*dp - '0');
						dp++;
					}
					if (strstr(req_payload, "fan"))
					{
						if (val == 0)
						{
							Relay_OFF();
							UsartPrintf(USART_DEBUG, "fan = %d, stop the relay\r\n", val);
						}
						else if (val == 1)
						{
							Relay_ON();
							UsartPrintf(USART_DEBUG, "fan = %d, start the relay\r\n", val);
						}
						else
						{
							Relay_Turn();
							UsartPrintf(USART_DEBUG, "fan = %d, turn the relay\r\n", val);
						}
					}
					else if (strstr(req_payload, "led"))
					{
						if (val == 0)
						{
							Led_OFF();
							UsartPrintf(USART_DEBUG, "led = %d, close the led\r\n", val);
						}
						else if (val == 1)
						{
							Led_ON();
							UsartPrintf(USART_DEBUG, "led = %d, open the led\r\n", val);
						}
						else
						{
							Led_Turn();
							UsartPrintf(USART_DEBUG, "led = %d, turn the led\r\n", val);
						}
					}
					report_asap = 1;	// 尽快上报最新执行器状态
				}

				if (MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	// 命令回复组包
				{
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);	// 回复平台
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
			break;

		case MQTT_PKT_PUBACK:	// PUBLISH 的 Ack
			if (MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			break;

		default:
			UsartPrintf(USART_DEBUG, "Tips:	default\r\n");
			result = -1;
			break;
	}

	ESP8266_Clear();	// 清空接收缓存

	if (type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}
