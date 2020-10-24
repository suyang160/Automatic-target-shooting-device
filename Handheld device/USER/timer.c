#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/4
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20120904
//1,����TIM3_PWM_Init������
//2,����LED0_PWM_VAL�궨�壬����TIM3_CH2����									  
//////////////////////////////////////////////////////////////////////////////////  
//TIM1 PWM���ֳ�ʼ�� 
//PWM�����ʼ��                 //<----�͸ĸú���ΪTIM1
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM1_PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB2ENR|=1<<11; 	//TIM1ʱ��ʹ��    
	RCC->APB2ENR|=1<<2;    	//ʹ��PORTAʱ��	
	GPIOA->CRH&=0XFFFFFFF0;	//PA8���
	GPIOA->CRH|=0X0000000B;	//���ù������
  GPIOA->ODR|=1<<8;	
	   
	RCC->APB2ENR|=1<<0;     //��������ʱ�� <---AFIO��ʱ��	   
	AFIO->MAPR&=0XFFFFFF3F; //���MAPR��[6:7],û����ӳ��

	TIM1->ARR=arr;			//�趨�������Զ���װֵ 
	TIM1->PSC=psc;			//Ԥ��Ƶ������Ƶ
	                    //PA8��Ӧͨ��1
	TIM1->CCMR1|=7<<4;  	//CH1 PWM2ģʽ		 
	TIM1->CCMR1|=1<<3; 	  //CH1Ԥװ��ʹ��

	TIM1->BDTR=0X8000;   //����PWM�����<---ò��timer1Ҫ������<---����why,�Լ����ֲ�
	TIM1->CCER|=1<<0;   	//OC1 ���ʹ��	   
	TIM1->CR1=0x0080;   	//ARPEʹ�� 
	TIM1->CR1|=0x01;    	//ʹ�ܶ�ʱ��1 											  
} 


//TIM4 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM4_PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB1ENR|=1<<2; 	//TIM4ʱ��ʹ��    
	RCC->APB2ENR|=1<<3;    	//ʹ��PORTBʱ��	
	GPIOB->CRL&=0X00FFFFFF;	//PB6,PB7���
	GPIOB->CRL|=0XBB000000;	//���ù������ 	  	 
	   
	RCC->APB2ENR|=1<<0;     //��������ʱ��	   
	AFIO->MAPR&=0XFFFFF3FF; //���MAPR��[11:10]--->û����ӳ��

	TIM4->ARR=arr;			//�趨�������Զ���װֵ 
	TIM4->PSC=psc;			//Ԥ��Ƶ������Ƶ
	
	TIM4->CCMR1|=6<<4;  	//CH1 PWM2ģʽ		 
	TIM4->CCMR1|=1<<3;  	//CH1Ԥװ��ʹ��	
	TIM4->CCMR1|=6<<12;  	//CH2 PWM2ģʽ		 
	TIM4->CCMR1|=1<<11; 	//CH2Ԥװ��ʹ��	
	
	TIM4->CCER|=1<<0;   	//OC1 ���ʹ��
	TIM4->CCER|=1<<4;   	//OC2 ���ʹ��	   
	TIM4->CR1=0x0080;   	//ARPEʹ�� 
	TIM4->CR1|=0x01;    	//ʹ�ܶ�ʱ��3 											  
}  	 











