/**************************************************

�ļ�����test.c
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  ������Χ�豸���Ժ���
�޸ļ�¼��

**************************************************/
#include "test.h"
#include "led.h"
#include "stdio.h"

#if TEST_FLAG
/**************************************************

��������LED_Test
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  ����LEDָʾ�Ʋ��Ժ���
��  ����
�޸ļ�¼��

**************************************************/
void LED_Test(void)
{
		delay_ms(1000);
		LED1 = !LED1;
		LED2 = !LED3;
		LED3 = !LED3;
		LED4 = !LED4;
		LED5 = !LED5;
}

/**************************************************

��������leak_Test
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  ����©ˮ�����Ժ���
��  ����
�޸ļ�¼��

**************************************************/
void leak_Test(u8 index)
{
	rt_base_t level;
	level = rt_hw_interrupt_disable();				//�����ж�
	switch(index)
	{
		case 1:
			if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_14) == Bit_RESET)
				LED1 = 0;
			else
				LED1 = 1;
			break;
		case 2:
			if(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_15) == Bit_RESET)
				LED2 = 0;
			else
				LED2 = 1;
			break;
		case 3:
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == Bit_RESET)
				LED3 = 0;
			else
				LED3 = 1;
			break;
		case 4:
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == Bit_RESET)
				LED4 = 0;
			else
				LED4 = 1;
			break;
		default:
				break;
	}
	rt_hw_interrupt_enable(level);			//���ж�
}


/**************************************************

��������delay_test()
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  ����������ʱ����
�޸ļ�¼��

**************************************************/
void delay_test()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ��

	//GPIOG��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	//25MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//����
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIO	
	
	delay_test_flag = 1;
	delay_us(1000);
	delay_us(1000);
	delay_us(1000);
	delay_us(1000);
	delay_us(1000);
	delay_us(1000);
	delay_test_flag = 0;
	delay_us(1000);
	delay_us(1000);
	delay_us(1000);
	delay_us(1000);
	delay_us(1000);
	delay_us(1000);
	delay_us(1000);
	delay_us(358);
	delay_test_flag = 1;
}



/**************************************************

��������DS18B20_test()
��  �ߣ�������
��  �ڣ�2018.8.17
��  ����V1.00
˵  ����������ʱ����
�޸ļ�¼��

**************************************************/
float DS18B20_test()
{
	float temper;
	u8 buf[24];
//	while(DS18B20_Init() == RT_ERROR)
//	{
//		LED1 = 0;
//	}
	if(DS18B20_Search_Rom())
		LED2 = 0;
	temper = DS18B20_Get_Temp(0);
	delay_us(10);
	sprintf((char*)buf,"temper:%8.4f\r\n",temper);
	AHRS_Tx_Enable(buf, 17);
	return temper;
}



#endif




