/**************************************************

文件名：RF.c
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：RF无线短距离传输
修改记录：

**************************************************/
#include "RF.h"

u8 RF_Rev_DMA_Buf[RF_LEN];
u8 RF_Rev_Buf[RF_LEN];
u8 RF_Send_Buf[RF_LEN];

u8 RF_Tx_Flag = 0;				//RF发送标志 1 正在发送 0 发送空闲
u8 RF_Rev_Len = 0;

/**************************************************

函数名：RF_Init()
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：
参  数：
修改记录：

**************************************************/

void RF_USART2_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 							//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); 							//使能GPIOC时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);							//使能USART2时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 						//GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); 						//GPIOA3复用为USART2
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; 							//GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;									//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;								//速度2MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 									//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 									//上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); 											//初始化PA2，PA3
	
	/************PC3推挽输出，RF模块片选管脚**********/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 										//GPIOC3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;									//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;								//速度2MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 									//推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 									//上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); 	
	
	USART_InitStructure.USART_BaudRate = 9600;										//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
	USART_Init(USART2, &USART_InitStructure); 										//初始化串口2

	/*****使能USART2中断*****/
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;								//USART2串口中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;						//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;								//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);									//使能串口DMA发送接口
	
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);									//使能IDLE中断
	USART_Cmd(USART2, ENABLE);  													//使能串口2	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	RF_CS = 1;
}

/**************************************************

函数名：RF_DMA_Tx_init()
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：初始化DMA发送
修改记录：

**************************************************/
static void RF_DMA_Tx_Init()
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Stream6);												//发送数据流初始化
	while(DMA_GetCmdStatus(DMA1_Stream6) != DISABLE);
	
	DMA_InitStructure.DMA_Channel             = DMA_Channel_4;      		// DMA通道       
    DMA_InitStructure.DMA_PeripheralBaseAddr  = (u32)(&(USART2->DR));   	//目的地址
    DMA_InitStructure.DMA_Memory0BaseAddr     = (u32)RF_Send_Buf;    		//源地址         
    DMA_InitStructure.DMA_DIR                 = DMA_DIR_MemoryToPeripheral; //传输方向   
    DMA_InitStructure.DMA_BufferSize          = RF_LEN;                    //数据长度        
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;  //外设地址不增长  
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;       //存储器地址增长  
    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;//外设数据宽度   
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;  	//存储器数据宽度    
    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;        	//单次/循环传输     
    DMA_InitStructure.DMA_Priority            = DMA_Priority_High;  		//优先级           
    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;		//FIFO/直接模式          
    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull; //FIFO大小
    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;     //单次传输  
    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single; //单次传输
	DMA_Init(DMA1_Stream6, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);							//使能DMA中断
	
	
	/***********配置DMA中断优先级************/
	NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Stream6_IRQn;           
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2; 
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	DMA_Cmd(DMA1_Stream6, DISABLE);
}

/**************************************************

函数名：RF_DMA_Rx_init()
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：RF模块DMA接收初始化
修改记录：

**************************************************/
static void RF_DMA_Rx_Init()
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Stream5);												//接收数据流初始化
	while(DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);
	
	DMA_InitStructure.DMA_Channel             = DMA_Channel_4;      		// DMA通道       
    DMA_InitStructure.DMA_PeripheralBaseAddr  = (u32)(&(USART2->DR));   	//源地址
    DMA_InitStructure.DMA_Memory0BaseAddr     = (u32)RF_Rev_DMA_Buf;    		//目的地址         
    DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralToMemory; //传输方向   
    DMA_InitStructure.DMA_BufferSize          = RF_LEN;                    //数据长度        
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;  //外设地址不增长  
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;       //存储器地址不增长  
    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;//外设数据宽度   
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;  	//存储器数据宽度    
    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;        	//单次/循环传输     
    DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;  	//优先级           
    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;		//FIFO/直接模式          
    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull; //FIFO大小
    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;     //单次传输  
    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single; //单次传输
	
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);
	
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);							//使能串口DMA接收接口
	
	
	DMA_Cmd(DMA1_Stream5, ENABLE);
}

/**************************************************

函数名：AHRS_Tx_Enable()
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：DMA发送使能
修改记录：

**************************************************/
void AHRS_Tx_Enable(u8 *data, u16 ndtr)
{	
	while(RF_Tx_Flag);			//确认上次发送已结束
	RF_Tx_Flag = 1;
	
	rt_memcpy(RF_Send_Buf, data, ndtr);	
	DMA_SetCurrDataCounter(DMA1_Stream6, ndtr);
	DMA_Cmd(DMA1_Stream6, ENABLE);
}

/**************************************************

函数名：RF_irq_rx_end
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：串口接收完成处理函数
参  数：buf 接收缓存
返回值：接收数据长度
修改记录：

**************************************************/
static u8 RF_irq_rx_end(u8 *buf)
{
	u16 len = 0;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		USART2->SR;
		USART2->DR;
		DMA_Cmd(DMA1_Stream5, DISABLE);
		DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
		len = RF_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
		
		rt_memcpy(buf, RF_Rev_DMA_Buf, len);
		
		DMA_SetCurrDataCounter(DMA1_Stream5, RF_LEN);
		DMA_Cmd(DMA1_Stream5, ENABLE);
		return len;
	}
	return 0;
}

/**************************************************

函数名：RF_irq_tx_end
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：串口接收完成处理函数
修改记录：

**************************************************/

static void RF_irq_tx_end(void)
{
	if(USART_GetFlagStatus(USART2, USART_IT_TC) == RESET)
	{
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);
		
		RF_Tx_Flag = 0;
	}
}

/**************************************************

函数名：USART2_IRQHandler
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：USART2接收中断处理函数
修改记录：

**************************************************/
void USART2_IRQHandler(void)
{
	rt_interrupt_enter();
	
	RF_irq_tx_end();
	RF_Rev_Len = RF_irq_rx_end(RF_Rev_Buf);
	
	rt_interrupt_leave();
}

/**************************************************

函数名：DMA1_Stream6_IRQHandler
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：DMA发送中断处理函数
修改记录：

**************************************************/
void DMA1_Stream6_IRQHandler(void)
{
	rt_interrupt_enter();
	
	if(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6) != RESET)
	{
		DMA_ClearFlag(DMA1_Stream6, DMA_IT_TCIF6);						//清发送完成标志
		
		DMA_Cmd(DMA1_Stream6, DISABLE);									//关闭DMA发送
		
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);					//打开发送完成中断
	}
	
	rt_interrupt_leave();
}

/**************************************************

函数名：RF_Init
作  者：刘晓东
日  期：2018.8.21
版  本：V1.00
说  明：RF初始化函数
修改记录：

**************************************************/
void RF_Init(void)
{
	RF_USART2_Init();
	RF_DMA_Tx_Init();
	RF_DMA_Rx_Init();
}



