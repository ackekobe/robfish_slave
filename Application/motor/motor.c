/**************************************************

�ļ�����motor.c
��  �ߣ�������
��  �ڣ�2018.8.27
��  ����V1.00
˵  �����������ļ�
�޸ļ�¼��

**************************************************/

#include "motor.h"

_move_motor  left_pectoral;				//����������
_move_motor	 right_pectoral;			//������
_oil_pumb_motor  oil_pumb_motor;		//�ͱõ������
_gravity_motor 	 gravity_motor;			//���ĵ������
_move_motor	 waist_motor;				//�����������	
_move_motor		 tail_motor;			//β���������


/**************************************************

��������fish_posure_analy
��  �ߣ�������
��  �ڣ�2018.9.5
��  ����V1.00
˵  ���������·���������̬����
��  ����
�޸ļ�¼��

**************************************************/
u8 fish_posure_analy(u8 *buf)
{
	if(buf[0] < YTSF || buf[0] > DJYG)
		return RT_ERROR;
	if(buf[Fish_Stat_Len-1] != 0xEE)
		return RT_ERROR;
	if(CRC8_Table(buf, 36) != buf[36])
		return RT_ERROR;
	left_pectoral.run_mode = buf[2];
	left_pectoral.flap_amplitude = buf[3];
	left_pectoral.flap_fre = (float)buf[5]/100 + buf[4];
	left_pectoral.target_location = (s16)((buf[6]<<8) + buf[7]);
	right_pectoral.run_mode = buf[8];
	right_pectoral.flap_amplitude = buf[9];
	right_pectoral.flap_fre = (float)buf[11]/100 + buf[10];
	right_pectoral.target_location = (s16)((buf[12]<<8) + buf[13]);
	oil_pumb_motor.run_mode = buf[14];
	oil_pumb_motor.target_location = (u16)((buf[15]<<8) + buf[16]);
	oil_pumb_motor.target_speed = (u16)((buf[17]<<8) + buf[18]);
	gravity_motor.run_mode = buf[19];
	gravity_motor.target_location = (float)(((u32)buf[20]<<24) +((u32)buf[21]<<16)+ ((u32)buf[22]<<8) + buf[23])/1000;
	gravity_motor.target_speed = (u16)((buf[24]<<8) + buf[25]);
	waist_motor.run_mode = buf[26];
	waist_motor.flap_amplitude = buf[27];
	waist_motor.flap_fre = (float)buf[28]/100 + buf[29];
	waist_motor.target_location = (s8)buf[30];
	tail_motor.run_mode = buf[31];
	tail_motor.flap_amplitude = buf[32];
	tail_motor.flap_fre = (float)buf[33]/100 + buf[34];
	tail_motor.target_location = (s8)buf[25];
	return RT_EOK;
}





