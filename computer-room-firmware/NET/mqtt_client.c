//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "mqtt_client.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"

//C库
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#include "actuators.h"

#define DEVICEID      "stm32_01"  // 设备ID
#define MQTT_USERNAME       ""  // MQTT服务用户名
#define MQTT_PASSWORD       ""   // MQTT服务密码
#define MQTT_KEEP_ALIVE     60 // 最大连接时间

extern unsigned char esp8266_buf[2048];

/*******************************************************************************
* MQTT_Client_DevLink
* 描述  ：与MQTT服务器创建连接
* 输入  ：无
* 输出  ：无
*******************************************************************************/		
_Bool MQTT_Client_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	UsartPrintf(USART_DEBUG, "Mqtt_Client_DevLink----USERNAME: %s,	PASSWORD: %s,	DEVICEID:%s	--- 连接中....\r\n"
                        , MQTT_USERNAME, MQTT_PASSWORD, DEVICEID);
  if(MQTT_PacketConnect(MQTT_USERNAME, MQTT_PASSWORD, DEVICEID, MQTT_KEEP_ALIVE, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)	
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法的clientid\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}

extern uint8_t humi;
extern uint8_t temp;
extern uint8_t gasPPM;
extern uint8_t gasDig;
extern uint8_t ldrDig;
extern uint8_t ldrPer;
extern uint8_t flameDig;
extern uint8_t flamePer;
extern uint8_t alarmFlag;
extern uint8_t fan;
extern uint8_t led;
extern unsigned short timeCount;
unsigned char MQTT_Client_FillBuf(char *buf)
{
  snprintf(buf, 256,
			"{\"dev\":\"" DEVICEID "\",\"temp\":%d,\"humi\":%d,\"gasPPM\":%d,\"gasDig\":%d,\"ldrDig\":%d,\"ldrPer\":%d,\"flameDig\":%d,\"flamePer\":%d,\"alarm\":%d,\"fan\":%d,\"led\":%d}",
			temp, humi, gasPPM, gasDig, ldrDig, ldrPer, flameDig, flamePer, alarmFlag, fan, led);
  return strlen(buf);
}

/*******************************************************************************
* MQTT_Client_SendData
* 描述  ：上传数据到平台MQTT服务器
* 输入  ：无
* 输出  ：无
*******************************************************************************/	
void MQTT_Client_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[256];
  char topic[64]; // MQTT topic

	short body_len = 0, i = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	MQTT_Client_SendData-MQTT\r\n");
	memset(buf, 0, sizeof(buf));//清空数组内容
	sprintf(topic, "room/%s/report", DEVICEID);
	
	body_len = MQTT_Client_FillBuf(buf);	//获取当前需要发送的数据流的总长度
	UsartPrintf(USART_DEBUG,"topic: %s, payload: %s\r\n",topic, buf);
	if(body_len)
	{		
		if(MQTT_PacketSaveData(topic, body_len, NULL, 5, &mqttPacket) == 0)							//封包
		{
			for(; i < body_len; i++){
				mqttPacket._data[mqttPacket._len++] = buf[i];
			}
			UsartPrintf(USART_DEBUG, "ESP8266_SendData\r\n");
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:EDP_NewBuffer Failed\r\n");
	}
	
}

/**
 * @brief 发送心跳到 MQTT 服务器
 */
void MQTT_Client_SendHeartbeat(void)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
    char buf[64];
    char topic[64];
    short body_len = 0, i = 0;

    UsartPrintf(USART_DEBUG, "Tips: MQTT_Client_SendHeartbeat\r\n");
    memset(buf, 0, sizeof(buf));
    sprintf(topic, "room/%s/heartbeat", DEVICEID);

    // 直接构造心跳 JSON
    body_len = snprintf(buf, sizeof(buf), "{\"dev\":\"%s\"}", DEVICEID);
    UsartPrintf(USART_DEBUG, "topic: %s, payload: %s\r\n", topic, buf);

    if (body_len > 0 && body_len < sizeof(buf))
    {
        if (MQTT_PacketSaveData(topic, body_len, NULL, 5, &mqttPacket) == 0)
        {
            for (; i < body_len; i++)
            {
                mqttPacket._data[mqttPacket._len++] = buf[i];
            }
            ESP8266_SendData(mqttPacket._data, mqttPacket._len);
            MQTT_DeleteBuffer(&mqttPacket);
        }
        else
        {
            UsartPrintf(USART_DEBUG, "WARN: MQTT_PacketSaveData Failed\r\n");
        }
    }
}

/*******************************************************************************
* MQTT_Client_Publish
* 描述  ：发布消息
* 输入  ：topic：发布的主题
*         msg：消息内容
* 输出  ：无
*******************************************************************************/	
void MQTT_Client_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
	//UsartPrintf(USART_DEBUG, "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}

}

/*******************************************************************************
* MQTT_Client_Subscribe
* 描述  ：订阅MQTT服务器的topic
* 输入  ：订阅类型（Dev_Att_Rep: 设备属性上报; Dev_Att_Acq: 设备属性获取）
* 输出  ：无
*******************************************************************************/	
void MQTT_Client_Subscribe(u8 Sub)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
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
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}

}

/*******************************************************************************
* MQTT_Client_RevPro
* 描述  ：MQTT服务器返回数据检测
* 输入  ：dataPtr：平台返回的数据
* 输出  ：无
*******************************************************************************/	
void MQTT_Client_RevPro(unsigned char *cmd)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	
	short result = 0;

	type = MQTT_UnPacketRecv(cmd);

	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体

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
						timeCount=200;
				}
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
				{
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//回复命令
					MQTT_DeleteBuffer(&mqttPacket);									//删包
				}
			}
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
		break;
		
		default:
			UsartPrintf(USART_DEBUG,"Tips:	default\r\n");
			result = -1;
		break;
	}

	ESP8266_Clear();									//清空缓存

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
