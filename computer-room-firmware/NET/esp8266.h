#ifndef _ESP8266_H_
#define _ESP8266_H_


#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志


void ESP8266_Init(void);

void ESP8266_Clear(void);
extern void ESP_Send(const char* str);
void ESP8266_SendData(unsigned char *data, unsigned short len);

unsigned char *ESP8266_GetIPD(unsigned short timeOut);

/*******************************************************************************
* 函数名：ESP8266_SNTP_Init
* 描述  ：WiFi 获取 IP 后校时：优先 SNTP，失败则 HTTP Date（兼容旧 AT）
* 输入  ：无
* 输出  ：无（失败仅打印提示，不阻塞后续 MQTT）
*******************************************************************************/
void ESP8266_SNTP_Init(void);

/*******************************************************************************
* 函数名：ESP8266_GetTime
* 描述  ：读取墙钟为 yyyy-MM-dd HH:mm:ss（SNTP 查询或缓存+millis 推算）
* 输入  ：time_str-输出缓冲，len-缓冲长度（建议 >= 20）
* 输出  ：0-成功，1-失败
*******************************************************************************/
_Bool ESP8266_GetTime(char *time_str, unsigned short len);


#endif
