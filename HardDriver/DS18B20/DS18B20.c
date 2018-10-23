/**************************************************

�ļ�����DS18B20.c
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  ����������¶ȼ��ģ������
�޸ļ�¼��

**************************************************/

#include "DS18B20.h"
#include <stdlib.h>

u8 DS18B20_ID[MaxSensorNum][8];
u8 DS18B20_SensorNum;


/**************************************************

��������DS18B20_Init
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  �����¶Ȳɼ��ܽų�ʼ������
��  ����
�޸ļ�¼��

**************************************************/

static void DS18B20_DQ_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOEʱ��

	//GPIOE��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		//��©���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//25MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;		//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);				//��ʼ��GPIO
}


/**************************************************

��������DS18B20_Mode_IN
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  �����¶Ȳɼ��ܽ�����Ϊ����
��  ����
�޸ļ�¼��

**************************************************/

static void DS18B20_Mode_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//GPIOE��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		//��©���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//25MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);				//��ʼ��GPIO
}

/**************************************************

��������DS18B20_Mode_OUT
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  �����¶Ȳɼ��ܽ�����Ϊ���
��  ����
�޸ļ�¼��

**************************************************/

static void DS18B20_Mode_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//GPIOE��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//25MHz
	GPIO_Init(GPIOE, &GPIO_InitStructure);				//��ʼ��GPIO
}


/**************************************************

��������DS18B20_Rst
��  �ߣ�������
��  �ڣ�2018.8.20
��  ����V1.00
˵  �����������ӻ����͸�λ����
��  ����
�޸ļ�¼��

**************************************************/
static void DS18B20_Rst(void)
{
	
	DS18B20_Mode_OUT();
	DQ_OUT = 0;			//������������
	delay_us(500);		//��������480us����ʱ
	DQ_OUT = 1;			//�ͷ�����
	delay_us(15);		//�ȴ����߱�DS18B20����
	
}

/**************************************************

��������DS18B20_Answer_Check
��  �ߣ�������
��  �ڣ�2018.8.20
��  ����V1.00
˵  �����ӻ���������Ӧ������
��  ����
�޸ļ�¼��

**************************************************/
static u8 DS18B20_Answer_Check(void)
{
	u16 delay = 0;
	DS18B20_Mode_IN();		//����Ϊ��������
	
	while(DQ_IN && delay < 100)			//100us�ڴӻ���Ӧ������
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

��������DS18B20_Read_Bit
��  �ߣ�������
��  �ڣ�2018.8.20
��  ����V1.00
˵  ������DS18B20��ȡ1λ
��  ����
�޸ļ�¼��

**************************************************/
static u8 DS18B20_Read_Bit(void)
{
	u8 data;
	rt_base_t level;
	DS18B20_Mode_OUT();

	level = rt_hw_interrupt_disable();				//�����ж�
	DQ_OUT = 0; // ��ʱ�����ʼ���������������� >1us <15us �ĵ͵�ƽ�ź�
	delay_us(2);
	DQ_OUT = 1;
	delay_us(12);
	DS18B20_Mode_IN();// ���ó����룬�ͷ����ߣ����ⲿ�������轫��������
	if (DQ_IN)
		data = 1;
	else
		data = 0;
	delay_us(50);
	rt_hw_interrupt_enable(level);					//���ж�
	return data;
}

/**************************************************

��������DS18B20_Read_2Bit
��  �ߣ�������
��  �ڣ�2018.8.20
��  ����V1.00
˵  ������DS18B20��ȡ2λ
��  ����
�޸ļ�¼��

**************************************************/
static u8 DS18B20_Read_2Bit(void)
{
	u8 i;
	u8 dat = 0;
	rt_base_t level;
	
	level = rt_hw_interrupt_disable();				//�����ж�
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
	rt_hw_interrupt_enable(level);					//���ж�
	return dat;
}


/**************************************************

��������DS18B20_Read_Byte
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  �����¶Ȳɼ�������
��  ����
�޸ļ�¼��

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

��������DS18B20_Write_Bit
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  ����д1λ���ݵ�DS18B20
��  ����
�޸ļ�¼��

**************************************************/
static void DS18B20_Write_Bit(u8 dat)
{
	rt_base_t level;
	
	level = rt_hw_interrupt_disable();				//�����ж�
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
	rt_hw_interrupt_enable(level);					//���ж�
}

/**************************************************

��������DS18B20_write_byte
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  �����¶Ȳɼ�д����
��  ����dat	 Ҫд������
�޸ļ�¼��

**************************************************/

static void DS18B20_Write_Byte(u8 dat)
{	
	u8 j;
	u8 testb;
	rt_base_t level;
	
	DS18B20_Mode_OUT();
	level = rt_hw_interrupt_disable();				//�����ж�
	for (j = 0; j < 8; j++)
	{
		testb = dat & 0x01;
		dat = dat >> 1;
		if (testb)
		{
			DQ_OUT = 0;				// д1
			delay_us(10);
			DQ_OUT = 1;
			delay_us(50);
		}
		else
		{
			DQ_OUT = 0;				// д0
			delay_us(60);
			DQ_OUT = 1;				// �ͷ�����
			delay_us(2);
		}
	}
	rt_hw_interrupt_enable(level);					//���ж�
}

/**************************************************

��������DS18B20_Init
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  ����DS18B20��ʼ��
��  ����
�޸ļ�¼��

**************************************************/
u8 DS18B20_Init(void)
{
	DS18B20_DQ_Config();
	DS18B20_Rst();
	return DS18B20_Answer_Check();
}


/**************************************************

��������DS18B20_ReadTemper
��  �ߣ�������
��  �ڣ�2018.8.20
��  ����V1.00
˵  ������һ��DS18B20�õ��¶�����
��  ����
�޸ļ�¼��

**************************************************/
float DS18B20_ReadTemper(void)
{
	u8 th, tl;
	u16 Temperature = 0;
	float Temperature1 = 0.0;
	rt_base_t level;
	
	level = rt_hw_interrupt_disable();				//�����ж�  ��λ���̲��ܴ��
	DS18B20_Rst();
	if(DS18B20_Answer_Check() == RT_ERROR)	
	{
		return RT_ERROR;					//���ش���
	}
	rt_hw_interrupt_enable(level);					//���ж�
 
	DS18B20_Write_Byte(Ignore_ROM);				//���������к�
	DS18B20_Write_Byte(Temp_Trans);				//�����¶�ת��
	
	level = rt_hw_interrupt_disable();				//�����ж�
	DS18B20_Rst();
	if(DS18B20_Answer_Check() == RT_ERROR)	
	{
		return RT_ERROR;						//���ش���
	}
	rt_hw_interrupt_enable(level);					//���ж�
	
	DS18B20_Write_Byte(Ignore_ROM);				//���������к�
	DS18B20_Write_Byte(Read_Reg);				//��ȡ�¶�
	tl = DS18B20_Read_Byte();					//��ȡ�Ͱ�λ
	th = DS18B20_Read_Byte(); 					//��ȡ�߰�λ
	if ((th & 0xF8) == 0xF8)					//�����¶�ת��
	{
		Temperature = (th << 8) | tl;
		Temperature1 = (~Temperature) + 1;
		Temperature1 *= (float)0.0625;
	}
	else										//�����¶�ת��
	{
		Temperature1 = ((float)th * 256 + tl) * (float)0.0625;
	}
	return Temperature1;
}


/**************************************************

��������DS18B20_Get_Temp
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  �������ض�DS18B20�õ��¶�����
��  ����senor_index  DS18B20������
�޸ļ�¼��

**************************************************/
float DS18B20_Get_Temp(u8 senor_index)
{
	u8 cnt;								//ƥ����ֽ�
	u8 TL, TH;							//�¶�����
	u16 Temperature;
	float Temperature1;
	rt_base_t level;
	
	level = rt_hw_interrupt_disable();				//�����ж�
	DS18B20_Rst();
	DS18B20_Answer_Check();
	DS18B20_Write_Byte(Ignore_ROM);		//����ROM
	DS18B20_Write_Byte(Temp_Trans);		//�¶�ת��
	DS18B20_Rst();
	DS18B20_Answer_Check();
	rt_hw_interrupt_enable(level);					//���ж�
 
	
	DS18B20_Write_Byte(Match_ROM);
	for (cnt = 0; cnt < 8; cnt++)
	{
		DS18B20_Write_Byte(DS18B20_ID[senor_index][cnt]);
	}
	delay_us(10);
	DS18B20_Write_Byte(Read_Reg);		//��ȡ�¶�����
	TL = DS18B20_Read_Byte(); 			// LSB   
	TH = DS18B20_Read_Byte();			// MSB  
	if ((TH & 0xF8) == 0xF8)			//�����¶�ת��
	{
		Temperature = (TH << 8) | TL;
		Temperature1 = (~Temperature) + 1;
		Temperature1 *= (float)0.0625;
	}
	else								//�����¶�ת��
	{
		Temperature1 = ((TH << 8) | TL)*0.0625;
	}
	return Temperature1;
}

/**************************************************

��������DS18B20_Search_Rom
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  �������������ϵ�DS18B20����Ŀ
��  ����
�޸ļ�¼��

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
		DS18B20_Rst(); 				//ע�⣺��λ����ʱ����
		delay_us(480); 				//480��720
		DS18B20_Write_Byte(Search_ROM);
		for (m = 0; m < 8; m++)
		{
			u8 s = 0;
			for (n = 0; n < 8; n++)
			{
				k = DS18B20_Read_2Bit();//����λ����
 
				k = k & 0x03;
				s >>= 1;
				if (k == 0x01)//01����������Ϊ0 д0 ��λΪ0��������Ӧ
				{
					DS18B20_Write_Bit(0);
					ss[(m * 8 + n)] = 0;
				}
				else if (k == 0x02)//����������Ϊ1 д1 ��λΪ1��������Ӧ
				{
					s = s | 0x80;
					DS18B20_Write_Bit(1);
					ss[(m * 8 + n)] = 1;
				}
				else if (k == 0x00)						//����������Ϊ00���г�ͻλ���жϳ�ͻλ�������ͻλ����ջ��д0��С��ջ��д��ǰ���� ����ջ��д1
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
			DS18B20_ID[num][m] = tempp; 		// ������������ID
		}
		++num;									// �����������ĸ���
	} while (stack[l] != 0 && (num < MaxSensorNum));
	DS18B20_SensorNum = num;
	return RT_EOK;
}






