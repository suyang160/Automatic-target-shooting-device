#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20120904
//1,增加TIM3_PWM_Init函数。
//2,增加LED0_PWM_VAL宏定义，控制TIM3_CH2脉宽									  
//////////////////////////////////////////////////////////////////////////////////  
//TIM1 PWM部分初始化 
//PWM输出初始化                 //<----就改该函数为TIM1
//arr：自动重装值
//psc：时钟预分频数
void TIM1_PWM_Init(u16 arr,u16 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB2ENR|=1<<11; 	//TIM1时钟使能    
	RCC->APB2ENR|=1<<2;    	//使能PORTA时钟	
	GPIOA->CRH&=0XFFFF00FF;	//PA10，11输出
	GPIOA->CRH|=0X0000BB00;	//复用功能输出
  GPIOA->ODR|=3<<10;	
	   
	RCC->APB2ENR|=1<<0;     //开启辅助时钟 <---AFIO的时钟	   
	AFIO->MAPR&=0XFFFFFF3F; //清除MAPR的[6:7],没有重映像

	TIM1->ARR=arr;			//设定计数器自动重装值 
	TIM1->PSC=psc;			//预分频器不分频
	                    //PA8对应通道1
	TIM1->CCMR2|=6<<4;  	//CH3 PWM1模式		 
	TIM1->CCMR2|=1<<3; 	  //CH3预装载使能
	TIM1->CCMR2|=6<<12;  	//CH4 PWM1模式		 
	TIM1->CCMR2|=1<<11; 	//CH4预装载使能	

	TIM1->BDTR=0X8000;   //设置PWM主输出<---貌似timer1要多加这句<---问我why,自己看手册
	TIM1->CCER|=1<<8;   	//OC1 输出使能
  TIM1->CCER|=1<<12;   	//OC1 输出使能	
	TIM1->CR1=0x0080;   	//ARPE使能 
	TIM1->CR1|=0x01;    	//使能定时器1 											  
} 


//TIM4 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM4_PWM_Init(u16 arr,u16 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB1ENR|=1<<2; 	//TIM4时钟使能    
	RCC->APB2ENR|=1<<3;    	//使能PORTB时钟	
	GPIOB->CRL&=0X00FFFFFF;	//PB6,PB7输出
	GPIOB->CRL|=0XBB000000;	//复用功能输出 	  	 
	   
	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
	AFIO->MAPR&=0XFFFFF3FF; //清除MAPR的[11:10]--->没有重映像

	TIM4->ARR=arr;			//设定计数器自动重装值 
	TIM4->PSC=psc;			//预分频器不分频
	
	TIM4->CCMR1|=6<<4;  	//CH1 PWM2模式		 
	TIM4->CCMR1|=1<<3;  	//CH1预装载使能	
	TIM4->CCMR1|=6<<12;  	//CH2 PWM2模式		 
	TIM4->CCMR1|=1<<11; 	//CH2预装载使能	
	
	TIM4->CCER|=1<<0;   	//OC1 输出使能
	TIM4->CCER|=1<<4;   	//OC2 输出使能	   
	TIM4->CR1=0x0080;   	//ARPE使能 
	TIM4->CR1|=0x01;    	//使能定时器3 											  
}  	 












