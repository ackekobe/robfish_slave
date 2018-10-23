/**************************************************

文件名：CAN1.c
作  者：刘晓东
日  期：2018.8.23
版  本：V1.00
说  明：CAN1通信驱动
修改记录：

**************************************************/
#include "CAN1.h"


/**************************************************

函数名：CAN1_Init
作  者：刘晓东
日  期：2018.8.23
版  本：V1.00
说  明：初始化CAN1通信接口
参  数：tsjw	重新同步跳步时间单元
		tbs2	时间段2的时间单元
		tbs1	时间段1的时间单元
		brp		波特率分频系数
		mode	普通模式/回环模式
修改记录：

**************************************************/
void CAN1_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef     	CAN_InitStructure;						//CAN初始化结构体
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;				//过滤器初始化结构体
	NVIC_InitTypeDef  		NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);		//使能PORTA时钟	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);		//使能CAN1时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//50MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);						//初始化PB8,PB9
	
	  //引脚复用映射配置
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1); 		//GPIOB8复用为CAN1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1); 		//GPIOB9复用为CAN1
	
	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;							//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;							//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;							//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;							//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;							//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;							//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= mode;	 						//模式设置 
  	CAN_InitStructure.CAN_SJW=tsjw;								//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1;								//Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;								//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  						//分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   						// 初始化CAN1 
	
	//配置过滤器
 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  						//过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 		//32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;					////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;				//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;	//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; 				//激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);							//滤波器初始化
	
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************

函数名：CAN1_Send_Msg
作  者：刘晓东
日  期：2018.8.23
版  本：V1.00
说  明：CAN1发送数据
参  数：msg  	要发送的数据，最大8字节
		len	 	要发送的数据的长度
		stdid	标准标识符
		extid	设置扩展标示符
		ide		是否使用扩展标志符
		rtr		消息类型
返回值：0 发送成功
		1 发送失败
修改记录：

**************************************************/
u8 CAN1_Send_Msg(u8* msg,u8 len, u32 stdid, u32 extid, u8 ide, u8 rtr)
{	
  u8 mbox;					//选择发送数据的信箱
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId = stdid;	 	// 标准标识符为0
  TxMessage.ExtId = extid;	 	// 设置扩展标示符（29位）
  TxMessage.IDE = ide;		  	// 使用扩展标识符
  TxMessage.RTR = rtr;		  	// 消息类型为数据帧，一帧8位
  TxMessage.DLC = len;							 // 发送两帧信息
  for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];				 // 第一帧信息          
  mbox= CAN_Transmit(CAN1, &TxMessage);   	
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
  if(i>=0XFFF)return RT_ERROR;
  return RT_EOK;		

}

/**************************************************

函数名：CAN1_Receive_Msg
作  者：刘晓东
日  期：2018.8.23
版  本：V1.00
说  明：CAN1接收数据
参  数：msg  要发送的数据，最大8字节
		len	 要发送的数据的长度
修改记录：

**************************************************/
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)
		return RT_ERROR;		//没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
    for(i=0;i<RxMessage.DLC;i++)
		buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}


/**************************************************

函数名：CAN1_RX0_IRQHandler
作  者：刘晓东
日  期：2018.8.23
版  本：V1.00
说  明：CAN1接收中断处理函数
修改记录：

**************************************************/
void CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
	int i=0;
    CAN_Receive(CAN1, 0, &RxMessage);
	for(i=0;i<8;i++)
		rt_kprintf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}




