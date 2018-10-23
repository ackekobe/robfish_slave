/**************************************************

�ļ�����RF.c
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ����RF���߶̾��봫��
�޸ļ�¼��

**************************************************/
#include "RF.h"

u8 RF_Rev_DMA_Buf[RF_LEN];
u8 RF_Rev_Buf[RF_LEN];
u8 RF_Send_Buf[RF_LEN];

u8 RF_Tx_Flag = 0;				//RF���ͱ�־ 1 ���ڷ��� 0 ���Ϳ���
u8 RF_Rev_Len = 0;

/**************************************************

��������RF_Init()
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ����
��  ����
�޸ļ�¼��

**************************************************/

void RF_USART2_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 							//ʹ��GPIOAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); 							//ʹ��GPIOCʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);							//ʹ��USART2ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 						//GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); 						//GPIOA3����ΪUSART2
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; 							//GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;									//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;								//�ٶ�2MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 									//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 									//����
	GPIO_Init(GPIOA,&GPIO_InitStructure); 											//��ʼ��PA2��PA3
	
	/************PC3���������RFģ��Ƭѡ�ܽ�**********/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 										//GPIOC3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;									//���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;								//�ٶ�2MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 									//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 									//����
	GPIO_Init(GPIOC,&GPIO_InitStructure); 	
	
	USART_InitStructure.USART_BaudRate = 9600;										//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); 										//��ʼ������2

	/*****ʹ��USART2�ж�*****/
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;								//USART2�����ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;						//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;								//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);									//ʹ�ܴ���DMA���ͽӿ�
	
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);									//ʹ��IDLE�ж�
	USART_Cmd(USART2, ENABLE);  													//ʹ�ܴ���2	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	RF_CS = 1;
}

/**************************************************

��������RF_DMA_Tx_init()
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ������ʼ��DMA����
�޸ļ�¼��

**************************************************/
static void RF_DMA_Tx_Init()
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Stream6);												//������������ʼ��
	while(DMA_GetCmdStatus(DMA1_Stream6) != DISABLE);
	
	DMA_InitStructure.DMA_Channel             = DMA_Channel_4;      		// DMAͨ��       
    DMA_InitStructure.DMA_PeripheralBaseAddr  = (u32)(&(USART2->DR));   	//Ŀ�ĵ�ַ
    DMA_InitStructure.DMA_Memory0BaseAddr     = (u32)RF_Send_Buf;    		//Դ��ַ         
    DMA_InitStructure.DMA_DIR                 = DMA_DIR_MemoryToPeripheral; //���䷽��   
    DMA_InitStructure.DMA_BufferSize          = RF_LEN;                    //���ݳ���        
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;  //�����ַ������  
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;       //�洢����ַ����  
    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;//�������ݿ��   
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;  	//�洢�����ݿ��    
    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;        	//����/ѭ������     
    DMA_InitStructure.DMA_Priority            = DMA_Priority_High;  		//���ȼ�           
    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;		//FIFO/ֱ��ģʽ          
    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull; //FIFO��С
    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;     //���δ���  
    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single; //���δ���
	DMA_Init(DMA1_Stream6, &DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);							//ʹ��DMA�ж�
	
	
	/***********����DMA�ж����ȼ�************/
	NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Stream6_IRQn;           
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2; 
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	DMA_Cmd(DMA1_Stream6, DISABLE);
}

/**************************************************

��������RF_DMA_Rx_init()
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ����RFģ��DMA���ճ�ʼ��
�޸ļ�¼��

**************************************************/
static void RF_DMA_Rx_Init()
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Stream5);												//������������ʼ��
	while(DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);
	
	DMA_InitStructure.DMA_Channel             = DMA_Channel_4;      		// DMAͨ��       
    DMA_InitStructure.DMA_PeripheralBaseAddr  = (u32)(&(USART2->DR));   	//Դ��ַ
    DMA_InitStructure.DMA_Memory0BaseAddr     = (u32)RF_Rev_DMA_Buf;    		//Ŀ�ĵ�ַ         
    DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralToMemory; //���䷽��   
    DMA_InitStructure.DMA_BufferSize          = RF_LEN;                    //���ݳ���        
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;  //�����ַ������  
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;       //�洢����ַ������  
    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;//�������ݿ��   
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;  	//�洢�����ݿ��    
    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;        	//����/ѭ������     
    DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;  	//���ȼ�           
    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;		//FIFO/ֱ��ģʽ          
    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull; //FIFO��С
    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;     //���δ���  
    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single; //���δ���
	
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);
	
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);							//ʹ�ܴ���DMA���սӿ�
	
	
	DMA_Cmd(DMA1_Stream5, ENABLE);
}

/**************************************************

��������AHRS_Tx_Enable()
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ����DMA����ʹ��
�޸ļ�¼��

**************************************************/
void AHRS_Tx_Enable(u8 *data, u16 ndtr)
{	
	while(RF_Tx_Flag);			//ȷ���ϴη����ѽ���
	RF_Tx_Flag = 1;
	
	rt_memcpy(RF_Send_Buf, data, ndtr);	
	DMA_SetCurrDataCounter(DMA1_Stream6, ndtr);
	DMA_Cmd(DMA1_Stream6, ENABLE);
}

/**************************************************

��������RF_irq_rx_end
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  �������ڽ�����ɴ�����
��  ����buf ���ջ���
����ֵ���������ݳ���
�޸ļ�¼��

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

��������RF_irq_tx_end
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  �������ڽ�����ɴ�����
�޸ļ�¼��

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

��������USART2_IRQHandler
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ����USART2�����жϴ�����
�޸ļ�¼��

**************************************************/
void USART2_IRQHandler(void)
{
	rt_interrupt_enter();
	
	RF_irq_tx_end();
	RF_Rev_Len = RF_irq_rx_end(RF_Rev_Buf);
	
	rt_interrupt_leave();
}

/**************************************************

��������DMA1_Stream6_IRQHandler
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ����DMA�����жϴ�����
�޸ļ�¼��

**************************************************/
void DMA1_Stream6_IRQHandler(void)
{
	rt_interrupt_enter();
	
	if(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6) != RESET)
	{
		DMA_ClearFlag(DMA1_Stream6, DMA_IT_TCIF6);						//�巢����ɱ�־
		
		DMA_Cmd(DMA1_Stream6, DISABLE);									//�ر�DMA����
		
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);					//�򿪷�������ж�
	}
	
	rt_interrupt_leave();
}

/**************************************************

��������RF_Init
��  �ߣ�������
��  �ڣ�2018.8.21
��  ����V1.00
˵  ����RF��ʼ������
�޸ļ�¼��

**************************************************/
void RF_Init(void)
{
	RF_USART2_Init();
	RF_DMA_Tx_Init();
	RF_DMA_Rx_Init();
}



