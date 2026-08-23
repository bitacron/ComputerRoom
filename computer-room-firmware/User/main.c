/**
 ************************************* Copyright ****************************** 
 * @FileName   : main.c
 * @Author     : Zhmy
 * @Date       : 2026-03-10
 * @Description: 01. DHT11传感器测量温湿度
 *							 02. MQ2烟雾传感器测量烟雾浓度。包括模拟输出和数字输出。模拟输出转化为PPM，数字输出为1或0。
 *							 03. LDR光敏电阻测量光照。包括模拟输出和数字输出。模拟输出转化为百分比，数字输出为1或0。
 *							 04. Flame火焰传感器测量火焰。包括模拟输出和数字输出。模拟输出转化为百分比，数字输出为1或0。
 *							 05. 蜂鸣器支持自动报警，兼容高低电平触发方式的蜂鸣器。
 *							 06. 继电器远程反控散热器，兼容高低电平触发方式的继电器。
 *							 07. Led支持远程反控开关灯。
 *							 08. OLED显示屏显示感知层的数据。
 *							 09. ESP8266-01连接Wifi将感知层的设别通过MQTT协议发送给服务器，并接收服务器的反控指令。
 *							 10. 串口和PC端通信调试设备。
 *								DHT11   DATA----PB14
 *								        VCC-----3.3V
 *								OLED    SCL-----PB8
 *								    	  SDA-----PB9
 *								        VCC-----5V
 *								MQ2     AO------PA1
 *								        DO------PA4
 *								        VCC-----5V
 *								LDR     AO------PB1
 *								        DO------PB13
 *								        VCC-----3.3V
 *								Flame   AO------PB0
 *								        DO------PA0
 *								        VCC-----3.3V
 *								BEEP	  IO------PB12
 *								        VCC-----3.3V
 *								Relay   IN------PA12
 *								        VCC-----5V
 *  							        NO------Radiator
 *								        COM-----5V
 *								Led     + ------PB5
 *								        VCC-----3.3V
 *								Usart1  RXD-----PA9
 *											  TXD-----PA10
 *								        VCC-----5V
 *								ESP8266 RXD-----PA2
 *											  TXD-----PA3
 *								        VCC-----3.3V
  ******************************************************************************
 */
#include "stm32f10x.h"
#include "delay.h"
#include "oled.h"
#include "actuators.h"
#include "dht11.h"
#include "mq2.h"
#include "beep.h"
#include "stdio.h"
#include "light_dependent_resisto.h"
#include "flame_sensor.h"
#include "usart.h"
#include "esp8266.h"
#include "mqtt_client.h"
#include "adc.h"
#include "timer.h"

#include <string.h>

#define SAMPLE_PERIOD_MS   1000u
#define OLED_PERIOD_MS      500u
#define REPORT_PERIOD_MS   5000u

uint8_t temp, humi, ldrDig, alarmFlag, fan, led, flameDig, flamePer, gasDig, ldrPer;
uint16_t gasPPM;  // MQ2 PPM 可达上千，不可用 uint8_t

u8 ESP8266_INIT_OK = 0;
volatile uint8_t report_asap = 0;

int main(void)
{
	unsigned char *dataPtr = NULL;
	uint32_t last_sample_ms;
	uint32_t last_oled_ms;
	uint32_t last_report_ms;
	uint32_t now;

	ADC1_Init();
	ADC2_Init();

	OLED_Init();
	OLED_Clear();

	Flame_Init_AO();
	Flame_Init_DO();

	MQ2_Init_AO();
	MQ2_Init_DO();

	LDR_Init_AO();
	LDR_Init_DO();

	Beep_Init();

	Led_Init();
	Relay_Init();

	Usart1_Init(115200);
	Usart2_Init(115200);
	UsartPrintf(USART_DEBUG, "========================\r\n");

	/* 1ms 系统节拍：72MHz / (71+1) / (999+1) = 1000Hz */
	TIM3_Int_Init(999, 71);

	ESP8266_Init();

	while (MQTT_Client_DevLink()) {
		Delay_ms(500);
	}
	MQTT_Client_Subscribe(Dev_Att_Rep);

	MQTT_Client_SendOnline();

	OLED_ShowString(1, 1, "Init_Finish");
	Beep_ShortBeep(200);
	OLED_Clear();

	/* OLED 标签只画一次，循环里只刷新数值 */
	OLED_ShowString(1, 1, "Compoter Room!");
	OLED_ShowString(2, 1, "temp:");
	OLED_ShowString(2, 9, "humi:");
	OLED_ShowString(3, 1, "gas:");
	OLED_ShowString(3, 9, "flame:");
	OLED_ShowString(4, 1, "ldr:");

	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");

	last_sample_ms = last_oled_ms = last_report_ms = millis();

	while (1)
	{
		now = millis();

		/* ---- 采样 + 本地报警 ---- */
		if ((now - last_sample_ms) >= SAMPLE_PERIOD_MS)
		{
			uint16_t gasAna;
			uint16_t ldrAna;
			uint16_t flameAna;

			last_sample_ms = now;

			DHT11_ReadData(&temp, &humi);

			gasAna = MQ2_GetAnalogAverage(ADC_Channel_1, 10);
			gasPPM = (uint16_t)MQ2_GetGasPPM();
			gasDig = MQ2_GetDigital();

			ldrDig = LDR_GetDigital();
			ldrAna = LDR_GetAnalogAverage();
			ldrPer = LDR_GetPercentage();

			flameDig = Flame_GetDigital();
			flameAna = Flame_GetAnalogAverage();
			flamePer = Flame_GetPercentage();

			if (humi < 60 && temp < 50 && gasPPM < 20 && flameDig == 0) {
				Beep_Off();
			} else {
				Beep_On();
			}
			fan = Relay_GetState();
			alarmFlag = Beep_GetState();
			led = Led_GetState();
			{
				char measure_time[24];
				/* 与 MQTT 上报一致：有校时用墙钟，否则占位 */
				if (ESP8266_GetTime(measure_time, sizeof(measure_time)) != 0)
					snprintf(measure_time, sizeof(measure_time), "0000-00-00 00:00:00");
				UsartPrintf(USART_DEBUG,
					"measureTime=%s, temp=%d, humi=%d, gasPPM=%d, gasAna=%d,gasDig=%d, alarm=%d, fan=%d, led = %d, ldrDig=%d,ldrAna=%d,ldrPer=%d, flameDig=%d, flameAna=%d, flamePer=%d\r\n",
					measure_time, temp, humi, gasPPM, gasAna, gasDig, alarmFlag, fan, led,
					ldrDig, ldrAna, ldrPer, flameDig, flameAna, flamePer);
			}
		}

		/* ---- OLED 数值刷新 ---- */
		if ((now - last_oled_ms) >= OLED_PERIOD_MS)
		{
			last_oled_ms = now;
			OLED_ShowNum(2, 6, temp, 2);
			OLED_ShowNum(2, 14, humi, 2);
			OLED_ShowNum(3, 5, gasPPM, 4);
			OLED_ShowNum(3, 15, flameDig, 1);
			OLED_ShowNum(4, 5, ldrDig, 1);
		}

		/* ---- MQTT 周期上报 / 指令后尽快上报 ---- */
		if (report_asap || (now - last_report_ms) >= REPORT_PERIOD_MS)
		{
			report_asap = 0;
			last_report_ms = now;
			UsartPrintf(USART_DEBUG, "MQTT_Client_Publish\r\n");
			MQTT_Client_SendData();
			ESP8266_Clear();
		}

		/* ---- 每圈短轮询下行 ---- */
		dataPtr = ESP8266_GetIPD(3);
		if (dataPtr != NULL) {
			MQTT_Client_RevPro(dataPtr);
			MQTT_Client_FlushCmdAck();
		}
	}
}
