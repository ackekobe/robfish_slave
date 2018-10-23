#ifndef		__LEAK_H
#define 	__LEAK_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "sys.h"
#include "rtthread.h"
#include "delay.h"

extern u8 leak_stat;

void leak_Init(void);

#endif


