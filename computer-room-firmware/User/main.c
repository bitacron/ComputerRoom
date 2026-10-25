/**
 ************************************* Copyright ****************************** 
 * @FileName   : main.c
 * @Author     : Zhmy
 * @Date       : 2026-03-10
 * @Description: 01. DHT11传感器测量温湿度
 *							 02. MQ2烟雾传感器测量烟雾浓度。包含模拟输出和数字输出。模拟输出转化为PPM，数字输出为1或0。
 *							 03. LDR光敏电阻测量光照。包含模拟输出和数字输出。模拟输出转化为百分比，数字输出为1或0。
 *							 04. Flame火焰传感器测量火焰。包含模拟输出和数字输出。模拟输出转化为百分比，数字输出为1或0。
 *							 05. Leak漏水/雨滴传感器测量积水。包含模拟输出和数字输出。模拟输出转化为百分比，数字输出为1或0。
 *							 06. 蜂鸣器支持自动报警，兼容高低电平触发方式的蜂鸣器。
 *							 07. 继电器远程反控散热器，兼容高低电平触发方式的继电器。
 *							 08. Led支持远程反控开关灯。
 *							 09. OLED显示屏显示感知层的数据。
 *							 10. ESP8266-01连接Wifi将感知层的数据通过MQTT协议发送给服务器，并接收服务器的反控指令。
 *							 11. 串口和PC通信调试设备。
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
 *								Leak    AO------PA5
 *								        DO------PA8
 *								        VCC-----5V
 *								BEEP	  IO------PB12
 *								        VCC-----3.3V
 *								Led relay IN ------PB5
 *								        VCC-----5V
 *								CoolingFan relay IN------PA12
 *								        VCC-----5V
 *  							        NO------CoolingFan
 *								        COM-----5V
 *								Heater relay IN----------PB7
 *								Humidifier relay IN------PA11
 *								DehumidifyingFan relay IN-PB15
 *								KEY1    IO------PB10   (勿接 PB1，与 LDR AO 冲突)
 *								KEY2    IO------PB11
 *								        GND-----GND    低电平有效，无锁自复位；KEY1翻LED，KEY2翻散热风扇
 *								KEY3    IO------PA6    Heater
 *								KEY4    IO------PA7    Humidifier
 *								KEY5    IO------PB6    DehumidifyingFan
 *								KEY3-KEY5 buttons connect to GND; active-low, momentary
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
#include "control.h"
#include "key.h"
#include "dht11.h"
#include "mq2.h"
#include "beep.h"
#include "stdio.h"
#include "light_dependent_resisto.h"
#include "flame_sensor.h"
#include "leak_sensor.h"
#include "usart.h"
#include "esp8266.h"
#include "custom.h"
#include "adc.h"
#include "timer.h"

#include <string.h>

#define SAMPLE_PERIOD_MS   1000u
#define OLED_PERIOD_MS      500u
#define REPORT_PERIOD_MS   5000u

uint8_t temp, humi, ldrDig, alarmFlag, coolingFan, led, heater, humidifier, dehumidifyingFan, flameDig, flamePer, gasDig, ldrPer, leakDig, leakPer;
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
    uint8_t displayed_mode = 255;

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

	Leak_Init_AO();
	Leak_Init_DO();

	Beep_Init();

	Led_Init();
	CoolingFan_Init();
	Heater_Init();
	Humidifier_Init();
	DehumidifyingFan_Init();
	Key_Init();

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
    report_asap = 1;

	OLED_ShowString(1, 1, "Init_Finish");
	Beep_ShortBeep(200);
	OLED_Clear();

	/* OLED 标题只画一次，循环里只刷新数值 */
	OLED_ShowString(1, 1, "Mode: MANUAL    ");
	OLED_ShowString(2, 1, "temp:");
	OLED_ShowString(2, 9, "humi:");
	OLED_ShowString(3, 1, "gas:");
	OLED_ShowString(3, 9, "flame:");
	OLED_ShowString(4, 1, "ldr:");
	OLED_ShowString(4, 9, "leak:");

	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");

	last_sample_ms = last_oled_ms = last_report_ms = millis();

	while (1)
	{
		now = millis();

		/* ---- 本地按键：无锁自复位，按下松开后翻转一次 ---- */
		{
			uint8_t keyNum = Key_GetNum();
            if (keyNum >= 1 && keyNum <= 5) {
                const char *sources[] = { "KEY1", "KEY2", "KEY3", "KEY4", "KEY5" };
                Control_SetMode(CONTROL_MANUAL, sources[keyNum - 1]);
            }
            if (keyNum == 6) Control_SetMode(!Control_GetMode(), "KEY6");
			if (keyNum == 1)
			{
				Led_Turn();
				led = Led_GetState();
				report_asap = 1;
				UsartPrintf(USART_DEBUG, "KEY1 led=%d\r\n", led);
			}
			else if (keyNum == 2)
			{
				CoolingFan_Turn();
				coolingFan = CoolingFan_GetState();
				report_asap = 1;
				UsartPrintf(USART_DEBUG, "KEY2 coolingFan=%d\r\n", coolingFan);
			}
			else if (keyNum == 3)
			{
				Heater_Turn();
				heater = Heater_GetState();
				report_asap = 1;
				UsartPrintf(USART_DEBUG, "KEY3 heater=%d\r\n", heater);
			}
			else if (keyNum == 4)
			{
				Humidifier_Turn();
				humidifier = Humidifier_GetState();
				report_asap = 1;
				UsartPrintf(USART_DEBUG, "KEY4 humidifier=%d\r\n", humidifier);
			}
			else if (keyNum == 5)
			{
				DehumidifyingFan_Turn();
				dehumidifyingFan = DehumidifyingFan_GetState();
				report_asap = 1;
				UsartPrintf(USART_DEBUG, "KEY5 dehumidifyingFan=%d\r\n", dehumidifyingFan);
			}
		}

		/* ---- 采样 + 本地报警 ---- */
		if ((now - last_sample_ms) >= SAMPLE_PERIOD_MS)
		{
			uint8_t climate_valid;
            uint16_t gasAna;
			uint16_t ldrAna;
			uint16_t flameAna;
			uint16_t leakAna;

			last_sample_ms = now;

			climate_valid = DHT11_ReadData(&temp, &humi);

			gasAna = MQ2_GetAnalogAverage(ADC_Channel_1, 10);
			gasPPM = (uint16_t)MQ2_GetGasPPM();
			gasDig = MQ2_GetDigital();

			ldrDig = LDR_GetDigital();
			ldrAna = LDR_GetAnalogAverage();
			ldrPer = LDR_GetPercentage();

			flameDig = Flame_GetDigital();
			flameAna = Flame_GetAnalogAverage();
			flamePer = Flame_GetPercentage();

			leakDig = Leak_GetDigital();
			leakAna = Leak_GetAnalogAverage();
			leakPer = Leak_GetPercentage();

			if (humi < 60 && temp < 50 && gasPPM < 20 && flameDig == 0 && leakDig == 0) {
				Beep_Off();
			} else {
				Beep_On();
			}
			Control_AutoUpdate(temp, humi, ldrPer, climate_valid);
            coolingFan = CoolingFan_GetState();
			alarmFlag = Beep_GetState();
			led = Led_GetState();
			heater = Heater_GetState();
			humidifier = Humidifier_GetState();
			dehumidifyingFan = DehumidifyingFan_GetState();
			{
				char measure_time[24];
				/* 与 MQTT 上报一致：有校时用墙钟，否则占位 */
				if (ESP8266_GetTime(measure_time, sizeof(measure_time)) != 0)
					snprintf(measure_time, sizeof(measure_time), "0000-00-00 00:00:00");
				UsartPrintf(USART_DEBUG,
					"mode=%d, measureTime=%s, temp=%d, humi=%d, gasPPM=%d, gasAna=%d,gasDig=%d, alarm=%d, coolingFan=%d, led = %d, ldrDig=%d,ldrAna=%d,ldrPer=%d, flameDig=%d, flameAna=%d, flamePer=%d, leakDig=%d,leakAna=%d,leakPer=%d\r\n",
					Control_GetMode(),  measure_time, temp, humi, gasPPM, gasAna, gasDig, alarmFlag, coolingFan, led,
					ldrDig, ldrAna, ldrPer, flameDig, flameAna, flamePer,
					leakDig, leakAna, leakPer);
			}
		}

		if (displayed_mode != Control_GetMode()) {
            displayed_mode = Control_GetMode();
            OLED_ShowString(1, 1, displayed_mode ? "Mode: AUTO      " : "Mode: MANUAL    ");
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
			OLED_ShowNum(4, 15, leakDig, 1);
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

		/* ---- 每圈尝试收一次下行 ---- */
		dataPtr = ESP8266_GetIPD(3);
		if (dataPtr != NULL) {
			MQTT_Client_RevPro(dataPtr);
			MQTT_Client_FlushCmdAck();
		}
	}
}
