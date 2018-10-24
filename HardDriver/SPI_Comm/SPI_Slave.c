/**************************************************

�ļ�����SPI_Slave.c
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ����SPIͨ�Ŵӻ�����
�޸ļ�¼��

**************************************************/
#include "SPI_Slave.h"





u8 SPI_Data_Cnt = 0;						//�ӻ�SPI�շ����ݼ���
u8 SPI_Comm_Stat = Comm_IDLE;						//SPIͨ��״̬��־
u8 Master_CMD = 0;									//�������͵�����������
u8 System_Check = 0;								//ϵͳ�Լ��־λ
u8 System_timing = 0;								//ϵͳУʱ��־λ
u8 System_Stop = 0;									//ϵͳ���ֹͣλ
u8 Pose_Trans = 0;									//����̬ת��

u8 time_rev_buf[Time_Len];								//Уʱ���ջ���
u8 run_posture[Fish_Stat_Len];									//������̬����
u8 Slave_Stat_Buf[SPI_DATA_LEN];				//�ӻ�״̬�ֽڻ���
u8 slave_stat_len = 0;							//�ӻ�״̬���ĳ���

u8 SPI_Rev_Len = 0;

_slave_stat slave_stat;

/**************************************************

��������SPI_Slave_Init
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ������ʼ���ӻ�SPI1�ӿ�
�޸ļ�¼��

**************************************************/

void SPI_Slave_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	SPI_InitTypeDef		SPI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);					//ʹ��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);					//ʹ��SPI1ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;			//PB5~7���ù������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;							//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;						//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);									//��ʼ��
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); 					//PB5����Ϊ SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); 					//PB6����Ϊ SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); 					//PB7����Ϊ SPI1
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);						//��λSPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);					//ֹͣ��λSPI1
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  	//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;							//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;								//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;							//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;								//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;						//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;								//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  									//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	
	SPI_I2S_ITConfig(SPI1, SPI_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;							//SPI1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;			//��ռ�ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	
	SPI_Cmd(SPI1, ENABLE); 													//ʹ��SPI����
	SPI_Slave_WriteByte(0xFF);											//��Ϊ�ӻ�,�˴�д��0xFF���ȴ���������ͨ��
	SPI_Slave_WriteByte(0xFF);
}


/**************************************************

��������SPI_Slave_TIMode_Init
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ������ʼ���ӻ�SPI1�ӿ�
�޸ļ�¼��

**************************************************/

void SPI_Slave_TIMode_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);					//ʹ��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);					//ʹ��SPI1ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;			//PB5~7���ù������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;							//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;						//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);									//��ʼ��
	

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); 					//PB5����Ϊ SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); 					//PB6����Ϊ SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); 					//PB7����Ϊ SPI1
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);						//��λSPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);					//ֹͣ��λSPI1
	
	SPI1->CR1 |= SPI_BaudRatePrescaler_256;
	SPI1->CR1 |= SPI_DataSize_8b;
	SPI1->CR2 |= 0x0010;
	SPI1->CR1 |= 0x0040;
		
	SPI_I2S_ITConfig(SPI1, SPI_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;							//SPI1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;			//��ռ�ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	SPI_Slave_WriteByte(0xFF);											//��Ϊ�ӻ�,�˴�д��0xFF���ȴ���������ͨ��
	SPI_Slave_WriteByte(0xFF);
}


/**************************************************

��������SPI_Slave_WriteByte
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ��������/��������
��	����TxData �����͵�����
�޸ļ�¼��

**************************************************/
u8 SPI_Slave_WriteByte(u8 TxData)
{		 			 
	u8 retry;																//�ȴ�ʱ�����
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)			//�ȴ���������	
	{
		if(++retry > 200)
			return RT_ERROR;
	}
	SPI_I2S_SendData(SPI1, TxData); 										//ͨ������SPI1����һ��byte����
	return RT_EOK; 											
 		    
}

/**************************************************

��������Slave_ReadByte
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ������������
��	����
�޸ļ�¼��

**************************************************/
u8 Slave_ReadByte(void)
{  
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);			//�ȴ�������һ��byte
 
	return SPI_I2S_ReceiveData(SPI1); 										//����ͨ��SPI1������յ�����      
}

/**************************************************

��������Slave_stat_framing
��  �ߣ�������
��  �ڣ�2018.8.28
��  ����V1.00
˵  �����ӻ�״̬������֡
��	����buf  ��֡������
����ֵ���������ݵĳ���
�޸ļ�¼��

**************************************************/
u8 Slave_stat_framing(u8 *buf)
{
	u8 cnt = 0;
	buf[cnt++] = ZTSB;
	buf[cnt++] = 28;
	buf[cnt++] = slave_stat.total_stat>>8;
	buf[cnt++] = slave_stat.total_stat;
	buf[cnt++] = slave_stat.slave_fault>>24;
	buf[cnt++] = slave_stat.slave_fault>>16;
	buf[cnt++] = slave_stat.slave_fault>>8;
	buf[cnt++] = slave_stat.slave_fault;
	buf[cnt++] = slave_stat.motor1_2;
	buf[cnt++] = slave_stat.motor3_4;
	buf[cnt++] = slave_stat.motor5_6;
	buf[cnt++] = slave_stat.power_vol;
	buf[cnt++] = slave_stat.power_cur;
	buf[cnt++] = (u32)(slave_stat.power_quantity * 1000) >> 24;
	buf[cnt++] = (u32)(slave_stat.power_quantity * 1000) >> 16;
	buf[cnt++] = (u32)(slave_stat.power_quantity * 1000) >> 8;
	buf[cnt++] = (u32)(slave_stat.power_quantity * 1000);
	buf[cnt++] = slave_stat.power_tem;
	buf[cnt++] = (u32)(slave_stat.power_fault * 1000) >> 24;
	buf[cnt++] = (u32)(slave_stat.power_fault * 1000) >> 16;
	buf[cnt++] = (u32)(slave_stat.power_fault * 1000) >> 8;
	buf[cnt++] = (u32)(slave_stat.power_fault * 1000);
	buf[cnt++] = slave_stat.cabin1_tem;
	buf[cnt++] = slave_stat.cabin2_tem;
	buf[cnt++] = slave_stat.cabin3_tem;
	buf[cnt++] = slave_stat.cabin4_tem;
	buf[cnt] = CRC8_Table(buf, cnt);
	buf[++cnt] = 0xEE;
	return cnt+1;
}

/**************************************************

��������SPI1_IRQHandler
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ����SPI1�жϽ��պ���
��	����
�޸ļ�¼��

**************************************************/
void SPI1_IRQHandler(void)
{
	rt_interrupt_enter();
	if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == SET) 
    {    
		if(SPI_Comm_Stat == Comm_IDLE)						//��SPI����
		{
			
			SPI_Data_Cnt = 0;
			SPI_Comm_Stat = Comm_BUSY;
			Master_CMD = SPI1->DR; 							//��ȡ����
			if(Master_CMD == ZTSB)							//��Ҫ��ӻ�����״̬��������֡
				Slave_stat_framing(Slave_Stat_Buf);
			slave_stat_len = 0;
		}
		if(SPI_Comm_Stat == Comm_BUSY)
		{
			switch(Master_CMD)
			{
				case XTZJ:									//ϵͳ�Լ�
					System_Check = 1;
				case XTJS:									//ϵͳУʱ
					System_timing = 1;
					time_rev_buf[SPI_Data_Cnt++] = SPI1->DR;
#ifdef  __DEBUG
//					rt_kprintf("%d\n",time_rev_buf[SPI_Data_Cnt-1]);
#endif
					if(SPI_Data_Cnt >= Time_Len)
					{
						SPI_Comm_Stat = Comm_IDLE;			//������ɣ���ΪIDLE״̬
					}
					SPI1->DR = 0xFF;
					break;
				case ZTSB:
					Slave_ReadByte();
					if(slave_stat_len < Slave_Stat_Len)
						SPI1->DR = Slave_Stat_Buf[slave_stat_len++];
					else
					{
						SPI1->DR = 0xFF;
						SPI_Comm_Stat = Comm_IDLE;			//���ͽ�������ΪIDLE״̬
					}
					break;
				case XTTZ:									//ϵͳֹͣ
					System_Stop = 1;
					SPI1->DR = 0xFF;
					SPI_Comm_Stat = Comm_IDLE;				//������ɣ���ΪIDLE״̬
					break;
				case XTFW:
					SPI_Comm_Stat = Comm_IDLE;				//������ɣ���ΪIDLE״̬
					Feed_Flag = WDG_DISABLE;				//���Ź���ֹι��
					break;
				case YTSF:									//�����������̬
				case YTXQ:
				case YTZZ:
				case YTYZ:
				case YTHT:
				case YTHX:
				case HLGH:
#ifdef  __DEBUG
				case DJXQL:									//���������
				case DJXQR:									//���������
				case DJZX :									//���ĵ��
				case DJYB:									//�������
				case DJWB:									//β�����
				case DJYG:									//�͸׵��
#endif
					
					run_posture[SPI_Data_Cnt++] = SPI1->DR;
#ifdef  __DEBUG
//					rt_kprintf("%d  ",run_posture[SPI_Data_Cnt-1]);
#endif
					if(SPI_Data_Cnt >= Fish_Stat_Len)
					{
						Pose_Trans = 1;
#ifdef  __DEBUG
//						rt_kprintf("\n");
#endif
						SPI_Comm_Stat = Comm_IDLE;			//������ɣ���ΪIDLE״̬
					}
					SPI1->DR = 0xFF;
					break;
				default:
					SPI1->DR = 0xFF;
					SPI_Comm_Stat = Comm_IDLE;				//������ɣ���ΪIDLE״̬
					break;
			}
				
		}
    }
	rt_interrupt_leave();

}



