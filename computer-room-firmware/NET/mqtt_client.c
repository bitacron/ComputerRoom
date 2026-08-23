// ?????????
#include "stm32f10x.h"

// ???????
#include "esp8266.h"

// ???????
#include "mqtt_client.h"
#include "mqttkit.h"

// ???????
#include "usart.h"
#include "delay.h"

// C ??
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
* ????????MQTT_Client_DevLink
* ????  ?????? MQTT ????????CONNECT ???? Last Will??????????? online:0??
* ????  ????
* ???  ??0-?????1-???
*******************************************************************************/
_Bool MQTT_Client_DevLink(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};	// ?????
	unsigned char *dataPtr;
	char will_topic[48];
	_Bool status = 1;

	UsartPrintf(USART_DEBUG,
		"Mqtt_Client_DevLink----USERNAME: %s,	PASSWORD: %s,	DEVICEID:%s	--- connecting....\r\n",
		MQTT_USERNAME, MQTT_PASSWORD, DEVICEID);
	sprintf(will_topic, "room/%s/status", DEVICEID);
	/* CONNECT Last Will?????????? broker ???? online:0 */
	if (MQTT_PacketConnect(MQTT_USERNAME, MQTT_PASSWORD, DEVICEID, MQTT_KEEP_ALIVE, 1, MQTT_QOS_LEVEL0,
						will_topic, MQTT_WILL_MSG, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);	// ?????????

		dataPtr = ESP8266_GetIPD(250);	// ??? CONNACK
		if (dataPtr != NULL)
		{
			if (MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch (MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0: UsartPrintf(USART_DEBUG, "Tips:	??????\r\n"); status = 0; break;
					case 1: UsartPrintf(USART_DEBUG, "WARN:	???????????????\r\n"); break;
					case 2: UsartPrintf(USART_DEBUG, "WARN:	????????????? clientId\r\n"); break;
					case 3: UsartPrintf(USART_DEBUG, "WARN:	????????????????????\r\n"); break;
					case 4: UsartPrintf(USART_DEBUG, "WARN:	??????????????????????\r\n"); break;
					case 5: UsartPrintf(USART_DEBUG, "WARN:	?????????????(?? token ???)\r\n"); break;
					default: UsartPrintf(USART_DEBUG, "ERR:	???????????????\r\n"); break;
				}
			}
		}

		MQTT_DeleteBuffer(&mqttPacket);	// ??????????
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

static volatile uint8_t cmd_ack_pending = 0;
static char pending_ack_cmd_id[24];
static char pending_ack_act[8];
static int pending_ack_val;
static int pending_ack_result;

/*******************************************************************************
* ????????MQTT_Client_FillBuf
* ????  ???????? JSON???????? measureTime??????????? 0000-00-00 00:00:00??
* ????  ??buf-????????????????? 320 ????
* ???  ??payload ??????
*******************************************************************************/
unsigned char MQTT_Client_FillBuf(char *buf)
{
	char measure_time[24];

	/* ???? measureTime?????????/??????????????????? */
	if (ESP8266_GetTime(measure_time, sizeof(measure_time)) != 0)
	{
		snprintf(measure_time, sizeof(measure_time), "0000-00-00 00:00:00");
	}

	fan = Relay_GetState();
	led = Led_GetState();
	snprintf(buf, 320,
		"{\"dev\":\"" DEVICEID "\",\"temp\":%d,\"humi\":%d,\"gasPPM\":%d,\"gasDig\":%d,\"ldrDig\":%d,\"ldrPer\":%d,\"flameDig\":%d,\"flamePer\":%d,\"alarm\":%d,\"fan\":%d,\"led\":%d,\"measureTime\":\"%s\"}",
		temp, humi, gasPPM, gasDig, ldrDig, ldrPer, flameDig, flamePer, alarmFlag, fan, led, measure_time);
	return strlen(buf);
}

/*******************************************************************************
* ????????MQTT_Client_SendData
* ????  ???? room/{id}/report ??????????????????????
* ????  ????
* ???  ????
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
		if (MQTT_PacketSaveData(topic, body_len, NULL, 5, &mqttPacket) == 0)	// ?? PUBLISH ??
		{
			for (; i < body_len; i++)
			{
				mqttPacket._data[mqttPacket._len++] = buf[i];
			}
			UsartPrintf(USART_DEBUG, "ESP8266_SendData\r\n");
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);	// ?? ESP8266 ????
			MQTT_DeleteBuffer(&mqttPacket);
		}
		else
		{
			UsartPrintf(USART_DEBUG, "WARN:EDP_NewBuffer Failed\r\n");
		}
	}
}

/*******************************************************************************
* ????????MQTT_Client_SendOnline
* ????  ???????????????????????????????????????????? report ????
* ????  ????
* ???  ????
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
		/* retain=1????????????????????? online:0 */
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
* ????????MQTT_Client_Publish
* ????  ?????????? topic/???
* ????  ??topic-????msg-???????
* ???  ????
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
* ????????MQTT_Client_Subscribe
* ????  ??????????????? topic
* ????  ??Sub-Dev_Att_Rep ???? command??Dev_Att_Acq ???? reply
* ???  ????
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
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);	// ???????????
		MQTT_DeleteBuffer(&mqtt_packet);
	}
}

/*******************************************************************************
* ????????MQTT_Client_RevPro
* ????  ?????? MQTT ?????????????????fan/led???????
* ????  ??cmd-ESP8266 ????? MQTT ????????
* ???  ????
*******************************************************************************/
static void MQTT_Client_SendCmdAck(const char *cmd_id, const char *act, int val, int result)
{
	char topic[48];
	char payload[192];
	int n;

	fan = Relay_GetState();
	led = Led_GetState();
	sprintf(topic, "room/%s/ack", DEVICEID);
	n = snprintf(payload, sizeof(payload),
		"{\"cmdId\":\"%s\",\"dev\":\"%s\",\"act\":\"%s\",\"val\":%d,\"result\":%d,\"fan\":%d,\"led\":%d}",
		cmd_id ? cmd_id : "", DEVICEID, act ? act : "", val, result, fan, led);
	if (n > 0 && n < (int)sizeof(payload))
	{
		UsartPrintf(USART_DEBUG, "ack topic: %s, payload: %s\r\n", topic, payload);
		MQTT_Client_Publish(topic, payload);
	}
}

void MQTT_Client_FlushCmdAck(void)
{
	if (!cmd_ack_pending)
	{
		return;
	}
	cmd_ack_pending = 0;
	Delay_ms(50);
	MQTT_Client_SendCmdAck(pending_ack_cmd_id, pending_ack_act, pending_ack_val, pending_ack_result);
}

static void MQTT_Client_QueueCmdAck(const char *cmd_id, const char *act, int val, int result)
{
	memset(pending_ack_cmd_id, 0, sizeof(pending_ack_cmd_id));
	memset(pending_ack_act, 0, sizeof(pending_ack_act));
	if (cmd_id != NULL)
	{
		strncpy(pending_ack_cmd_id, cmd_id, sizeof(pending_ack_cmd_id) - 1);
	}
	if (act != NULL)
	{
		strncpy(pending_ack_act, act, sizeof(pending_ack_act) - 1);
	}
	pending_ack_val = val;
	pending_ack_result = result;
	cmd_ack_pending = 1;
}

void MQTT_Client_RevPro(unsigned char *cmd)
{
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	unsigned short req_len = 0;
	unsigned char type = 0;
	short result = 0;

	type = MQTT_UnPacketRecv(cmd);

	switch (type)
	{
		case MQTT_PKT_CMD:	// ????????
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	// ??? topic ???????

			if (result == 0)
			{
				char payload_buf[256];
				cJSON *root = NULL;
				const char *act = "";
				const char *cmd_id = "";
				int val = -1;
				int exec_result = 2;

				if (req_len >= sizeof(payload_buf))
				{
					req_len = sizeof(payload_buf) - 1;
				}
				memcpy(payload_buf, req_payload, req_len);
				payload_buf[req_len] = '\0';
				UsartPrintf(USART_DEBUG, "cmdid_topic: %s; req_payload: %s; req_len: %d.\r\n",
					cmdid_topic, payload_buf, req_len);

				root = cJSON_Parse(payload_buf);
				if (root != NULL)
				{
					cJSON *act_item = cJSON_GetObjectItem(root, "act");
					cJSON *val_item = cJSON_GetObjectItem(root, "val");
					cJSON *id_item = cJSON_GetObjectItem(root, "cmdId");

					if (act_item != NULL && act_item->valuestring != NULL)
					{
						act = act_item->valuestring;
					}
					if (id_item != NULL && id_item->valuestring != NULL)
					{
						cmd_id = id_item->valuestring;
					}
					if (val_item != NULL)
					{
						val = val_item->valueint;
					}

					if (act[0] == '\0')
					{
						exec_result = 1;
					}
					else if (val != 0 && val != 1)
					{
						exec_result = 2;
					}
					else if (strcmp(act, "fan") == 0)
					{
						if (val == 1)
						{
							Relay_ON();
						}
						else
						{
							Relay_OFF();
						}
						fan = Relay_GetState();
						exec_result = (fan == val) ? 0 : 3;
						UsartPrintf(USART_DEBUG, "fan val=%d gpio=%d\r\n", val, fan);
					}
					else if (strcmp(act, "led") == 0)
					{
						if (val == 1)
						{
							Led_ON();
						}
						else
						{
							Led_OFF();
						}
						led = Led_GetState();
						exec_result = (led == val) ? 0 : 3;
						UsartPrintf(USART_DEBUG, "led val=%d gpio=%d\r\n", val, led);
					}
					else
					{
						exec_result = 1;
					}

					MQTT_Client_QueueCmdAck(cmd_id, act, val < 0 ? 0 : val, exec_result);
					cJSON_Delete(root);
					if (exec_result == 0)
					{
						report_asap = 1;
					}
				}
				else
				{
					UsartPrintf(USART_DEBUG, "WARN: command JSON parse failed\r\n");
					MQTT_Client_QueueCmdAck("", "", 0, 2);
				}
			}
			break;

		case MQTT_PKT_PUBACK:	// PUBLISH ?? Ack
			if (MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			break;

		default:
			UsartPrintf(USART_DEBUG, "Tips:	default\r\n");
			result = -1;
			break;
	}

	ESP8266_Clear();	// ?????????

	if (type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}
