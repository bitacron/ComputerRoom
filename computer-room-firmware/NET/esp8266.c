#include "stm32f10x.h"

#include "esp8266.h"

#include "delay.h"
#include "timer.h"
#include "usart.h"
#include "oled.h"

#include <string.h>
#include <stdio.h>


#define WIFI_SSID 						"CMCC-AC"								//	WIFI的名称 必须用2.4G的wifi不能用5G的，且不能用中文、空格
#define WIFI_PSWD 						"66661100"				    //	WIFI密码

//#define SERVER_HOST						"mqtts.heclouds.com"			//	MQTT服务器域名或IP
#define SERVER_PORT						"1883"								//	MQTT服务器端口（一般为1883不用改）
#define TIME_HTTP_PORT					"8080"								//	业务服务HTTP端口，用于 /api/time 校时

//#define SERVER_HOST						"broker.emqx.io"			//	MQTT服务器域名或IP
#define SERVER_HOST						"192.168.1.10"			//	MQTT服务器域名或IP
#define ESP8266_WIFI_INFO			"AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PSWD "\"\r\n"
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"" SERVER_HOST "\"," SERVER_PORT "\r\n"

unsigned char esp8266_buf[2048];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;

extern u8 ESP8266_INIT_OK;

/*******************************************************************************
* 函数名：ESP_Send
* 描述  ：通过 USART2 发送字符串（简易调试用）
* 输入  ：str-以'\0'结尾的字符串
* 输出  ：无
*******************************************************************************/
extern void ESP_Send(const char* str)
{
	int i; 
	for(i = 0; i<strlen(str); i++)
	{
			USART_SendData(USART2, str[i]);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);		
	}
	Delay_ms(100);
}
/*******************************************************************************
* 函数名：ESP8266_Clear
* 描述  ：清空缓存
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

/*******************************************************************************
* 函数名：ESP8266_WaitRecive
* 描述  ：等待接收完成。循环调用检测是否接收完成
* 输入  ：无
* 输出  ：REV_OK-接收完成 REV_WAIT-接收超时未完成
*******************************************************************************/
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

/*******************************************************************************
* 函数名：ESP8266_SendCmd
* 描述  ：发送命令
* 输入  ：cmd：命令 res：需要检查的返回指令
* 输出  ：0-成功 1-失败
*******************************************************************************/
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				
				return 0;
			}
			/* 明确失败时尽快返回，避免无SNTP命令空等 */
			if (res[0] != '\0' &&
				(strstr((const char *)esp8266_buf, "ERROR") != NULL ||
				 strstr((const char *)esp8266_buf, "FAIL") != NULL))
			{
				ESP8266_Clear();
				return 1;
			}
		}
		
		Delay_ms(10);
	}
	
	return 1;

}

/*******************************************************************************
* 函数名：ESP8266_SendData
* 描述  ：发送数据
* 输入  ：data：数据 len：长度
* 输出  ：无
*******************************************************************************/
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		Usart_SendString(USART2, data, len);		//发送设备连接请求数据
	}

}

/*******************************************************************************
* 函数名：ESP8266_GetIPD
* 描述  ：获取平台返回的数据。不同网络设备返回的格式不同，需要去调试 如ESP8266的返回格式为 "+IPD,x:yyy" x代表数据长度，yyy是数据内容
* 输入  ：等待的时间(乘以10ms)
* 输出  ：平台返回的原始数据
*******************************************************************************/
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		Delay_ms(5);
		timeOut--;	//延时等待
	} while(timeOut > 0);
	
	return NULL;														//超时还未找到，返回空指针

}

/*******************************************************************************
* 函数名：ESP8266_Init
* 描述  ：初始化ESP8266
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void ESP8266_Init(void)
{
	
//	GPIO_InitTypeDef GPIO_Initure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

//	//ESP8266复位引脚
//	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Initure.GPIO_Pin = GPIO_Pin_14;					//GPIOC14-复位
//	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC, &GPIO_Initure);
//	
//	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_RESET);
//	Delay_ms(250);
//	GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_SET);
//	Delay_ms(500);
	
	ESP8266_Clear();
	
	UsartPrintf(USART_DEBUG, "> 0. AT - 测试MCU-8266通讯\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		Delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "> 1. AT+RST - 软复位8266\r\n");
	ESP8266_SendCmd("AT+RST\r\n", "");
		Delay_ms(500);
	ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");
		Delay_ms(500);
	UsartPrintf(USART_DEBUG, "> 2. AT+CWMODE=1,1 - 设置8266工作模式为STA\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		Delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "> 3. AT+CWDHCP=1,1 - 使能STA模式下DHCP\r\n");
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		Delay_ms(500);
	

	UsartPrintf(USART_DEBUG, "> 4. AT+CWJAP - 连接WIFI\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		Delay_ms(500);

	/* 校时：优先 SNTP（新 AT 固件）；失败则 HTTP Date（兼容旧固件） */
	UsartPrintf(USART_DEBUG, "> 5. Time sync - SNTP/HTTP校时\r\n");
	ESP8266_SNTP_Init();

	/* 校时过程可能打乱状态，连 MQTT 前确认 AT 仍可用 */
	ESP8266_Clear();
	while (ESP8266_SendCmd("AT\r\n", "OK"))
		Delay_ms(200);

	UsartPrintf(USART_DEBUG, "> 6. AT+CIPSTART - 连接服务器 %s:%s\r\n", SERVER_HOST, SERVER_PORT);
	{
		unsigned char cip_retry = 0;
		while (ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		{
			Delay_ms(500);
			cip_retry++;
			if ((cip_retry % 10) == 0)
			{
				UsartPrintf(USART_DEBUG,
					"WARN: CIPSTART 仍失败(第%d次)，请检查MQTT服务是否在线、IP/端口是否正确\r\n",
					cip_retry);
			}
		}
	}
	ESP8266_INIT_OK = 1;
	UsartPrintf(USART_DEBUG, "> 7. ESP8266 Init OK - ESP8266初始化成功\r\n");
}

/* ---------- 设备侧墙钟缓存：HTTP/SNTP 成功后用 millis 推算 ---------- */
static uint8_t  s_time_valid = 0;
static uint8_t  s_sntp_query_ok = 0;	/* 1=AT 支持 CIPSNTPTIME，上报时可查询 */
static uint16_t s_year;
static uint8_t  s_mon, s_day, s_hour, s_min, s_sec;
static uint32_t s_sync_ms;

/*******************************************************************************
* 函数名：ESP8266_MonthFromName
* 描述  ：英文月份缩写转 1~12
* 输入  ：name-至少 3 字符，如 "Aug"
* 输出  ：1~12，无法识别返回 0
*******************************************************************************/
static unsigned char ESP8266_MonthFromName(const char *name)
{
	static const char *months[12] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	unsigned char i;

	if (name == NULL)
		return 0;
	for (i = 0; i < 12; i++)
	{
		if (strncmp(name, months[i], 3) == 0)
			return (unsigned char)(i + 1);
	}
	return 0;
}

/*******************************************************************************
* 函数名：ESP8266_IsLeap
* 描述  ：判断是否闰年
*******************************************************************************/
static uint8_t ESP8266_IsLeap(uint16_t y)
{
	return ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) ? 1 : 0;
}

/*******************************************************************************
* 函数名：ESP8266_CacheWallTime
* 描述  ：缓存墙钟并记录同步时的 millis，供后续推算
*******************************************************************************/
static void ESP8266_CacheWallTime(uint16_t y, uint8_t mon, uint8_t day,
	uint8_t hour, uint8_t min, uint8_t sec)
{
	s_year = y;
	s_mon = mon;
	s_day = day;
	s_hour = hour;
	s_min = min;
	s_sec = sec;
	s_sync_ms = millis();
	s_time_valid = 1;
}

/*******************************************************************************
* 函数名：ESP8266_FormatCachedTime
* 描述  ：按缓存墙钟 + millis 偏移格式化为 yyyy-MM-dd HH:mm:ss（默认按本地已校时区）
* 输出  ：0-成功，1-无有效缓存
*******************************************************************************/
static _Bool ESP8266_FormatCachedTime(char *dst, unsigned short dst_len)
{
	static const uint8_t mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	uint32_t add_s, carry;
	uint16_t y;
	uint8_t mon, day, hour, min, sec;
	uint8_t dim;

	if (!s_time_valid || dst == NULL || dst_len < 20)
		return 1;

	add_s = (millis() - s_sync_ms) / 1000UL;
	y = s_year;
	mon = s_mon;
	day = s_day;
	hour = s_hour;
	min = s_min;
	sec = s_sec;

	carry = (uint32_t)sec + add_s;
	sec = (uint8_t)(carry % 60UL);
	carry = (uint32_t)min + carry / 60UL;
	min = (uint8_t)(carry % 60UL);
	carry = (uint32_t)hour + carry / 60UL;
	hour = (uint8_t)(carry % 24UL);
	carry = carry / 24UL;	/* 剩余按“天”累加 */

	while (carry > 0)
	{
		dim = mdays[mon - 1];
		if (mon == 2 && ESP8266_IsLeap(y))
			dim = 29;
		if ((uint32_t)day + carry <= dim)
		{
			day = (uint8_t)(day + carry);
			carry = 0;
		}
		else
		{
			carry -= (uint32_t)(dim - day + 1);
			day = 1;
			mon++;
			if (mon > 12)
			{
				mon = 1;
				y++;
			}
		}
	}

	snprintf(dst, dst_len, "%04d-%02d-%02d %02d:%02d:%02d",
		y, mon, day, hour, min, sec);
	return 0;
}

/*******************************************************************************
* 函数名：ESP8266_ParseSntpTime
* 描述  ：解析 +CIPSNTPTIME 载荷为墙钟并写入缓存
* 输出  ：0-成功，1-失败
*******************************************************************************/
static _Bool ESP8266_ParseSntpTime(const char *src)
{
	char week[8], mon[8];
	int day = 0, hour = 0, min = 0, sec = 0, year = 0, month = 0;

	if (src == NULL)
		return 1;
	while (*src == ' ' || *src == '\t')
		src++;

	if (sscanf(src, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec) == 6)
	{
		if (year < 2020 || month < 1 || month > 12 || day < 1 || day > 31)
			return 1;
		ESP8266_CacheWallTime((uint16_t)year, (uint8_t)month, (uint8_t)day,
			(uint8_t)hour, (uint8_t)min, (uint8_t)sec);
		return 0;
	}

	if (sscanf(src, "%7s %7s %d %d:%d:%d %d", week, mon, &day, &hour, &min, &sec, &year) == 7)
	{
		month = ESP8266_MonthFromName(mon);
		if (year < 2020 || month == 0 || day < 1 || day > 31)
			return 1;
		ESP8266_CacheWallTime((uint16_t)year, (uint8_t)month, (uint8_t)day,
			(uint8_t)hour, (uint8_t)min, (uint8_t)sec);
		return 0;
	}
	return 1;
}

/*******************************************************************************
* 函数名：ESP8266_ParseMeasureTimeField
* 描述  ：从缓冲中解析 "measureTime":"yyyy-MM-dd HH:mm:ss" 并缓存（东八区，无需再加8）
* 输出  ：0-成功，1-失败
*******************************************************************************/
static _Bool ESP8266_ParseMeasureTimeField(const char *buf)
{
	const char *p;
	int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;

	p = strstr(buf, "measureTime");
	if (p == NULL)
		return 1;
	p = strchr(p, ':');
	if (p == NULL)
		return 1;
	p++;
	while (*p == ' ' || *p == '\"')
		p++;
	if (sscanf(p, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec) != 6)
		return 1;
	if (year < 2020 || month < 1 || month > 12 || day < 1 || day > 31)
		return 1;
	ESP8266_CacheWallTime((uint16_t)year, (uint8_t)month, (uint8_t)day,
		(uint8_t)hour, (uint8_t)min, (uint8_t)sec);
	return 0;
}

/*******************************************************************************
* 函数名：ESP8266_ParseHttpDate
* 描述  ：解析 HTTP 头 Date:（GMT）并转为东八区后缓存
* 示例  ：Date: Sat, 22 Aug 2026 09:46:37 GMT
*******************************************************************************/
static _Bool ESP8266_ParseHttpDate(const char *buf)
{
	const char *p;
	char week[8], mon[8], tz[8];
	int day = 0, hour = 0, min = 0, sec = 0, year = 0, month = 0;

	p = strstr(buf, "Date:");
	if (p == NULL)
		p = strstr(buf, "date:");
	if (p == NULL)
		return 1;
	p += 5;
	while (*p == ' ')
		p++;

	if (sscanf(p, "%7s %d %7s %d %d:%d:%d %7s",
			week, &day, mon, &year, &hour, &min, &sec, tz) != 8)
		return 1;

	month = ESP8266_MonthFromName(mon);
	if (year < 2020 || month == 0)
		return 1;

	hour += 8;
	if (hour >= 24)
	{
		hour -= 24;
		day++;
		{
			static const uint8_t mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
			uint8_t dim = mdays[month - 1];
			if (month == 2 && ESP8266_IsLeap((uint16_t)year))
				dim = 29;
			if (day > dim)
			{
				day = 1;
				month++;
				if (month > 12) { month = 1; year++; }
			}
		}
	}

	ESP8266_CacheWallTime((uint16_t)year, (uint8_t)month, (uint8_t)day,
		(uint8_t)hour, (uint8_t)min, (uint8_t)sec);
	return 0;
}

/*******************************************************************************
* 函数名：ESP8266_HttpGetTimeOnce
* 描述  ：对指定 host:port 发 HTTP GET path，从响应解析 measureTime 或 Date
* 输出  ：0-成功，1-失败
*******************************************************************************/
static _Bool ESP8266_HttpGetTimeOnce(const char *host, const char *port, const char *path)
{
	char cmd[96];
	char req[160];
	unsigned char t;
	unsigned short req_len;

	ESP8266_Clear();
	ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");
	Delay_ms(200);

	sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", host, port);
	UsartPrintf(USART_DEBUG, "Tips: HTTP time CIPSTART %s:%s\r\n", host, port);
	if (ESP8266_SendCmd(cmd, "CONNECT") != 0)
	{
		if (ESP8266_SendCmd(cmd, "OK") != 0)
		{
			UsartPrintf(USART_DEBUG, "WARN: HTTP CIPSTART fail %s:%s\r\n", host, port);
			return 1;
		}
	}

	/* HTTP/1.0 + Connection:close，便于旧模组收完即断 */
	snprintf(req, sizeof(req),
		"GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n", path, host);
	req_len = (unsigned short)strlen(req);
	sprintf(cmd, "AT+CIPSEND=%d\r\n", req_len);
	if (ESP8266_SendCmd(cmd, ">") != 0)
	{
		UsartPrintf(USART_DEBUG, "WARN: HTTP CIPSEND prompt fail\r\n");
		ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");
		return 1;
	}

	ESP8266_Clear();
	Usart_SendString(USART2, (unsigned char *)req, req_len);

	/* 直接轮询接收缓冲，避免 WaitRecive 清 cnt 导致分片响应被覆盖 */
	for (t = 0; t < 400; t++)
	{
		if (esp8266_cnt > 32)
		{
			if (esp8266_cnt < sizeof(esp8266_buf))
				esp8266_buf[esp8266_cnt] = 0;
			else
				esp8266_buf[sizeof(esp8266_buf) - 1] = 0;

			if (ESP8266_ParseMeasureTimeField((char *)esp8266_buf) == 0 ||
				ESP8266_ParseHttpDate((char *)esp8266_buf) == 0)
			{
				ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");
				Delay_ms(200);
				return 0;
			}
		}
		Delay_ms(10);
	}

	ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");
	Delay_ms(200);
	UsartPrintf(USART_DEBUG, "WARN: HTTP time parse timeout %s\r\n", host);
	return 1;
}

/*******************************************************************************
* 函数名：ESP8266_SyncTimeByHttpDate
* 描述  ：HTTP 校时。优先局域网业务服务 /api/time（与 MQTT 同机），再试公网 Date
* 输出  ：0-成功，1-失败
*******************************************************************************/
static _Bool ESP8266_SyncTimeByHttpDate(void)
{
	UsartPrintf(USART_DEBUG, "Tips: fallback HTTP time sync...\r\n");

	/* 1) 与 MQTT 相同主机的 Spring Boot（无需公网 DNS） */
	if (ESP8266_HttpGetTimeOnce(SERVER_HOST, TIME_HTTP_PORT, "/api/time") == 0)
	{
		UsartPrintf(USART_DEBUG, "Tips: LAN /api/time sync OK\r\n");
		return 0;
	}

	/* 2) 公网兜底（部分路由拦截 DNS/80 会失败） */
	if (ESP8266_HttpGetTimeOnce("www.baidu.com", "80", "/") == 0)
	{
		UsartPrintf(USART_DEBUG, "Tips: public HTTP Date sync OK\r\n");
		return 0;
	}

	UsartPrintf(USART_DEBUG, "WARN: HTTP time sync failed\r\n");
	return 1;
}

/*******************************************************************************
* 函数名：ESP8266_GetTime
* 描述  ：读取当前墙钟为 yyyy-MM-dd HH:mm:ss（SNTP 即时读或缓存推算）
* 输入  ：time_str/len
* 输出  ：0-成功，1-失败
*******************************************************************************/
_Bool ESP8266_GetTime(char *time_str, unsigned short len)
{
	unsigned char timeOut = 50;
	char *p;

	if (time_str == NULL || len < 20)
		return 1;
	time_str[0] = '\0';

	/* 仅当初始化确认 SNTP 可用时才发 AT，避免旧固件每次上报卡约 0.5s */
	if (s_sntp_query_ok)
	{
		ESP8266_Clear();
		Usart_SendString(USART2, (unsigned char *)"AT+CIPSNTPTIME?\r\n",
			strlen("AT+CIPSNTPTIME?\r\n"));
		while (timeOut--)
		{
			if (ESP8266_WaitRecive() == REV_OK)
			{
				p = strstr((char *)esp8266_buf, "+CIPSNTPTIME:");
				if (p != NULL)
				{
					p += strlen("+CIPSNTPTIME:");
					if (ESP8266_ParseSntpTime(p) == 0)
					{
						ESP8266_Clear();
						return ESP8266_FormatCachedTime(time_str, len);
					}
				}
				if (strstr((char *)esp8266_buf, "ERROR") != NULL)
				{
					s_sntp_query_ok = 0;
					ESP8266_Clear();
					break;
				}
				ESP8266_Clear();
			}
			Delay_ms(10);
		}
	}

	return ESP8266_FormatCachedTime(time_str, len);
}

/*******************************************************************************
* 函数名：ESP8266_SNTP_Init
* 描述  ：校时入口。先快速试 SNTP；失败再用 HTTP Date。均失败则上报不带 measureTime
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void ESP8266_SNTP_Init(void)
{
	char time_str[24];
	_Bool configured = 0;
	unsigned char retry;

	s_time_valid = 0;
	s_sntp_query_ok = 0;

	/* 多数安信可旧 AT 无此命令：最多试 2 种写法，失败立刻走 HTTP */
	if (ESP8266_SendCmd("AT+CIPSNTPCFG=1,8\r\n", "OK") == 0)
		configured = 1;
	else if (ESP8266_SendCmd(
			"AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\",\"ntp.tencent.com\",\"cn.ntp.org.cn\"\r\n",
			"OK") == 0)
		configured = 1;

	if (configured)
	{
		s_sntp_query_ok = 1;
		/* 再补 NTP 服务器；有的固件接受 CFG=1,8 但默认源不可达 */
		ESP8266_SendCmd(
			"AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\",\"ntp.tencent.com\",\"cn.ntp.org.cn\"\r\n",
			"OK");
		Delay_ms(3000);
		for (retry = 0; retry < 8; retry++)
		{
			/* 直接查时间，避免 GetTime 在未缓存时空转 */
			ESP8266_Clear();
			Usart_SendString(USART2, (unsigned char *)"AT+CIPSNTPTIME?\r\n",
				strlen("AT+CIPSNTPTIME?\r\n"));
			{
				unsigned char t = 80;
				char *p;
				while (t--)
				{
					if (ESP8266_WaitRecive() == REV_OK)
					{
						p = strstr((char *)esp8266_buf, "+CIPSNTPTIME:");
						if (p != NULL)
						{
							p += strlen("+CIPSNTPTIME:");
							if (ESP8266_ParseSntpTime(p) == 0)
							{
								ESP8266_Clear();
								if (ESP8266_FormatCachedTime(time_str, sizeof(time_str)) == 0)
								{
									UsartPrintf(USART_DEBUG, "Tips: SNTP OK, time=%s\r\n", time_str);
									return;
								}
							}
						}
						ESP8266_Clear();
					}
					Delay_ms(10);
				}
			}
			Delay_ms(400);
		}
		UsartPrintf(USART_DEBUG, "WARN: SNTP enabled but time not ready, try HTTP\r\n");
		s_sntp_query_ok = 0;
	}
	else
	{
		UsartPrintf(USART_DEBUG, "WARN: AT+CIPSNTPCFG unsupported, try HTTP Date\r\n");
	}

	if (ESP8266_SyncTimeByHttpDate() == 0)
	{
		if (ESP8266_FormatCachedTime(time_str, sizeof(time_str)) == 0)
			UsartPrintf(USART_DEBUG, "Tips: time sync OK, time=%s\r\n", time_str);
		return;
	}

	UsartPrintf(USART_DEBUG, "WARN: time sync failed, report may omit measureTime\r\n");
}


/*******************************************************************************
* 函数名：USART2_IRQHandler
* 描述  ：串口2收发中断
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //防止串口被刷爆
		esp8266_buf[esp8266_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}

}

