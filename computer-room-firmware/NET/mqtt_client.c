//?????????
#include "stm32f10x.h"

//??????
#include "esp8266.h"

//??????
#include "mqtt_client.h"
#include "mqttkit.h"

//???????
#include "usart.h"
#include "delay.h"

//C??
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
* MQTT_Client_DevLink
* ????  ????MQTT??????????????
* ????  ????
* ???  ????
*******************************************************************************/		
_Bool MQTT_Client_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//????

	unsigned char *dataPtr;
	char will_topic[48];
	_Bool status = 1;
	
	UsartPrintf(USART_DEBUG, "Mqtt_Client_DevLink----USERNAME: %s,	PASSWORD: %s,	DEVICEID:%s	--- connecting....\r\n"
                        , MQTT_USERNAME, MQTT_PASSWORD, DEVICEID);
	sprintf(will_topic, "room/%s/status", DEVICEID);
	/* CONNECT Last Will: broker publishes online:0 on abnormal disconnect */
	if(MQTT_PacketConnect(MQTT_USERNAME, MQTT_PASSWORD, DEVICEID, MQTT_KEEP_ALIVE, 1, MQTT_QOS_LEVEL0,
						will_topic, MQTT_WILL_MSG, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//?????
		
		dataPtr = ESP8266_GetIPD(250);									//????????
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	??????\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	??????????????\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	?????????????clientid\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	?????????????????\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	??????????????????????\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	???????????????(????token???)\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	??????????????\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//???
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
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
unsigned char MQTT_Client_FillBuf(char *buf)
{
  snprintf(buf, 256,
			"{\"dev\":\"" DEVICEID "\",\"temp\":%d,\"humi\":%d,\"gasPPM\":%d,\"gasDig\":%d,\"ldrDig\":%d,\"ldrPer\":%d,\"flameDig\":%d,\"flamePer\":%d,\"alarm\":%d,\"fan\":%d,\"led\":%d}",
			temp, humi, gasPPM, gasDig, ldrDig, ldrPer, flameDig, flamePer, alarmFlag, fan, led);
  return strlen(buf);
}

/*******************************************************************************
* MQTT_Client_SendData
* ????  ????????????MQTT??????
* ????  ????
* ???  ????
*******************************************************************************/	
void MQTT_Client_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//????
	
	char buf[256];
  char topic[64]; // MQTT topic

	short body_len = 0, i = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	MQTT_Client_SendData-MQTT\r\n");
	memset(buf, 0, sizeof(buf));//???????????
	sprintf(topic, "room/%s/report", DEVICEID);
	
	body_len = MQTT_Client_FillBuf(buf);	//???????????????????????????
	UsartPrintf(USART_DEBUG,"topic: %s, payload: %s\r\n",topic, buf);
	if(body_len)
	{		
		if(MQTT_PacketSaveData(topic, body_len, NULL, 5, &mqttPacket) == 0)							//???
		{
			for(; i < body_len; i++){
				mqttPacket._data[mqttPacket._len++] = buf[i];
			}
			UsartPrintf(USART_DEBUG, "ESP8266_SendData\r\n");
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//??????????
			MQTT_DeleteBuffer(&mqttPacket);															//???
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:EDP_NewBuffer Failed\r\n");
	}
	
}

/**
 * @brief device online/birth message (once after connect). Liveness via report.
 */
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
		/* retain=1 to overwrite retained will online:0 */
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
* MQTT_Client_Publish
* ????  ?????????
* ????  ??topic????????????
*         msg?????????
* ???  ????
*******************************************************************************/	
void MQTT_Client_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//????
	
	//UsartPrintf(USART_DEBUG, "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//???????????????
		
		MQTT_DeleteBuffer(&mqtt_packet);										//???
	}

}

/*******************************************************************************
* MQTT_Client_Subscribe
* ????  ??????MQTT????????topic
* ????  ???????????Dev_Att_Rep: ?????????; Dev_Att_Acq: ??????????
* ???  ????
*******************************************************************************/	
void MQTT_Client_Subscribe(u8 Sub)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//????
	
	char topic_buf[56];
	const char *topic = topic_buf;
	switch(Sub)
	{
		case Dev_Att_Rep:snprintf(topic_buf, sizeof(topic_buf), "room/%s/command", DEVICEID);
		     UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topic_buf);
					break;
		case Dev_Att_Acq:snprintf(topic_buf, sizeof(topic_buf), "room/%s/reply", DEVICEID);
		     UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topic_buf);
		     break;
		default:break;
	}

	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, &topic, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//???????????????
		
		MQTT_DeleteBuffer(&mqtt_packet);										//???
	}

}

/*******************************************************************************
* MQTT_Client_RevPro
* ????  ??MQTT????????????????
* ????  ??dataPtr?????????????
* ???  ????
*******************************************************************************/	
void MQTT_Client_RevPro(unsigned char *cmd)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//????
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	
	short result = 0;

	type = MQTT_UnPacketRecv(cmd);

	switch(type)
	{
		case MQTT_PKT_CMD:															//?????·?
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//???topic???????

			if(result == 0)
			{
				UsartPrintf(USART_DEBUG,"cmdid_topic: %s; req_payload: %s; req_len: %d.\r\n", cmdid_topic, req_payload, req_len);
				
				char *dp = strchr(req_payload, ':');
				if(dp != NULL) {
						dp++;
						int val = 0;
						while(*dp >= '0' && *dp <= '9') {
								val = val*10 + (*dp - '0');
								dp++;
						}
						if(strstr(req_payload, "fan")) {
							if(val == 0){
								Relay_OFF();
								UsartPrintf(USART_DEBUG, "fan = %d, stop the relay\r\n", val);
							}else if(val == 1){
								Relay_ON();
								UsartPrintf(USART_DEBUG, "fan = %d, start the relay\r\n", val);
							} else {
								Relay_Turn();
								UsartPrintf(USART_DEBUG, "fan = %d, turn the relay\r\n", val);
							}
						} else if(strstr(req_payload, "led")) {
							if(val == 0){
								Led_OFF();
								UsartPrintf(USART_DEBUG, "led = %d, close the led\r\n", val);
							}else if(val == 1){
								Led_ON();
								UsartPrintf(USART_DEBUG, "led = %d, open the led\r\n", val);
							} else {
								Led_Turn();
								UsartPrintf(USART_DEBUG, "led = %d, turn the led\r\n", val);
							}
						}
						report_asap = 1;
				}
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//?????????
				{
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//???????
					MQTT_DeleteBuffer(&mqttPacket);									//???
				}
			}
		break;
			
		case MQTT_PKT_PUBACK:														//????Publish????????????Ack
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
		break;
		
		default:
			UsartPrintf(USART_DEBUG,"Tips:	default\r\n");
			result = -1;
		break;
	}

	ESP8266_Clear();									//??????

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
