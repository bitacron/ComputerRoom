#include "stm32f10x.h"

#include "esp8266.h"

#include "onenet.h"
#include "mqttkit.h"

#include "usart.h"
#include "delay.h"

#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#include "actuators.h"

#define AUTH_INFO	"version=2018-10-31&res=products%2F22tK9YyZ17%2Fdevices%2Flibrary&et=1747828759&method=md5&sign=kzR5wTDQyjzaCvSC8fsKgw%3D%3D"
#define DEVID		"test"
#define PROID		"032xlmj49q"


extern unsigned char esp8266_buf[128];
extern u8 alarmFlag;//是否报警的标志
extern u8 alarm_is_free;
/*******************************************************************************
* 函数名：OneNet_DevLink
* 描述  ：与onenet创建连接。与onenet平台建立连接
* 输入  ：无
* 输出  ：1-成功 0-失败
*******************************************************************************/
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s	--- 连接中....\r\n"
                        , PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
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


u8 velue0 = 0;
u8 velue1 = 0;
extern uint16_t gasPPM;
extern uint8_t humi;
extern uint8_t temp;
extern uint8_t ldrDig;
//extern u8 led;

/*******************************************************************************
* 函数名：OneNet_FillBuf
* 描述  ：填充 OneNET 属性上报 JSON
* 输入  ：buf-输出缓冲
* 输出  ：payload 长度
*******************************************************************************/
unsigned char OneNet_FillBuf(char *buf)
{
	
	char text[200];
	
	strcpy(buf,"{\"id\":\"171546\",\"version\":\"1.0\",\"params\":{");
	memset(text,0,sizeof(text));
	sprintf(text,"\"temp\":{\"value\":%d},",temp);
	strcat(buf,text);

	memset(text,0,sizeof(text));
	sprintf(text,"\"humi\":{\"value\":%d},",humi);
	strcat(buf,text);
	
	memset(text,0,sizeof(text));
	sprintf(text,"\"smoke\":{\"value\":%d},",gasPPM);
	strcat(buf,text);
	
	memset(text,0,sizeof(text));
	sprintf(text,"\"light\":{\"value\":%d}}}",ldrDig);
	strcat(buf,text);

	return strlen(buf);

}

/*******************************************************************************
* 函数名：OneNet_SendData
* 描述  ：上传数据到平台
* 输入  ：type：发送数据的格式
* 输出  ：无
*******************************************************************************/
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[128];
  char topic[64]; // MQTT topic
	
	short body_len = 0, i = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");
	memset(buf, 0, sizeof(buf));//清空数组内容
	sprintf(topic, "$sys/032xlmj49q/test/thing/property/post");
	sprintf(topic, "$sys/%s/%s/thing/property/post", PROID, DEVID);
	
	body_len = OneNet_FillBuf(buf);	//获取当前需要发送的数据流的总长度
	UsartPrintf(USART_DEBUG,"%s\r\n",buf);
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


/*******************************************************************************
* 函数名：OneNET_Publish
* 描述  ：发布消息
* 输入  ：topic：发布的主题 msg：消息内容
* 输出  ：无
*******************************************************************************/
void OneNET_Publish(const char *topic, const char *msg)
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
* 函数名：OneNET_Subscribe
* 描述  ：订阅
* 输入  ：Dev_Rro_Set //设备属性上报 Dev_Att_Acq //设备属性获取
* 输出  ：无
*******************************************************************************/
void OneNET_Subscribe(u8 Sub)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
	char topic_buf[56];
	const char *topic = topic_buf;
	switch(Sub)
	{
		case Dev_Rro_Set:snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/thing/property/post", PROID, DEVID);
		     UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topic_buf);
					break;
		case Dev_Att_Acq:snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/thing/property/post/reply", PROID, DEVID);
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
* 函数名：OneNet_RevPro
* 描述  ：平台返回数据检测
* 输入  ：dataPtr：平台返回的数据
* 输出  ：无
*******************************************************************************/
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				printf("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
				{
					printf("Tips:	Send CmdResp\r\n");
					
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//回复命令
					MQTT_DeleteBuffer(&mqttPacket);									//删包
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
//				printf("Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, ':');					//搜索'}'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
		if(strstr((char *)req_payload, "onoff"))		//搜索"onoff"
		{
			printf("onoff = %d", num);
			//led = num;
		}
		else if(strstr((char *)req_payload, "switch"))		//搜索"switch"
		{
			printf("switch = %d", num);
			velue1 = num;
		}
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
