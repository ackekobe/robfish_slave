/**************************************************

�ļ�����CAN1.c
��  �ߣ�������
��  �ڣ�2018.8.23
��  ����V1.00
˵  ����CAN1ͨ������
�޸ļ�¼��

**************************************************/
#include "CAN1.h"


/**************************************************

��������CAN1_Init
��  �ߣ�������
��  �ڣ�2018.8.23
��  ����V1.00
˵  ������ʼ��CAN1ͨ�Žӿ�
��  ����tsjw	����ͬ������ʱ�䵥Ԫ
		tbs2	ʱ���2��ʱ�䵥Ԫ
		tbs1	ʱ���1��ʱ�䵥Ԫ
		brp		�����ʷ�Ƶϵ��
		mode	��ͨģʽ/�ػ�ģʽ
�޸ļ�¼��

**************************************************/
void CAN1_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef     	CAN_InitStructure;						//CAN��ʼ���ṹ��
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;				//��������ʼ���ṹ��
	NVIC_InitTypeDef  		NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);		//ʹ��PORTAʱ��	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);		//ʹ��CAN1ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//50MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);						//��ʼ��PB8,PB9
	
	  //���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1); 		//GPIOB8����ΪCAN1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1); 		//GPIOB9����ΪCAN1
	
	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;							//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=DISABLE;							//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;							//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;							//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;							//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;							//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= mode;	 						//ģʽ���� 
  	CAN_InitStructure.CAN_SJW=tsjw;								//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1;								//Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;								//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  						//��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   						// ��ʼ��CAN1 
	
	//���ù�����
 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  						//������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 		//32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;					////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;				//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;	//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; 				//���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);							//�˲�����ʼ��
	
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************

��������CAN1_Send_Msg
��  �ߣ�������
��  �ڣ�2018.8.23
��  ����V1.00
˵  ����CAN1��������
��  ����msg  	Ҫ���͵����ݣ����8�ֽ�
		len	 	Ҫ���͵����ݵĳ���
		stdid	��׼��ʶ��
		extid	������չ��ʾ��
		ide		�Ƿ�ʹ����չ��־��
		rtr		��Ϣ����
����ֵ��0 ���ͳɹ�
		1 ����ʧ��
�޸ļ�¼��

**************************************************/
u8 CAN1_Send_Msg(u8* msg,u8 len, u32 stdid, u32 extid, u8 ide, u8 rtr)
{	
  u8 mbox;					//ѡ�������ݵ�����
  u16 i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId = stdid;	 	// ��׼��ʶ��Ϊ0
  TxMessage.ExtId = extid;	 	// ������չ��ʾ����29λ��
  TxMessage.IDE = ide;		  	// ʹ����չ��ʶ��
  TxMessage.RTR = rtr;		  	// ��Ϣ����Ϊ����֡��һ֡8λ
  TxMessage.DLC = len;							 // ������֡��Ϣ
  for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          
  mbox= CAN_Transmit(CAN1, &TxMessage);   	
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
  if(i>=0XFFF)return RT_ERROR;
  return RT_EOK;		

}

/**************************************************

��������CAN1_Receive_Msg
��  �ߣ�������
��  �ڣ�2018.8.23
��  ����V1.00
˵  ����CAN1��������
��  ����msg  Ҫ���͵����ݣ����8�ֽ�
		len	 Ҫ���͵����ݵĳ���
�޸ļ�¼��

**************************************************/
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)
		return RT_ERROR;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
    for(i=0;i<RxMessage.DLC;i++)
		buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}


/**************************************************

��������CAN1_RX0_IRQHandler
��  �ߣ�������
��  �ڣ�2018.8.23
��  ����V1.00
˵  ����CAN1�����жϴ�����
�޸ļ�¼��

**************************************************/
void CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
	int i=0;
    CAN_Receive(CAN1, 0, &RxMessage);
	for(i=0;i<8;i++)
		rt_kprintf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}




