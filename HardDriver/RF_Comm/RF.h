#ifndef __RF_H
#define __RF_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "sys.h"
#include "rtthread.h"
#include "delay.h"

#define  RF_LEN			(64)

#define  RF_CS			(PCout(3))

extern u8 RF_Rev_Buf[RF_LEN];
extern u8 RF_Send_Buf[RF_LEN];

void RF_Init(void);
void AHRS_Tx_Enable(u8 *data, u16 ndtr);


#endif



