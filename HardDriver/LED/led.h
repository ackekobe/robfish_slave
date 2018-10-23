#ifndef __LED_H
#define __LED_H
#include "sys.h"



//LED端口定义

#define LED1 PGout(10)		
#define LED2 PGout(11)	
#define LED3 PGout(12)	
#define LED4 PGout(13)	
#define LED5 PGout(14)	


void LED_Init(void);//初始化		 				    
#endif


