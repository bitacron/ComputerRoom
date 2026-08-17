/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "aliyun.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"

//C库
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#include "led.h"

#define MQTT_CLIENT_ID   "1111111|securemode=3\\,signmethod=hmacsha1\\,timestamp=1112222|"
#define MQTT_USER_NAME   "mqqt_stm32&k182wh0rHJY"
#define MQTT_PASSWD      "4720BA12105D2ED5505BDDFBFEAD9F3E05885E10"
#define BROKER_ASDDRESS  "k182wh0rHJY.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define TTGs_ASDDRESS    "/k182wh0rHJY/mqqt_stm32/user/get"
#define SUB_TOPIC        "/sys/k182wh0rHJY/mqtt_stm32/thing/service/property/set"
#define PUB_TOPIC        "/sys/k182wh0rHJY/mqtt_stm32/thing/event/property/post"
#define JSON_FORMAT      "{\\\"params\\\":{\\\"temp\\\":%d\\,\\\"humi\\\":%d\\}\\,\\\"version\\\":\\\"1.0.0\\\"}"

extern unsigned char esp8266_buf[128];
extern u8 alarmFlag;//是否报警的标志
extern u8 alarm_is_free;
//==========================================================
//	函数名称：	AliYun_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool AliYun_DevLink(void)
{
	
	uint8_t retval=0;
	uint16_t count = 0;


	
}

//==========================================================
//	函数名称：	OneNet_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	topics：订阅的topic
//				topic_cnt：topic个数
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_SUBSCRIBE-需要重发
//
//	说明：		
//==========================================================

extern int smoke;
extern int humi;
extern int temp;
extern int light;
//extern u8 led;
unsigned char Aliyun_FillBuf(char *buf)
{
	char text[32];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, ",;");
		
	memset(text, 0, sizeof(text));
	sprintf(text, "Temp,%d;", temp);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "Humi,%d;", humi);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "Smoke,%d;", smoke);
	strcat(buf, text);
	
	
	memset(text, 0, sizeof(text));
	sprintf(text, "Light,%d;", light);
	strcat(buf, text);
	//memset(text, 0, sizeof(text));
	//sprintf(text, "LED,%d;", led);
	//strcat(buf, text);
	
	return strlen(buf);
}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void Aliyun_SendData(void)
{
	

}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void Aliyun_RevPro(unsigned char *cmd)
{
	

}
