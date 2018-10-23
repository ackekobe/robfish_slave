/**************************************************

�ļ�����leak.c
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  ���������©ˮ���
�޸ļ�¼��

**************************************************/

#include "leak.h"
#include "led.h"
u8 leak_stat = 0;  			//©ˮ���״̬��־ 	BIT0 ��1��©ˮ
							//					BIT1 ��2��©ˮ
							//					BIT2 ��3��©ˮ
							//					BIT3 ��4��©ˮ
						
struct rt_semaphore leak_sem;				//©ˮ����ź���

/**************************************************

��������leak_Init
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  ����©ˮ����ʼ������
��  ����
�޸ļ�¼��

**************************************************/

void leak_Init()
{
#if 0
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);					//ʹ��SYSCFGʱ��
	EXTI_InitTypeDef	EXTI_InitStructure;	
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//ʹ��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	//ʹ��GPIOFʱ��
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource14);			//ӳ���ж���
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;								//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;							//�ٶ�2MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 								//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 								//����	 
	GPIO_Init(GPIOB,&GPIO_InitStructure); 										//PB14.15��ʼ��
	
	GPIO_Init(GPIOF,&GPIO_InitStructure);										//PF14.15��ʼ��
	
	/*******************����EXTI_Line_x***********************/
	EXTI_InitStructure.EXTI_Line = EXTI_Line14 | EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;					//�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;					//�ⲿ�ж���10-15
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;			//��ռ�ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	if(rt_sem_init(&leak_sem, "leak_sem", 0, RT_IPC_FLAG_FIFO) != RT_EOK)	//��ʼ���ź���
		rt_kprintf("leak_sem init ERROR!\n");
#endif	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);					//ʹ��SYSCFGʱ��
	EXTI_InitTypeDef	EXTI_InitStructure;	
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	//ʹ��GPIOBʱ��
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource3);			//ӳ���ж���
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;								//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;							//�ٶ�2MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 								//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 								//����	 
	GPIO_Init(GPIOD,&GPIO_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;					//�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;					//�ⲿ�ж���10-15
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;			//��ռ�ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************

��������EXTI15_10_IRQHandler
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  �����ⲿ�ж���14��15���жϴ�����
�޸ļ�¼��

**************************************************/
void EXTI15_10_IRQHandler(void)
{
	rt_interrupt_enter();
	if(EXTI_GetITStatus(EXTI_Line14) != RESET)						//����ж���14
	{
		if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_14) == Bit_RESET)	//��1��©ˮ
			leak_stat |= 0x01;
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == Bit_RESET)	//��3��©ˮ
			leak_stat |= 0x04;
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)						//����ж���15
	{
		if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_15) == Bit_RESET)	//��2��©ˮ
			leak_stat |= 0x02;
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == Bit_RESET)	//��4��©ˮ
			leak_stat |= 0x08;
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
	rt_interrupt_leave();
	if(leak_stat > 0)
		rt_sem_release(&leak_sem);									//�ͷ��ź���
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






