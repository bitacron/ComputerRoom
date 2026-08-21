#include "stm32f10x.h"

#include "esp8266.h"

#include "aliyun.h"
#include "mqttkit.h"

#include "usart.h"
#include "delay.h"

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
/*******************************************************************************
* 函数名：AliYun_DevLink
* 描述  ：连接阿里云 IoT（当前为空实现，保留接口）
* 输入  ：无
* 输出  ：1-成功 0-失败
*******************************************************************************/
_Bool AliYun_DevLink(void)
{
	
	uint8_t retval=0;
	uint16_t count = 0;


	
}

/*******************************************************************************
* 函数名：Aliyun_FillBuf
* 描述  ：填充阿里云上报数据缓冲（温度/湿度/烟雾/光照）
* 输入  ：buf-输出缓冲
* 输出  ：payload 长度
*******************************************************************************/
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

/*******************************************************************************
* 函数名：Aliyun_SendData
* 描述  ：上传数据到阿里云（当前为空实现）
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void Aliyun_SendData(void)
{
	

}

/*******************************************************************************
* 函数名：Aliyun_RevPro
* 描述  ：处理阿里云平台下行数据（当前为空实现）
* 输入  ：cmd-平台返回数据
* 输出  ：无
*******************************************************************************/
void Aliyun_RevPro(unsigned char *cmd)
{
	

}
