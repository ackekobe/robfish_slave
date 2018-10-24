#include "sys.h"
#include "delay.h"
#include "rtthread.h"
#include "usbh_usr.h"
#include <rthw.h>
#include "led.h"
#include "leak.h"
#include "test.h"
#include "DS18B20.h"
#include "SPI_Slave.h"
#include "CAN1.h"
#include "CAN2.h"
#include "SPI_Slave.h"
#include "adc.h"
#include "RF.h"




static struct rt_thread led0_thread;//�߳̿��ƿ�
static struct rt_thread led1_thread;//�߳̿��ƿ�
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rt_led0_thread_stack[1024];//�߳�ջ 
static rt_uint8_t rt_led1_thread_stack[1024];//�߳�ջ 

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;



static void led0_thread_entry(void* parameter)
{
//	u8 buf[10] = {1,2,3,4,5,6,7,8,9,0};
//	static u8 t = 0;
//	 u8 cnt = 0;
	while(1)
	{
//		u16 adcx = 0;
//		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
//		adcx=Get_Adc_Average(ADC_Channel_8,20);//��ȡͨ��5��ת��ֵ��20��ȡƽ��
//		rt_kprintf("adcx:%d\r\n", adcx);
		LED5 = !LED5;
	}


}

//�߳�LED1
static void led1_thread_entry(void* parameter)
{
	u16 res = 0, cnt = 0;
	u8 buf[8] = {9,5,1,7,5,3,4,6};
	rt_base_t level;

	while(1)
	{
		delay_ms(1000);
//		delay_ms(1000);
		if(Pose_Trans == 1)
		{
			Pose_Trans = 0;
			for(cnt = 0; cnt < 38; ++cnt)
				rt_kprintf("%d  ", run_posture[cnt]);
			rt_kprintf("\r\n");
		}
		
		LED1 = 1;
//	level = rt_hw_interrupt_disable();
//		CAN1_Send_Msg(buf,8,0x12,0x12, 0, 0);
//	rt_hw_interrupt_enable(level);
//		delay_ms(1000);
		LED1 = 1;
		
	}
}


int main(void)
{
	rt_base_t level;
	level = rt_hw_interrupt_disable();
	u16 delay_cnt = 0;
	
	LED_Init();
	LED1 = 0;
	for(delay_cnt= 0;  delay_cnt < 1000; ++delay_cnt)
		delay_us(1000);
	LED1 = 1;
	
	SPI_Slave_TIMode_Init();
//	leak_Init();	
//	delay_test();	
//	DS18B20_test();	
//	SPI_Slave_WriteByte(0xaa);
//	adc_Init();
//	RF_Init();
//	while(DS18B20_Init() == RT_ERROR)
//	{
//		LED1 = 0;
//	}

//CAN1_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS2_7tq,6,CAN_Mode_LoopBack);
//leak_Init();
	
	
	
	
	
	rt_hw_interrupt_enable(level);
    
    // ������̬�߳�
    rt_thread_init(&led0_thread,                 	//�߳̿��ƿ�
                   "led0",                       	//�߳����֣���shell������Կ���
                   led0_thread_entry,            	//�߳���ں���
                   RT_NULL,                     	//�߳���ں�������
                   &rt_led0_thread_stack[0],     	//�߳�ջ��ʼ��ַ
                   sizeof(rt_led0_thread_stack),  	//�߳�ջ��С
                   5,    //�̵߳����ȼ�
                   20);                         	//�߳�ʱ��Ƭ
    rt_thread_startup(&led0_thread);             	//�����߳�led0_thread  
				   
	rt_thread_init(&led1_thread,                 	//�߳̿��ƿ�
                   "led1",                      	//�߳����֣���shell������Կ���
                   led1_thread_entry,            	//�߳���ں���
                   RT_NULL,                     	//�߳���ں�������
                   &rt_led1_thread_stack[0],		//�߳�ջ��ʼ��ַ
                   sizeof(rt_led1_thread_stack), 	//�߳�ջ��С
                   4,    //�̵߳����ȼ�
                   20);         				   

	rt_thread_startup(&led1_thread);             //�����߳�led1_thread 
    
 
}



 



