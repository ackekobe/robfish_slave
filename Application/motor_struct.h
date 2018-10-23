#ifndef  _MOTOR_STRUCT_H
#define  _MOTOR_STRUCT_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"


typedef struct				//�˶��������
{
	u8 		run_mode;			//����ģʽ
	s8 		flap_amplitude;		//�Ķ�����
	s16 	target_location;	//Ŀ��λ��
	float 	flap_fre;			//�Ķ�Ƶ��
	
} _move_motor;

typedef struct				//�ͱõ������
{
	u8 	run_mode;			//����ģʽ
	u16 target_location;	//Ŀ��λ��
	u16 target_speed;		//Ŀ���ٶ�
} _oil_pumb_motor;

typedef struct				//���ĵ������
{
	u8 	run_mode;			//����ģʽ
	u16 target_speed;		//Ŀ���ٶ�
	float target_location;	//Ŀ��λ��
} _gravity_motor;



#endif




