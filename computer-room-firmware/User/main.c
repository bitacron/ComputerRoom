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
 *								LDR     AO------PB13
 *								        DO------PB1
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

#include <string.h>

uint8_t temp, humi, ldrDig, alarmFlag, fan, led, flameDig, flamePer, gasDig, ldrPer;
uint16_t gasPPM;  // MQ2 PPM 可达上千，不可用 uint8_t

u8 ESP8266_INIT_OK = 0;//esp8266
unsigned short timeCount = 0;
int main(void)
{
	unsigned char *dataPtr = NULL;
	 
	ADC1_Init();
	ADC2_Init();

	OLED_Init();
	OLED_Clear();

	Flame_Init_AO();   // 初始化模拟输入引脚（AO）及 ADC
	Flame_Init_DO();   // 初始化数字输出引脚（DO）
	
	MQ2_Init_AO();
	MQ2_Init_DO();

	LDR_Init_AO();
	LDR_Init_DO();
	
	Beep_Init();
	
	Led_Init();
	Relay_Init();
	
	Usart1_Init(115200);//debug Usart
	Usart2_Init(115200);//stm32-ESP8266 Usart
	UsartPrintf(USART_DEBUG,"========================\r\n");
	
	ESP8266_Init();

	// 连接MQTT服务器
	while(MQTT_Client_DevLink()){
		Delay_ms(500);
	}
	// 根据枚举类型，订阅不同的topic
	MQTT_Client_Subscribe(Dev_Att_Rep);
	
	MQTT_Client_SendHeartbeat();
	
	OLED_ShowString(1, 1, "Init_Finish");
	Beep_ShortBeep(200);
	OLED_Clear();
	
  UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");

	// MQ2_Calibrate();
  while (1)
  {
		if(timeCount % 40 == 0)//1000ms / 25 = 40
		{
			DHT11_ReadData(&temp, &humi);
			
			uint16_t gasAna = MQ2_GetAnalogAverage(ADC_Channel_1, 10);
			gasPPM = (uint16_t)MQ2_GetGasPPM();  // 驱动内已限幅 0~10000
			gasDig = MQ2_GetDigital();
			
			ldrDig=LDR_GetDigital();
			uint8_t ldrAna = LDR_GetAnalogAverage();
			ldrPer = LDR_GetPercentage();
			
			// 读取数字量输出
			flameDig = Flame_GetDigital();
			// 读取模拟量原始值（平均后）
			uint8_t flameAna = Flame_GetAnalogAverage();
			// 读取强度百分比
			flamePer = Flame_GetPercentage();
			
		  Delay_ms(1000);
			if(humi < 60 && temp < 50 && gasPPM < 20){
				Beep_Off();
			}
			else {
				Beep_On();
			}
			fan = Relay_GetState();
			alarmFlag = Beep_GetState();
			led = Led_GetState();
			UsartPrintf(USART_DEBUG,"temp=%d, humi=%d, gasPPM=%d, gasAna=%d,gasDig=%d, alarm=%d, fan=%d, led = %d, ldrDig=%d,ldrAna=%d,ldrPer=%d, flameDig=%d, flameAna=%d, flamePer=%d\r\n", 
																temp,humi,gasPPM,gasAna,gasDig,alarmFlag, fan, led, ldrDig, ldrAna, ldrPer, flameDig, flameAna, flamePer);
		}
		if(++timeCount >= 200)	// 5000ms / 25 = 200
		{
			OLED_Clear();
			UsartPrintf(USART_DEBUG, "MQTT_Client_Publish\r\n");
			MQTT_Client_SendData();
			timeCount = 0;
			ESP8266_Clear();
		}

		//OLED_Clear();
		OLED_ShowString(1, 1, "Compoter Room!");
		OLED_ShowString(2, 1, "temp:");
		OLED_ShowString(2, 9, "humi:");
		OLED_ShowString(3, 1, "gas:");
		OLED_ShowString(3, 9, "flame:");
		OLED_ShowString(4, 1, "ldr:");
		OLED_ShowNum(2, 6,temp,2);
		OLED_ShowNum(2, 14,humi,2);
		OLED_ShowNum(3, 5, gasPPM, 4);
		OLED_ShowNum(3, 15,flameDig,1);
		OLED_ShowNum(4, 5,ldrDig,1);

		// 发送MQTT消息到服务器
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL) {
				MQTT_Client_RevPro(dataPtr);
		}
		Delay_ms(10);
	}
}
