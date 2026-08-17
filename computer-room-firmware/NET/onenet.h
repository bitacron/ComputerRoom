#ifndef _ONENET_H_
#define _ONENET_H_

#include "stm32f10x.h"

#define	Dev_Rro_Set  0 //设备属性上报
#define Dev_Att_Acq  1  //设备属性获取




_Bool OneNet_DevLink(void);
void OneNet_SendData(void);
void OneNet_RevPro(unsigned char *cmd);

//void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt);
//void OneNet_Publish(const char *topic, const char *msg);
void OneNET_Subscribe(u8 Sub);
#endif
