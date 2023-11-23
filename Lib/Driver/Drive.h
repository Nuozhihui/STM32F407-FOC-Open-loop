#ifndef __DRIVER_H
#define __DRIVER_H
#include "stdint.h"

#define AIN_COUNT								(3)

extern uint32_t ADCValueDMA[AIN_COUNT] ;//此数组仅用于ADC_DMA使用，数据在ADC中断里用消息发给其他线程

void FOC_Iint(void);	
#endif

