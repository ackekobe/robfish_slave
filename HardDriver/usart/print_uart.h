#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define	UART4_BUF_SIZE		(10)


void uart4_init(u32 bound);
#endif


