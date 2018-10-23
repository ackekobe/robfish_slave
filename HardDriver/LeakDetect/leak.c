/**************************************************

文件名：leak.c
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：鱼舱内漏水检测
修改记录：

**************************************************/

#include "leak.h"
#include "led.h"
u8 leak_stat = 0;  			//漏水检测状态标志 	BIT0 第1处漏水
							//					BIT1 第2处漏水
							//					BIT2 第3处漏水
							//					BIT3 第4处漏水
						
struct rt_semaphore leak_sem;				//漏水检测信号量

/**************************************************

函数名：leak_Init
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：漏水检测初始化函数
参  数：
修改记录：

**************************************************/

void leak_Init()
{
#if 0
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);					//使能SYSCFG时钟
	EXTI_InitTypeDef	EXTI_InitStructure;	
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	//使能GPIOF时钟
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource14);			//映射中断线
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;								//输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;							//速度2MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 								//推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 								//下拉	 
	GPIO_Init(GPIOB,&GPIO_InitStructure); 										//PB14.15初始化
	
	GPIO_Init(GPIOF,&GPIO_InitStructure);										//PF14.15初始化
	
	/*******************配置EXTI_Line_x***********************/
	EXTI_InitStructure.EXTI_Line = EXTI_Line14 | EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;					//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;					//外部中断线10-15
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;			//抢占中断优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	if(rt_sem_init(&leak_sem, "leak_sem", 0, RT_IPC_FLAG_FIFO) != RT_EOK)	//初始化信号量
		rt_kprintf("leak_sem init ERROR!\n");
#endif	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);					//使能SYSCFG时钟
	EXTI_InitTypeDef	EXTI_InitStructure;	
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	//使能GPIOB时钟
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource3);			//映射中断线
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;								//输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;							//速度2MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 								//推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 								//上拉	 
	GPIO_Init(GPIOD,&GPIO_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;					//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;					//外部中断线10-15
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;			//抢占中断优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************

函数名：EXTI15_10_IRQHandler
作  者：刘晓东
日  期：2018.8.17
版  本：V1.00
说  明：外部中断线14、15的中断处理函数
修改记录：

**************************************************/
void EXTI15_10_IRQHandler(void)
{
	rt_interrupt_enter();
	if(EXTI_GetITStatus(EXTI_Line14) != RESET)						//检测中断线14
	{
		if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_14) == Bit_RESET)	//第1处漏水
			leak_stat |= 0x01;
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == Bit_RESET)	//第3处漏水
			leak_stat |= 0x04;
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)						//检测中断线15
	{
		if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_15) == Bit_RESET)	//第2处漏水
			leak_stat |= 0x02;
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == Bit_RESET)	//第4处漏水
			leak_stat |= 0x08;
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
	rt_interrupt_leave();
	if(leak_stat > 0)
		rt_sem_release(&leak_sem);									//释放信号量
}

void EXTI3_IRQHandler(void)
{
 u8	cnt = 0;
	if(cnt == 10)
		cnt  = 0;
	++cnt;
	LED1 = 0;
	EXTI_ClearITPendingBit(EXTI_Line3);
	//delay_us(10);
}






