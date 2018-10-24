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

#define 	Comm_BUSY		(1)		//SPIͨ��״̬
#define		Comm_IDLE		(0)		//SPI����״̬


#define 	Time_Len			(13)
#define 	Slave_Stat_Len		(28)
#define 	Fish_Stat_Len		(38)

typedef enum _cmd				//���ӻ�ͨ��������
{
	XTZJ = 0x01,				//ϵͳ�Լ�
	ZTSB = 0x02,				//״̬�ϱ�
	XTTZ = 0x03,				//ϵͳֹͣ
	XTJS = 0x04,				//ϵͳУʱ
	XTFW = 0x05,				//ϵͳ��λ
	
	YTSF = 0x10,				//�����ϸ�
	YTXQ = 0x11,				//������Ǳ
	YTZZ = 0x12,				//������ת
	YTYZ = 0x13,				//������ת
	YTHT = 0x14,				//�������
	YTHX = 0x15,				//���廬��
	
	HLGH = 0x20,				//��·�滮
#ifdef  __DEBUG
	DJXQL  = 0x30,				//���������
	DJXQR  = 0x31,				//���������
	DJZX   = 0x32,				//���ĵ��
	DJYB   = 0x33,				//�������
	DJWB   = 0x34,				//β�����
	DJYG   = 0x35,				//�͸׵��
#endif
} cmd;

typedef struct						//�ӻ�����״̬
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

