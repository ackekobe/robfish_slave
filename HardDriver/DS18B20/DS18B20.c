/**************************************************

文件名：DS18B20.c
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：鱼舱内温度检测模块驱动
修改记录：

**************************************************/

#include "DS18B20.h"
#include <stdlib.h>

u8 DS18B20_ID[MaxSensorNum][8];
u8 DS18B20_SensorNum;


/**************************************************

函数名：DS18B20_Init
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：温度采集管脚初始化函数
参  数：
修改记录：

**************************************************/

static void DS18B20_DQ_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟

	//GPIOE初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//25MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;		//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);				//初始化GPIO
}


/**************************************************

函数名：DS18B20_Mode_IN
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：温度采集管脚设置为输入
参  数：
修改记录：

**************************************************/

static void DS18B20_Mode_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//GPIOE初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//普通输入模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//25MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);				//初始化GPIO
}

/**************************************************

函数名：DS18B20_Mode_OUT
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：温度采集管脚设置为输出
参  数：
修改记录：

**************************************************/

static void DS18B20_Mode_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//GPIOE初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//25MHz
	GPIO_Init(GPIOE, &GPIO_InitStructure);				//初始化GPIO
}


/**************************************************

函数名：DS18B20_Rst
作  者：刘晓东
日  期：2018.8.20
版  本：V1.00
说  明：主机给从机发送复位脉冲
参  数：
修改记录：

**************************************************/
static void DS18B20_Rst(void)
{
	
	DS18B20_Mode_OUT();
	DQ_OUT = 0;			//主机拉低总线
	delay_us(500);		//产生至少480us的延时
	DQ_OUT = 1;			//释放总线
	delay_us(15);		//等待总线被DS18B20拉低
	
}

/**************************************************

函数名：DS18B20_Answer_Check
作  者：刘晓东
日  期：2018.8.20
版  本：V1.00
说  明：从机给主机的应答脉冲
参  数：
修改记录：

**************************************************/
static u8 DS18B20_Answer_Check(void)
{
	u16 delay = 0;
	DS18B20_Mode_IN();		//设置为上拉输入
	
	while(DQ_IN && delay < 100)			//100us内从机有应答脉冲
	{
		++delay;
		delay_us(1);
	}
	if(delay >= 100)
		return RT_ERROR;
	else
		delay = 0;
	
	while(!DQ_IN && delay <240)
	{
		++delay;
		delay_us(1);
	}
	if(delay >= 240)
		return RT_ERROR;
	return RT_EOK;
}


/**************************************************

函数名：DS18B20_Read_Bit
作  者：刘晓东
日  期：2018.8.20
版  本：V1.00
说  明：从DS18B20读取1位
参  数：
修改记录：

**************************************************/
static u8 DS18B20_Read_Bit(void)
{
	u8 data;
	rt_base_t level;
	DS18B20_Mode_OUT();

	level = rt_hw_interrupt_disable();				//屏蔽中断
	DQ_OUT = 0; // 读时间的起始：必须由主机产生 >1us <15us 的低电平信号
	delay_us(2);
	DQ_OUT = 1;
	delay_us(12);
	DS18B20_Mode_IN();// 设置成输入，释放总线，由外部上拉电阻将总线拉高
	if (DQ_IN)
		data = 1;
	else
		data = 0;
	delay_us(50);
	rt_hw_interrupt_enable(level);					//打开中断
	return data;
}

/**************************************************

函数名：DS18B20_Read_2Bit
作  者：刘晓东
日  期：2018.8.20
版  本：V1.00
说  明：从DS18B20读取2位
参  数：
修改记录：

**************************************************/
static u8 DS18B20_Read_2Bit(void)
{
	u8 i;
	u8 dat = 0;
	rt_base_t level;
	
	level = rt_hw_interrupt_disable();				//屏蔽中断
	for (i = 2; i > 0; i--)
	{
		dat = dat << 1;
		DS18B20_Mode_OUT();
		DQ_OUT = 0;
		delay_us(2);
		DQ_OUT = 1;
		DS18B20_Mode_IN();
		delay_us(12);
		if (DQ_IN)	dat |= 0x01;
		delay_us(50);
	}
	rt_hw_interrupt_enable(level);					//打开中断
	return dat;
}


/**************************************************

函数名：DS18B20_Read_Byte
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：温度采集读数据
参  数：
修改记录：

**************************************************/
static u8 DS18B20_Read_Byte(void)	
{
	u8 i, j, dat;
	dat = 0;
	for (i = 0; i < 8; i++)
	{
		j = DS18B20_Read_Bit();
		dat = (dat) | (j << i);
	}
	return dat;
}

/**************************************************

函数名：DS18B20_Write_Bit
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：写1位数据到DS18B20
参  数：
修改记录：

**************************************************/
static void DS18B20_Write_Bit(u8 dat)
{
	rt_base_t level;
	
	level = rt_hw_interrupt_disable();				//屏蔽中断
	DS18B20_Mode_OUT();
	if (dat)
	{
		DQ_OUT = 0;// Write 1
		delay_us(2);
		DQ_OUT = 1;
		delay_us(60);
	}
	else
	{
		DQ_OUT = 0;// Write 0
		delay_us(60);
		DQ_OUT = 1;
		delay_us(2);
	}
	rt_hw_interrupt_enable(level);					//打开中断
}

/**************************************************

函数名：DS18B20_write_byte
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：温度采集写数据
参  数：dat	 要写的数据
修改记录：

**************************************************/

static void DS18B20_Write_Byte(u8 dat)
{	
	u8 j;
	u8 testb;
	rt_base_t level;
	
	DS18B20_Mode_OUT();
	level = rt_hw_interrupt_disable();				//屏蔽中断
	for (j = 0; j < 8; j++)
	{
		testb = dat & 0x01;
		dat = dat >> 1;
		if (testb)
		{
			DQ_OUT = 0;				// 写1
			delay_us(10);
			DQ_OUT = 1;
			delay_us(50);
		}
		else
		{
			DQ_OUT = 0;				// 写0
			delay_us(60);
			DQ_OUT = 1;				// 释放总线
			delay_us(2);
		}
	}
	rt_hw_interrupt_enable(level);					//打开中断
}

/**************************************************

函数名：DS18B20_Init
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：DS18B20初始化
参  数：
修改记录：

**************************************************/
u8 DS18B20_Init(void)
{
	DS18B20_DQ_Config();
	DS18B20_Rst();
	return DS18B20_Answer_Check();
}


/**************************************************

函数名：DS18B20_ReadTemper
作  者：刘晓东
日  期：2018.8.20
版  本：V1.00
说  明：从一个DS18B20得到温度数据
参  数：
修改记录：

**************************************************/
float DS18B20_ReadTemper(void)
{
	u8 th, tl;
	u16 Temperature = 0;
	float Temperature1 = 0.0;
	rt_base_t level;
	
	level = rt_hw_interrupt_disable();				//屏蔽中断  复位过程不能打断
	DS18B20_Rst();
	if(DS18B20_Answer_Check() == RT_ERROR)	
	{
		return RT_ERROR;					//返回错误
	}
	rt_hw_interrupt_enable(level);					//打开中断
 
	DS18B20_Write_Byte(Ignore_ROM);				//跳过读序列号
	DS18B20_Write_Byte(Temp_Trans);				//启动温度转换
	
	level = rt_hw_interrupt_disable();				//屏蔽中断
	DS18B20_Rst();
	if(DS18B20_Answer_Check() == RT_ERROR)	
	{
		return RT_ERROR;						//返回错误
	}
	rt_hw_interrupt_enable(level);					//打开中断
	
	DS18B20_Write_Byte(Ignore_ROM);				//跳过读序列号
	DS18B20_Write_Byte(Read_Reg);				//读取温度
	tl = DS18B20_Read_Byte();					//读取低八位
	th = DS18B20_Read_Byte(); 					//读取高八位
	if ((th & 0xF8) == 0xF8)					//零下温度转换
	{
		Temperature = (th << 8) | tl;
		Temperature1 = (~Temperature) + 1;
		Temperature1 *= (float)0.0625;
	}
	else										//零上温度转换
	{
		Temperature1 = ((float)th * 256 + tl) * (float)0.0625;
	}
	return Temperature1;
}


/**************************************************

函数名：DS18B20_Get_Temp
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：从特定DS18B20得到温度数据
参  数：senor_index  DS18B20索引号
修改记录：

**************************************************/
float DS18B20_Get_Temp(u8 senor_index)
{
	u8 cnt;								//匹配的字节
	u8 TL, TH;							//温度数据
	u16 Temperature;
	float Temperature1;
	rt_base_t level;
	
	level = rt_hw_interrupt_disable();				//屏蔽中断
	DS18B20_Rst();
	DS18B20_Answer_Check();
	DS18B20_Write_Byte(Ignore_ROM);		//跳过ROM
	DS18B20_Write_Byte(Temp_Trans);		//温度转换
	DS18B20_Rst();
	DS18B20_Answer_Check();
	rt_hw_interrupt_enable(level);					//打开中断
 
	
	DS18B20_Write_Byte(Match_ROM);
	for (cnt = 0; cnt < 8; cnt++)
	{
		DS18B20_Write_Byte(DS18B20_ID[senor_index][cnt]);
	}
	delay_us(10);
	DS18B20_Write_Byte(Read_Reg);		//读取温度数据
	TL = DS18B20_Read_Byte(); 			// LSB   
	TH = DS18B20_Read_Byte();			// MSB  
	if ((TH & 0xF8) == 0xF8)			//零下温度转换
	{
		Temperature = (TH << 8) | TL;
		Temperature1 = (~Temperature) + 1;
		Temperature1 *= (float)0.0625;
	}
	else								//零上温度转换
	{
		Temperature1 = ((TH << 8) | TL)*0.0625;
	}
	return Temperature1;
}

/**************************************************

函数名：DS18B20_Search_Rom
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：搜索总线上的DS18B20的数目
参  数：
修改记录：

**************************************************/
u8 DS18B20_Search_Rom(void)
{
	u8 k, l, conflict_bit, m, n, num;
	u8 stack[MaxSensorNum + 1] = {0x00};
	u8 ss[64];
	u8 tempp;
	l = 0;
	num = 0;
	do
	{
		DS18B20_Rst(); 				//注意：复位的延时不够
		delay_us(480); 				//480、720
		DS18B20_Write_Byte(Search_ROM);
		for (m = 0; m < 8; m++)
		{
			u8 s = 0;
			for (n = 0; n < 8; n++)
			{
				k = DS18B20_Read_2Bit();//读两位数据
 
				k = k & 0x03;
				s >>= 1;
				if (k == 0x01)//01读到的数据为0 写0 此位为0的器件响应
				{
					DS18B20_Write_Bit(0);
					ss[(m * 8 + n)] = 0;
				}
				else if (k == 0x02)//读到的数据为1 写1 此位为1的器件响应
				{
					s = s | 0x80;
					DS18B20_Write_Bit(1);
					ss[(m * 8 + n)] = 1;
				}
				else if (k == 0x00)						//读到的数据为00，有冲突位，判断冲突位，如果冲突位大于栈顶写0，小于栈顶写以前数据 等于栈顶写1
				{
					conflict_bit = m * 8 + n + 1;
					if (conflict_bit > stack[l])
					{
						DS18B20_Write_Bit(0);
						ss[(m * 8 + n)] = 0;
						stack[++l] = conflict_bit;
					}
					else if (conflict_bit < stack[l])
					{
						s = s | ((ss[(m * 8 + n)] & 0x01) << 7);
						DS18B20_Write_Bit(ss[(m * 8 + n)]);
					}
					else if (conflict_bit == stack[l])
					{
						s = s | 0x80;
						DS18B20_Write_Bit(1);
						ss[(m * 8 + n)] = 1;
						l = l - 1;
					}
				}
				else
				{
					return RT_ERROR;
				}
			}
			tempp = s;
			DS18B20_ID[num][m] = tempp; 		// 保存搜索到的ID
		}
		++num;									// 保存搜索到的个数
	} while (stack[l] != 0 && (num < MaxSensorNum));
	DS18B20_SensorNum = num;
	return RT_EOK;
}






