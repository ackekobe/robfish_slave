/**************************************************

文件名：SPI_Slave.c
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：SPI通信从机驱动
修改记录：

**************************************************/
#include "SPI_Slave.h"





u8 SPI_Data_Cnt = 0;						//从机SPI收发数据计数
u8 SPI_Comm_Stat = Comm_IDLE;						//SPI通信状态标志
u8 Master_CMD = 0;									//主机发送的数据命令码
u8 System_Check = 0;								//系统自检标志位
u8 System_timing = 0;								//系统校时标志位
u8 System_Stop = 0;									//系统电机停止位
u8 Pose_Trans = 0;									//鱼姿态转换

u8 time_rev_buf[Time_Len];								//校时接收缓存
u8 run_posture[Fish_Stat_Len];									//运行姿态缓存
u8 Slave_Stat_Buf[SPI_DATA_LEN];				//从机状态字节缓存
u8 slave_stat_len = 0;							//从机状态报文长度

u8 SPI_Rev_Len = 0;

_slave_stat slave_stat;

/**************************************************

函数名：SPI_Slave_Init
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：初始化从机SPI1接口
修改记录：

**************************************************/

void SPI_Slave_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	SPI_InitTypeDef		SPI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);					//使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);					//使能SPI1时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;			//PB5~7复用功能输出	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;							//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;						//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);									//初始化
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); 					//PB5复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); 					//PB6复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); 					//PB7复用为 SPI1
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);						//复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);					//停止复位SPI1
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  	//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;							//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;								//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;							//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;								//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;						//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;								//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  									//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	SPI_I2S_ITConfig(SPI1, SPI_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;							//SPI1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;			//抢占中断优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	
	SPI_Cmd(SPI1, ENABLE); 													//使能SPI外设
	SPI_Slave_WriteByte(0xFF);											//作为从机,此处写入0xFF，等待主机开启通信
	SPI_Slave_WriteByte(0xFF);
}


/**************************************************

函数名：SPI_Slave_TIMode_Init
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：初始化从机SPI1接口
修改记录：

**************************************************/

void SPI_Slave_TIMode_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);					//使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);					//使能SPI1时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;			//PB5~7复用功能输出	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;							//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;						//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);									//初始化
	

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); 					//PB5复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); 					//PB6复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); 					//PB7复用为 SPI1
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);						//复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);					//停止复位SPI1
	
	SPI1->CR1 |= SPI_BaudRatePrescaler_256;
	SPI1->CR1 |= SPI_DataSize_8b;
	SPI1->CR2 |= 0x0010;
	SPI1->CR1 |= 0x0040;
		
	SPI_I2S_ITConfig(SPI1, SPI_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;							//SPI1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;			//抢占中断优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	SPI_Slave_WriteByte(0xFF);											//作为从机,此处写入0xFF，等待主机开启通信
	SPI_Slave_WriteByte(0xFF);
}


/**************************************************

函数名：SPI_Slave_WriteByte
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：发送/接收数据
参	数：TxData 待发送的数据
修改记录：

**************************************************/
u8 SPI_Slave_WriteByte(u8 TxData)
{		 			 
	u8 retry;																//等待时间变量
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)			//等待发送区空	
	{
		if(++retry > 200)
			return RT_ERROR;
	}
	SPI_I2S_SendData(SPI1, TxData); 										//通过外设SPI1发送一个byte数据
	return RT_EOK; 											
 		    
}

/**************************************************

函数名：Slave_ReadByte
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：接收数据
参	数：
修改记录：

**************************************************/
u8 Slave_ReadByte(void)
{  
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);			//等待接收完一个byte
 
	return SPI_I2S_ReceiveData(SPI1); 										//返回通过SPI1最近接收的数据      
}

/**************************************************

函数名：Slave_stat_framing
作  者：刘晓东
日  期：2018.8.28
版  本：V1.00
说  明：从机状态报文组帧
参	数：buf  组帧缓存区
返回值：发送数据的长度
修改记录：

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

函数名：SPI1_IRQHandler
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：SPI1中断接收函数
参	数：
修改记录：

**************************************************/
void SPI1_IRQHandler(void)
{
	rt_interrupt_enter();
	if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == SET) 
    {    
		if(SPI_Comm_Stat == Comm_IDLE)						//若SPI空闲
		{
			
			SPI_Data_Cnt = 0;
			SPI_Comm_Stat = Comm_BUSY;
			Master_CMD = SPI1->DR; 							//读取数据
			if(Master_CMD == ZTSB)							//若要求从机上送状态，则先组帧
				Slave_stat_framing(Slave_Stat_Buf);
			slave_stat_len = 0;
		}
		if(SPI_Comm_Stat == Comm_BUSY)
		{
			switch(Master_CMD)
			{
				case XTZJ:									//系统自检
					System_Check = 1;
				case XTJS:									//系统校时
					System_timing = 1;
					time_rev_buf[SPI_Data_Cnt++] = SPI1->DR;
#ifdef  __DEBUG
//					rt_kprintf("%d\n",time_rev_buf[SPI_Data_Cnt-1]);
#endif
					if(SPI_Data_Cnt >= Time_Len)
					{
						SPI_Comm_Stat = Comm_IDLE;			//接收完成，置为IDLE状态
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
						SPI_Comm_Stat = Comm_IDLE;			//发送结束，置为IDLE状态
					}
					break;
				case XTTZ:									//系统停止
					System_Stop = 1;
					SPI1->DR = 0xFF;
					SPI_Comm_Stat = Comm_IDLE;				//接收完成，置为IDLE状态
					break;
				case XTFW:
					SPI_Comm_Stat = Comm_IDLE;				//接收完成，置为IDLE状态
					Feed_Flag = WDG_DISABLE;				//开门狗禁止喂狗
					break;
				case YTSF:									//鱼各种运行姿态
				case YTXQ:
				case YTZZ:
				case YTYZ:
				case YTHT:
				case YTHX:
				case HLGH:
#ifdef  __DEBUG
				case DJXQL:									//左胸鳍电机
				case DJXQR:									//右胸鳍电机
				case DJZX :									//重心电机
				case DJYB:									//腰部电机
				case DJWB:									//尾部电机
				case DJYG:									//油缸电机
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
						SPI_Comm_Stat = Comm_IDLE;			//接收完成，置为IDLE状态
					}
					SPI1->DR = 0xFF;
					break;
				default:
					SPI1->DR = 0xFF;
					SPI_Comm_Stat = Comm_IDLE;				//接收完成，置为IDLE状态
					break;
			}
				
		}
    }
	rt_interrupt_leave();

}



