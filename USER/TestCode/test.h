#ifndef 	__TEST_H
#define 	__TEST_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "sys.h"
#include "rtthread.h"
#include "delay.h"
#include <rthw.h>
#include "DS18B20.h"
#include "RF.h"

#define TEST_FLAG		(1)				//使用测试程序宏定义

#if TEST_FLAG

#define delay_test_flag		(PCout(4))

void LED_Test(void);
void leak_Test(u8 index);
void delay_test(void);
float DS18B20_test(void);

#endif


#endif



