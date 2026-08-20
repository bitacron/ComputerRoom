#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

void TIM2_Int_Init(u16 arr, u16 psc);
void TIM3_Int_Init(u16 arr, u16 psc);

/** 系统毫秒计数（TIM3 1ms 节拍），无符号差值可安全处理溢出 */
uint32_t millis(void);

#endif
