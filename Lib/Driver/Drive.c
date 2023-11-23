#include "Drive.h"
#include "stdint.h"
#include "adc.h"

uint32_t ADCValueDMA[AIN_COUNT] = {0};//此数组仅用于ADC_DMA使用，数据在ADC中断里用消息发给其他线程

void FOC_Iint()
{
	//初始化ADC
	
	//初始化ADC
	if(HAL_ADC_Start_DMA(&hadc1,ADCValueDMA, AIN_COUNT) != HAL_OK)//开启ADC的DMA，采集的数据直接放入
  {
    /* Start Conversation Error */
    Error_Handler(); 
  } 
	
	
}