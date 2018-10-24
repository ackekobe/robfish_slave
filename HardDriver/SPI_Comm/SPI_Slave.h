#ifndef  __SPI_SLAVE_H
#define  __SPI_SLAVE_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "sys.h"
#include "rtthread.h"
#include "delay.h"
#include <rthw.h>
#include "CRC8.h"
#include "motor_struct.h"
#include "wdg.h"

#define 	SPI_DATA_LEN	(32)

#define 	Comm_BUSY		(1)		//SPI通信状态
#define		Comm_IDLE		(0)		//SPI空闲状态


#define 	Time_Len			(13)
#define 	Slave_Stat_Len		(28)
#define 	Fish_Stat_Len		(38)

typedef enum _cmd				//主从机通信命令码
{
	XTZJ = 0x01,				//系统自检
	ZTSB = 0x02,				//状态上报
	XTTZ = 0x03,				//系统停止
	XTJS = 0x04,				//系统校时
	XTFW = 0x05,				//系统复位
	
	YTSF = 0x10,				//鱼体上浮
	YTXQ = 0x11,				//鱼体下潜
	YTZZ = 0x12,				//鱼体左转
	YTYZ = 0x13,				//鱼体右转
	YTHT = 0x14,				//鱼体后退
	YTHX = 0x15,				//鱼体滑翔
	
	HLGH = 0x20,				//航路规划
#ifdef  __DEBUG
	DJXQL  = 0x30,				//左胸鳍电机
	DJXQR  = 0x31,				//右胸鳍电机
	DJZX   = 0x32,				//重心电机
	DJYB   = 0x33,				//腰部电机
	DJWB   = 0x34,				//尾部电机
	DJYG   = 0x35,				//油缸电机
#endif
} cmd;

typedef struct						//从机运行状态
{
	u8 		motor1_2;
	u8 		motor3_4;
	u8 		motor5_6;
	u8		power_vol;
	u8		power_cur;
	u8		power_tem;
	u8		cabin1_tem;
	u8		cabin2_tem;
	u8		cabin3_tem;
	u8		cabin4_tem;
	u16 	total_stat;
	u32 	slave_fault;
	float	power_quantity;
	float	power_fault;
} _slave_stat;


extern u8 Pose_Trans;
extern u8 run_posture[Fish_Stat_Len];

extern u8 SPI_Comm_Stat;
extern u8 Slave_Stat_Buf[SPI_DATA_LEN];
extern _slave_stat slave_stat;

u8 SPI_Slave_WriteByte(u8 TxData);
void SPI_Slave_Init(void);
void SPI_Slave_TIMode_Init(void);
u8 Slave_ReadByte(void);

#endif

