#ifndef __DRIVER_H
#define __DRIVER_H
#include "stdint.h"

#define AIN_COUNT								(3)

extern uint32_t ADCValueDMA[AIN_COUNT] ;//�����������ADC_DMAʹ�ã�������ADC�ж�������Ϣ���������߳�

void FOC_Iint(void);	
#endif

