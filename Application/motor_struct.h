#ifndef  _MOTOR_STRUCT_H
#define  _MOTOR_STRUCT_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"


typedef struct				//运动电机参数
{
	u8 		run_mode;			//运行模式
	s8 		flap_amplitude;		//拍动幅度
	s16 	target_location;	//目标位置
	float 	flap_fre;			//拍动频率
	
} _move_motor;

typedef struct				//油泵电机参数
{
	u8 	run_mode;			//运行模式
	u16 target_location;	//目标位置
	u16 target_speed;		//目标速度
} _oil_pumb_motor;

typedef struct				//重心电机参数
{
	u8 	run_mode;			//运行模式
	u16 target_speed;		//目标速度
	float target_location;	//目标位置
} _gravity_motor;



#endif




