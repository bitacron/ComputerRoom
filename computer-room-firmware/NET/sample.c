#include "mqttkit.h"


unsigned char dataMem[128];		//全局数组方式


/*******************************************************************************
* 函数名：OneNet_DevLink
* 描述  ：与onenet创建连接。与onenet平台建立连接，成功或会标记oneNetInfo.netWork网络状态标志
* 输入  ：devid：创建设备的devid proid：产品ID auth_key：创建设备的masterKey或apiKey或设备鉴权信息
* 输出  ：无
*******************************************************************************/
void OneNet_DevLink(const char* devid, const char *proid, const char* auth_info)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	
	edpPacket._data = dataMem;				//全局数组方式
	edpPacket._size = sizeof(dataMem);		//全局数组方式
	

	if(MQTT_PacketConnect(proid, auth_info, devid, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{

		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);


		if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
		{

			switch(MQTT_UnPacketConnectAck(dataPtr))
			{
				case 0:
					UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");
					oneNetInfo.netWork = 1;
				break;
				
				case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
				case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法的clientid\r\n");break;
				case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
				case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户名或密码错误\r\n");break;
				case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
				
				default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
			}
		}
		

		MQTT_DeleteBuffer(&mqttPacket);
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
}

/*******************************************************************************
* 函数名：OneNet_DisConnect
* 描述  ：与平台断开连接
* 输入  ：无
* 输出  ：0-成功 1-失败
*******************************************************************************/
_Bool OneNet_DisConnect(void)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	unsigned char dataMem1[8];				//局部数组方式
	
	edpPacket._data = dataMem;				//局部数组方式
	edpPacket._size = sizeof(dataMem);		//局部数组方式
	

	if(MQTT_PacketDisConnect(&mqttPacket) == 0)
	{

		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
		

		MQTT_DeleteBuffer(&mqttPacket);
	}
	
	return 0;

}

/*******************************************************************************
* 函数名：OneNet_SendData
* 描述  ：上传数据到平台
* 输入  ：type：发送数据的格式 devid：设备ID apikey：设备apikey streamArray：数据流 streamArrayNum：数据流个数
* 输出  ：0-成功 1-失败
*******************************************************************************/
_Bool OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	
	_Bool status = 0;
	short body_len = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT_TYPE%d\r\n", type);
	

	body_len = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt, 0);
	if(body_len)
	{

		if(MQTT_PacketSaveData(devid, body_len, NULL, (uint8)type, &mqttPacket) == 0)
		{

			body_len = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, mqttPacket._data, mqttPacket._size, mqttPacket._len);
			
			if(body_len)
			{
				mqttPacket._len += body_len;
				UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);

				NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
			}
			else
				UsartPrintf(USART_DEBUG, "WARN:	DSTREAM_GetDataStream_Body Failed\r\n");
			

			MQTT_DeleteBuffer(&mqttPacket);
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	MQTT_NewBuffer Failed\r\n");
	}
	else
		status = 1;
	
	return status;
	
}

/*******************************************************************************
* 函数名：OneNet_HeartBeat
* 描述  ：心跳检测
* 输入  ：无
* 输出  ：无
*******************************************************************************/
void OneNet_HeartBeat(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};

	unsigned char sCount = 3;
	

	if(MQTT_PacketPing(&mqttPacket))
		return;
	
	while(sCount--)
	{

		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);


		if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_PINGRESP)
		{
			UsartPrintf(USART_DEBUG, "Tips:	HeartBeat OK\r\n");
			
			break;
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Check Device\r\n");
		}
		
		RTOS_TimeDly(2);
	}
	

	MQTT_DeleteBuffer(&mqttPacket);

}

/*******************************************************************************
* 函数名：OneNet_Publish
* 描述  ：发布消息
* 输入  ：topic：发布的主题 msg：消息内容
* 输出  ：0-成功 1-失败
*******************************************************************************/
_Bool OneNet_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	
	UsartPrintf(USART_DEBUG, "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	

	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL2, 0, 1, &mqttPacket) == 0)
	{

		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
		

		MQTT_DeleteBuffer(&mqttPacket);
	}
	
	return 0;

}

/*******************************************************************************
* 函数名：OneNet_Subscribe
* 描述  ：订阅
* 输入  ：topics：订阅的topic topic_cnt：topic个数
* 输出  ：0-成功 1-失败
*******************************************************************************/
_Bool OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	for(; i < topic_cnt; i++)
		UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topics[i]);
	

	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topics, topic_cnt, &mqttPacket) == 0)
	{

		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		

		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}
	
	return 0;

}

/*******************************************************************************
* 函数名：OneNet_UnSubscribe
* 描述  ：取消订阅
* 输入  ：topics：订阅的topic topic_cnt：topic个数
* 输出  ：0-成功 1-失败
*******************************************************************************/
_Bool OneNet_UnSubscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	for(; i < topic_cnt; i++)
		UsartPrintf(USART_DEBUG, "UnSubscribe Topic: %s\r\n", topics[i]);
	

	if(MQTT_PacketUnSubscribe(MQTT_UNSUBSCRIBE_ID, topics, topic_cnt, &mqttPacket) == 0)
	{

		NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送取消订阅请求
		

		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}
	
	return 0;

}

/*******************************************************************************
* 函数名：OneNet_RevPro
* 描述  ：平台返回数据检测
* 输入  ：dataPtr：平台返回的数据
* 输出  ：无
*******************************************************************************/
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = -1;
	

	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{

		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload);				//解出topic和消息体
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s\r\n", cmdid_topic, req_payload);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");
					
					NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);			//回复命令
					MQTT_DeleteBuffer(&mqttPacket);									//删包
				}
			}
		
		break;
			
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &req_payload, &qos, &pkt_id);
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "topic: %s\r\npayload: %s\r\n", cmdid_topic, req_payload);
				
				switch(qos)
				{
					case 1:															//收到publish的qos为1，设备需要回复Ack
					
						if(MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
						{
							UsartPrintf(USART_DEBUG, "Tips:	Send PublishAck\r\n");
							NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					case 2:															//收到publish的qos为2，设备先回复Rec
																					//平台回复Rel，设备再回复Comp
						if(MQTT_PacketPublishRec(pkt_id, &mqttPacket) == 0)
						{
							UsartPrintf(USART_DEBUG, "Tips:	Send PublishRec\r\n");
							NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					default:
						break;
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
		break;
			
		case MQTT_PKT_PUBREC:														//发送Publish消息，平台回复的Rec，设备需回复Rel消息
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Rev PublishRec\r\n");
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send PublishRel\r\n");
					NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														//收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp
			
			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Rev PublishRel\r\n");
				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send PublishComp\r\n");
					NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
		
		case MQTT_PKT_PUBCOMP:														//发送Publish消息，平台返回Rec，设备回复Rel，平台再返回的Comp
		
			if(MQTT_UnPacketPublishComp(cmd) == 0)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Rev PublishComp\r\n");
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe OK\r\n");
			else
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe Err\r\n");
		
		break;
			
		case MQTT_PKT_UNSUBACK:														//发送UnSubscribe消息的Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT UnSubscribe OK\r\n");
			else
				UsartPrintf(USART_DEBUG, "Tips:	MQTT UnSubscribe Err\r\n");
		
		break;
	}
	

	if(req)
	{
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
