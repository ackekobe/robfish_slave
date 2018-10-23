#ifndef __CAN2_H
#define __CAN2_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "sys.h"
#include "rtthread.h"
#include "delay.h"
#include <rthw.h>

void CAN2_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);
u8 CAN2_Send_Msg(u8* msg,u8 len, u8 stdid, u8 extid, u8 ide, u8 rtr);
u8 CAN2_Receive_Msg(u8 *buf);


#endif
