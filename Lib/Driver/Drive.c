#include "Drive.h"
#include "stdint.h"
#include "adc.h"

uint32_t ADCValueDMA[AIN_COUNT] = {0};//�����������ADC_DMAʹ�ã�������ADC�ж�������Ϣ���������߳�

void FOC_Iint()
{
	//��ʼ��ADC
	
	//��ʼ��ADC
	if(HAL_ADC_Start_DMA(&hadc1,ADCValueDMA, AIN_COUNT) != HAL_OK)//����ADC��DMA���ɼ�������ֱ�ӷ���
  {
    /* Start Conversation Error */
    Error_Handler(); 
  } 
	
	
}