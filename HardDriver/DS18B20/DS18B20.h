#ifndef  __DS18B20_H
#define  __DS18B20_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "sys.h"
#include "rtthread.h"
#include "delay.h"
#include <rthw.h>

#define DQ_OUT		(PEout(11))
#define DQ_IN		(PEin(11))

#define MaxSensorNum			(4)

extern u8 DS18B20_ID[MaxSensorNum][8];
extern u8 DS18B20_SensorNum;


/***DS18B20µÄÖ¸ÁîÂë***/
#define  Search_ROM				(0xF0)
#define  Read_ROM				(0x33)
#define  Match_ROM				(0x55)
#define  Ignore_ROM				(0xCC)
#define  Alarm_Search_ROM		(0xEC)

#define  Temp_Trans				(0x44)
#define  Write_Reg				(0x4E)
#define  Read_Reg				(0xBE)
#define  Copy_Reg				(0x48)
#define  Recall_EEPROM			(0xB8)
#define  Read_Power_Mode		(0xB4)


u8 DS18B20_Init(void);
u8 DS18B20_Search_Rom(void);
float DS18B20_Get_Temp(u8 senor_index);
float DS18B20_ReadTemper(void);
u8 DS18B20_Search_Rom(void);

#endif


