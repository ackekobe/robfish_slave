#ifndef __ADC_H
#define __ADC_H

#include "stm32f4xx_conf.h"
#include "sys.h"
#include "rtthread.h"
#include "delay.h"

void adc_Init(void);
u16 Get_Adc_Average(u8 ch,u8 times);


#endif


