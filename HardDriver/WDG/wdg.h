#ifndef __WDG_H
#define __WDG_H


#include "stm32f4xx_conf.h"
#include "sys.h"
#include "rtthread.h"
#include "delay.h"



#define 	WDI				(PDout(11))
#define		WDG_ENABLE		(0x00)
#define		WDG_DISABLE		(0x55)

extern u8 Feed_Flag;


void WDG_Init(void);
void WDG_Feed(void);

#endif


