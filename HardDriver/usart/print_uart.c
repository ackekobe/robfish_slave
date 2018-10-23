/**************************************************

�ļ�����print_uart.c
��  �ߣ�������
��  �ڣ�2018.7.18
��  ����V1.00
˵  ������ʼ����ӡ����uart4
�޸ļ�¼��

**************************************************/


#include "sys.h"
#include "print_uart.h"	

#if SYSTEM_SUPPORT_OS
#include "rtthread.h"					 
#endif

u16 UART4_RX_STA = 0;
u8 UART4_buf[17];
u8 uart4_flag = 0;				//���յ����ݱ�־


#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 

/**************************************************

��������uart4_init()
��  �ߣ�������
��  �ڣ�2018.7.18
��  ����V1.00
˵  ������ʼ��IO���봮��4
�޸ļ�¼��

**************************************************/
void uart4_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;										//�ж�����
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 						//ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);						//ʹ��USART4ʱ��
	
	
	
	//GPIO�˿�����
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;							//�ٶ�2MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 								//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 								//����	 
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	
	//����4��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4); 					//GPIOA0����ΪUSART4
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); 					//GPIOA1����ΪUSART4
	

   //UART4 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;									//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;					//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;						//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;							//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				//�շ�ģʽ
	USART_Init(UART4, &USART_InitStructure); 									//��ʼ������4
	USART_Cmd(UART4, ENABLE);  													//ʹ�ܴ���4
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;							//����4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;						//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;								//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);									//USART4�����жϿ���
	
}

/**************************************************

��������rt_hw_console_output(const char *str)
��  �ߣ�������
��  �ڣ�2018.7.18
��  ����V1.00
˵  ������д��ӡ���ڵ��������
�޸ļ�¼��

**************************************************/
void rt_hw_console_output(const char *str)
{
    /* empty console output */
	
	rt_interrupt_enter();

	while(*str!='\0')
	{
		if(*str=='\n')
		{
			USART_SendData(UART4, '\r'); 
			while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
		}
		USART_SendData(UART4, *str++); 
		while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);	
	}

	rt_interrupt_leave();
}

/**************************************************

��������UART4_IRQHandler
��  �ߣ�������
��  �ڣ�2018.7.24
��  ����V1.00
˵  ����UART4��������жϴ�����
�޸ļ�¼��

**************************************************/
void UART4_IRQHandler(void)
{
	u8 res;
	rt_interrupt_enter();
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		res = UART4->DR;
		if((UART4_RX_STA & 0x8000) == 0)
		{
			if(UART4_RX_STA++ < UART4_BUF_SIZE)		//�ж��ǹ����յ��㹻���ֽ���
				UART4_buf[UART4_RX_STA] = res;
			else
			{
				UART4_RX_STA |= 1<<15;				//�������λ��λ
				UART4->CR1 &= ~USART_Mode_Rx;
				uart4_flag = 1;
			}
		}
	}
	rt_interrupt_leave();
}

 



