#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"
#include "test.h"
#include "ov7670.h"
#include "timer.h"
#include "keypad.h"
#include "24l01.h"
#include "spi.h"
#include "24l01_2.h"
#include "spi_2.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F103RBT6,����ԭ��MiniSTM32������,��Ƶ72MHZ
//QDtech-TFTҺ������ for STM32 IOģ��
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtech.net
//�Ա���վ��http://qdtech.taobao.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ
//�̻�(����) :+86 0755-23594567 
//�ֻ�:15989313508���빤�� 
//����:QDtech2008@gmail.com 
//Skype:QDtech2008
//��������QQȺ:324828016
//��������:2013/5/13
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2009-2019
//All rights reserved
/****************************************************************************************************
//=======================================Һ���������߽���==========================================//
//DB0       ��PD14 
//DB1       ��PD15 
//DB2       ��PD0 
//DB3       ��PD1 
//DB4~DB12  ���ν�PE7~PE15
//DB13      ��PD8 
//DB14      ��PD9
//DB15      ��PD10  
//=======================================Һ���������߽���==========================================//
//LCD_CS	��PG12	//Ƭѡ�ź�
//LCD_RS	��PG0	//�Ĵ���/����ѡ���ź�
//LCD_WR	��PD5	//д�ź�
//LCD_RD	��PD4	//���ź�
//LCD_RST	��PC5	//��λ�ź�
//LCD_LED	��PB0	//��������ź�(�ߵ�ƽ����)
//=========================================������������=========================================//
//��ʹ�ô�������ģ�鱾������������ɲ�����
//MO(MISO)	��PF8	//SPI�������
//MI(MOSI)	��PF9	//SPI��������
//PEN		��PF10	//�������ж��ź�
//TCS		��PB2	//����ICƬѡ
//CLK		��PB1	//SPI����ʱ��
**************************************************************************************************/	
#define jiguang  PAout(12)
extern u16 key_val;
extern u8 ov_sta=0;	//��exit.c���涨��
u8 jiao_zhen=0;  //<----�����ûУ���ã���Ϊ0;У������Ϊ1(����)<---����Ϊ1������һֱУ��
u16 x1,y1;
u16 x2,y2;
u16 x3,y3;
u16 x4,y4;           //<---�ĸ���λ�����������
u16 x1_temp,y1_temp;
u16 x2_temp,y2_temp;
u16 x3_temp,y3_temp;
u16 x4_temp,y4_temp;           //<---�ĸ���λ�����ʱ��������<--ò���кܶ���ʱ�ò���
u16 count1;
u16 count2;
u16 count3;
u16 count4;                   //�ֱ�Ϊ�ĸ�����ӵĵ�ĸ���<---��������ƽ��
u16 red_x1,red_y1;
u16 red_x1_temp,red_y1_temp;
u16 red_count;
u16 ba_dian_x,ba_dian_y;
u32 i_temp;                 //<---���ڲ���
u32 j_temp;
u32 color_temp;
u32 i_red;                 //<---���ڲ���
u32 j_red;
u32 color_red;
u8  a;
u16 red_max;
u8 kongping_flag;//<---��flag�������޼����ʱ��ˢ��
u32 length;  //<----�е㵽Բ�ĵľ���
u16 PWM_temp1=1440;
u16 PWM_temp2=1220;
double jiaozhi;
u16 shoudong_flag=1;//<----��ʼ״̬�£���ֵΪ1���������ֶ�״̬
u8 tmp_buf[7]={'0','0','0','0','0','0',0};//<---���ǳ�ʼ��״̬���ֳֶ˵�Ƭ�����յ������ݱ�֪��û��ɨ�赽���
u8 tmp_buf_2[7];//<---����ͷ�������飬ֻҪ����λ
u16 t;                            //ʹ�ܽ������t
u8 jiguang_flag;//��Ϊһʱ�������
u8 xiangxian_flag;//<---1�����һ��������2����ڶ���������

//��������11�������ʹ��TIM3ʱ�ӣ�����t�ڳ������Զ����;
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//����ж�
	{
		if(jiguang_flag==1)
		{
			 t++;//û����һ��500ms
			 if(t<=8)
			 {
			 jiguang=0;//<---��������
			 }
			 else
			 {
			 jiguang=1;//<---Ϩ��
			 jiguang_flag=0;
			 t=0;     
			 } 
		 }			 
	}				   
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms    
	TIM3->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��		  
	TIM3->DIER|=1<<0;   //��������ж�	  
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  MY_NVIC_Init(1,3,0x1D,2);//��ռ1�������ȼ�3����2									 
}

void chuansong();
void  length_jisuan();
void EXTI8_Init(void)
{												  
	Ex_NVIC_Config(GPIOA,8,2);		 		//�����ش���			  
	MY_NVIC_Init(0,0,0x17,2);		//��ռ0,�����ȼ�0����2	   
}
void camera_refresh(void);
void EXTI9_5_IRQHandler(void)
{		 		
	if(EXTI->PR&(1<<8))//��8�ߵ��ж�
	{     
		if(ov_sta<2)
		{
			if(ov_sta==0)
			{
				OV7670_WRST=0;	 	//��λдָ��		  		 
				OV7670_WRST=1;	
				OV7670_WREN=1;		//����д��FIFO
			}else 
			{OV7670_WREN=0;
			 OV7670_WRST=0;	 	//��λдָ��		  		 
			 OV7670_WRST=1;		//��ֹд��FIFO 
     //  camera_refresh();			
			} 
			ov_sta++;
		}
	}
	EXTI->PR=1<<8;     //���LINE8�ϵ��жϱ�־λ						  
}

	


 void camera1_refresh(void)
{
	u32 i;//<---��ʾ����
	u32 j;//<---��ʾ����
 	u16 color;
	if(a==1)
	{
	 LCD_Fill(0,0,239,239,WHITE);   
	 gui_circle(119,119,BLACK,17,0);
	 gui_circle(119,119,BLACK,34,0);
	 gui_circle(119,119,BLACK,51,0);
	 gui_circle(119,119,BLACK,68,0);
	 gui_circle(119,119,BLACK,85,0);
	 gui_circle(119,119,BLACK,102,0);
		a=0;
//		Show_Str(240,45,BLUE,PINK,"jinqu",32,0);
	}
  if(jiao_zhen==0)//������У��״̬
  {
		if(ov_sta==2)
		{
		LCD_SetCursor(0x00,0x0000);	 
		LCD_WriteRAM_Prepare(); 
		OV7670_RRST=0;				//��ʼ��λ��ָ�� 
		OV7670_RCK=0;
		OV7670_RCK=1;
		OV7670_RCK=0;
		OV7670_RRST=1;				//��λ��ָ����� 
		OV7670_RCK=1;
		for(i=0;i<240;i++)
		{
			for(j=0;j<320;j++)
			{
				OV7670_RCK=0;
				__nop();__nop();
				color=GPIOC->IDR&0XFF;	//������
				OV7670_RCK=1;
				__nop();__nop(); 			
				color<<=8;  
				OV7670_RCK=0;
				__nop();__nop();
				color|=GPIOC->IDR&0XFF;	//������
				OV7670_RCK=1;
				LCD->LCD_RAM=color;
				if(i<235&&i>5&&j>10&&j<310)//<---���Ͻ�
				{					
					if(color>=60000&&color<=65000)//<---�����˺��
					{
						i_temp=i;
						j_temp=j;
						color_temp=color;
					}
			  }
        if(i<120&&i>5&&j>5&&j<160)//<---���Ͻ�<---ͼ��������ΧһȦ����
				{					
					if(color<30000)//<---�����˺ڵ�
					{
						x1_temp+=j;
						y1_temp+=i;
						count1++;
					}
			  }
			  if(i<120&&i>5&&j>160&&j<315)
				{
					if(color<30000)//<---�����˺ڵ�
					{
						x2_temp+=j;
						y2_temp+=i;
						count2++;
					}
				}
				if(i>120&&i<235&&j<160&&j>5)
				{
					if(color<30000)//<---�����˺ڵ�
					{
						x3_temp+=j;
						y3_temp+=i;
						count3++;
					}
				}
				if(i>120&&i<235&&j>160&&j<315)
				{
					if(color<30000)//<---�����˺ڵ�
					{
						x4_temp+=j;
						y4_temp+=i;
						count4++;
					}
				}
        if(i==239&&j==319)
				{
					x1_temp=x1_temp/count1;
					y1_temp=y1_temp/count1;
					x2_temp=x2_temp/count2;
					y2_temp=y2_temp/count2;
					x3_temp=x3_temp/count3;
					y3_temp=y3_temp/count3;
					x4_temp=x4_temp/count4;
					y4_temp=y4_temp/count4;
          if(((x1_temp<x3_temp&&x3_temp-x1_temp<2)||(x3_temp<x1_temp&&x1_temp-x3_temp<2))&&((y1_temp<y2_temp&&y2_temp-y1_temp<2)||(y2_temp<y1_temp&&y1_temp-y2_temp<2)))//<---У�����ж�����	
					{
					jiao_zhen=1;
          x1=x1_temp;y1=y1_temp;
          x2=x2_temp;y2=y2_temp;
          x3=x3_temp;y3=y3_temp;            //�����ȷ��
          x4=x4_temp;y4=y4_temp;
          a=1;						
					}
           else	//�����ǰֵ�Ǵ����
					{
						  LCD_ShowNum(20,20,x1_temp,3,12);
      			  LCD_ShowNum(20,40,y1_temp,3,12);
						  LCD_ShowNum(160,20,x2_temp,3,12);
      			  LCD_ShowNum(160,40,y2_temp,3,12);
						  LCD_ShowNum(20,120,x3_temp,3,12);
      			  LCD_ShowNum(20,140,y3_temp,3,12);
						  LCD_ShowNum(160,120,x4_temp,3,12);
      			  LCD_ShowNum(160,140,y4_temp,3,12);
          						
						 x1_temp=0;y1_temp=0;
						 x2_temp=0;y2_temp=0;
						 x3_temp=0;y3_temp=0;          //����ʱֵ����
						 x4_temp=0;y4_temp=0;
             count1=0;count2=0;
             count3=0;count4=0;						
					}
//          LCD_ShowNum(20,20,i_temp,3,12);
//				  LCD_ShowNum(20,40,j_temp,3,12);
//					LCD_ShowNum(20,60,color_temp,5,12);
					color_temp=0;
          i_temp=0;
          j_temp=0;//<---����־λ����  					
				}					
			}
		}//<---forѭ���ڲ�			
		EXTI->PR=1<<8;     			//���LINE8�ϵ��жϱ�־λ
		ov_sta=0;					//��ʼ��һ�βɼ
	}
}//<---jiaozhen_flag
 else
 { 
		if(ov_sta==2)
		{
			OV7670_RRST=0;				//��ʼ��λ��ָ�� 
			OV7670_RCK=0;
			OV7670_RCK=1;
			OV7670_RCK=0;
			OV7670_RRST=1;				//��λ��ָ����� 
			OV7670_RCK=1;			
	   for(i=0;i<240;i++)
		{
			for(j=0;j<320;j++)
			{
				if(i==0&&j==0)
				{
					red_x1=0;
					red_y1=0;
					red_count=0;
				}
				OV7670_RCK=0;
				__nop();__nop();
				color=GPIOC->IDR&0XFF;	//������
				OV7670_RCK=1;
				__nop();__nop(); 			
				color<<=8;  
				OV7670_RCK=0;
				__nop();__nop();
				color|=GPIOC->IDR&0XFF;	//������
				OV7670_RCK=1;
        if(j>x1&&j<x2&&i>y1&&i<y3)//�����ڷ�Χ��
				{					
//					if(color>=60000&&color<=65536)
//					{
//						if(color>red_max)
//						{
//							red_max=color;
//							red_x1=j;
//							red_y1=i;
//						}
////						red_x1_temp+=j;
////						red_y1_temp+=i;
////						red_count++;
////						red_x1=j;
////  					red_y1=i;
//					}
					if(color==65535)
					{
						red_x1+=j;
						red_y1+=i;
						red_count++;
					}
				}
        if(i==239&&j==319)
				{
//					LCD_ShowNum(20,120,red_max,5,12);
					red_x1=red_x1/red_count;
					red_y1=red_y1/red_count;
//					red_x1=red_x1_temp/red_count;
//					red_y1=red_y1_temp/red_count;
//			 		//<--ת�ƣ���x1,y1=0,��ûɨ����㣬����ʾ
//          red_x1_temp=0;
//          red_x1_temp=0;					
//          red_count=0;
//          LCD_ShowNum(20,20,red_x1,3,12);
//				  LCD_ShowNum(20,40,red_y1,3,12);         					
				}					
			}   							 
		 }//<---forѭ�������
		 EXTI->PR=1<<8;     			//���LINE8�ϵ��жϱ�־λ
		 ov_sta=0;					//��ʼ��һ�βɼ�
	  }
 }//<--else
 if(jiao_zhen==1)//������У��״̬
{

	if(red_x1==0&&red_y1==0)//ûɨ������ʾ�õ�<--�����������ʾǰ������
	{
   ba_dian_x=0;
	 ba_dian_y=0;//<---���û��ɨ�赽��㣬����00
	}
  else
	{
		 LCD_Fill(0,0,239,239,WHITE);
	   LCD_Fill(240,0,319,239,PINK);		
		 gui_circle(119,119,BLACK,17,0);
     gui_circle(119,119,BLACK,34,0);
     gui_circle(119,119,BLACK,51,0);
     gui_circle(119,119,BLACK,68,0);
     gui_circle(119,119,BLACK,85,0);
     gui_circle(119,119,BLACK,102,0);
		 ba_dian_x=(red_x1-x1)*204/(x2-x1)+17;
		 ba_dian_y=(red_y1-y1)*204/(y3-y1)+17;
//		 if(ba_dian_x>320)
//		 {
//			 Gui_StrCenter(0,30,RED,BLUE,"error",16,1);//������ʾ
//		 }
//		 if(ba_dian_y>240)
//		 {
//			 Gui_StrCenter(0,100,RED,BLUE,"error",16,1);//������ʾ
//		 }
//		 gui_circle(ba_dian_x,ba_dian_y,RED,2,1);
		
		 LCD_ShowNum(20,20,ba_dian_x,3,12);
		 LCD_ShowNum(20,40,ba_dian_y,3,12);
		 LCD_ShowNum(20,120,key_val,3,12);
		 LCD_ShowNum(20,160,length,5,12);
//����red_x1,red_y1�Ĳ�ֵͬ����ʾ��Ӧ����<--���ݰе㵽Բ�ĵľ���
//			LCD_ShowNum(20,20,i_red,3,12);
//			LCD_ShowNum(20,40,j_red,3,12);
//			LCD_ShowNum(20,60,color_red,5,12);
//			color_red=0;
//			i_red=0;
//			j_red=0;//<---����־λ���� 	
	}		
}

 
}
void camera2_refresh(void)//<---���麯�������⼤����е���ɫֵ�����Ժ���
{
	u32 i;
	u32 j;
 	u16 color;	 
	if(ov_sta==2)
	{
		LCD_SetCursor(0x00,0x0000);	 
		LCD_WriteRAM_Prepare();    
		OV7670_RRST=0;			
		OV7670_RCK=0;
		OV7670_RCK=1;
		OV7670_RCK=0;
		OV7670_RRST=1;	
		OV7670_RCK=1;
    for(i=0;i<240;i++)
    {		
			for(j=0;j<320;j++)
			{
				OV7670_RCK=0;
				color=GPIOC->IDR&0XFF;	
				OV7670_RCK=1; 
				color<<=8;  
				OV7670_RCK=0;
				color|=GPIOC->IDR&0XFF;
				OV7670_RCK=1; 
				LCD->LCD_RAM=color;
        if(i==10&&j==10)
				{
        color_temp=color;			
				}					
			}
		}
        LCD_ShowNum(20,20,color_temp,5,12);			
		EXTI->PR=1<<8;     			
		ov_sta=0;			
	} 
}
void camera_refresh(void)
{
	u32 j;
 	u16 color;	 
	if(ov_sta==2)
	{
		LCD_SetCursor(0x00,0x0000);	 
		LCD_WriteRAM_Prepare();    
		OV7670_RRST=0;			
		OV7670_RCK=0;
		OV7670_RCK=1;
		OV7670_RCK=0;
		OV7670_RRST=1;	
		OV7670_RCK=1;  
		for(j=0;j<76800;j++)
		{
			OV7670_RCK=0;
			color=GPIOC->IDR&0XFF;	
			OV7670_RCK=1; 
			color<<=8;  
			OV7670_RCK=0;
			color|=GPIOC->IDR&0XFF;
			OV7670_RCK=1; 
			LCD->LCD_RAM=color;    
		}   							 
		EXTI->PR=1<<8;     			
		ov_sta=0;			
	} 
}

int main(void)
{	
	u16 i=0;
	SystemInit ();
	delay_init();	     
  LCD_Init();	
	OV7670_Init();
	NRF24L01_Init();          //��ʼ�����͵���������
  NRF24L01_Init_2();    	//��ʼ��NRF24L01<---���հ���ֵ
  TIM3_Int_Init(4999,7199);//10Khz�ļ���Ƶ�ʣ�����5K��Ϊ500ms  
	TIM1_PWM_Init(20000,71);	//1MHZ�ķ�Ƶ��ÿ��������1us,200000������20ms
	TIM1->CCR3=PWM_temp1;       //PA10       
	TIM1->CCR4=PWM_temp2;       //PA11
 	Init_Keypad();
	while(OV7670_Init())//��ʼ��OV7670
	{
		Gui_StrCenter(0,60,RED,BLUE,"OV7670 Error!!",16,1);
		delay_ms(200);
	}
    Gui_StrCenter(0,60,RED,BLUE,"OV7670 Init OK",16,1);
    EXTI8_Init();
	  OV7670_Window_Set(10,174,240,320);
 	  OV7670_CS=0;
    LCD_Fill(0,0,239,239,WHITE);
	  LCD_Fill(239,0,319,239,BLUE);
	
		while(NRF24L01_Check())	//��鷢�͵�NRF24L01�Ƿ���λ.	
	{
		Gui_StrCenter(0,80,RED,BLUE,"NRF24L01 Init error",16,1);
		delay_ms(200);
	}
	 Gui_StrCenter(0,80,RED,BLUE,"NRF24L01 Init OK",16,1);
	 	NRF24L01_TX_Mode();
	
  	while(NRF24L01_Check_2())	//�����յ�NRF24L01�Ƿ���λ.	
	{
		Gui_StrCenter(0,80,RED,BLUE,"NRF24L01*2 Init error",16,1);
		delay_ms(200);
	}
	 Gui_StrCenter(0,80,RED,BLUE,"NRF24L01*2 Init OK",16,1);
	 	NRF24L01_RX_Mode_2();


    RCC->APB2ENR|=1<<2;     //ʹ��Aʱ��
    GPIOA->CRH&=0XFFF0FFFF;
    GPIOA->CRH|=0X00030000;
    GPIOA->ODR&=0X11101111;  //��PA12�õͣ�PA12�ߵ�ƽʱ����	
    Show_Str(240,120,BLUE,PINK,"shoudong",32,0);//<---��ʼ���ֶ�ģ�
	
	 while(1)
	 {
		camera1_refresh();
    chuansong();
		length_jisuan();
//		if(shoudong_flag==0)//<---�ô����Ǳ�֤���Զ�ģʽ�´��׼ȷ
//		{
//			if(key_val==1)
//			{
//				if(length>=0&&length<=289)
//				{}
//			  else
//				{
//					goto loop1;
//				}
//			}
//			 if(key_val==2)
//			{
//				if(length>289&&length<=1156)
//				{}
//			  else
//				{
//					goto loop2;
//				}
//			}
//			if(key_val==3)//<--�˻�
//			{
//				if(length>1156&&length<=2601)
//				{}
//			  else
//				{
//					goto loop3;
//				}
//			}
//			if(key_val==4)
//			{
//				if(length>2601&&length<=4624)
//				{}
//			  else
//				{
//					goto loop4;
//				}
//			}
//			if(key_val==5)
//			{
//				if(length>4624&&length<=7225)
//				{}
//			  else
//				{
//					goto loop5;
//				}
//			}
//			if(key_val==6)
//			{
//				if(length>7225&&length<=10404)
//				{}
//			  else
//				{
//				 goto loop6;
//				}
//			}						
//		 }
		  if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//һ�����յ���Ϣ,����ʾ����.
		 {
			 key_val=(tmp_buf_2[0]-'0')*10+(tmp_buf_2[1]-'0');
       if(shoudong_flag==1)
			{//<---�ֶ�ģʽ��				
				if(key_val==9)//PWM_temp1��ֵ1500
				{
					PWM_temp1+=5;
					TIM1->CCR3=PWM_temp1;       //PB6 <--������ת
					LCD_ShowNum(20,20,PWM_temp1,4,12);  
				}
				if(key_val==10)
				{
					PWM_temp1-=5;
					TIM1->CCR3=PWM_temp1;       //PB6  <--������ת
					LCD_ShowNum(20,20,PWM_temp1,4,12); 
				}
				if(key_val==7)
				{
					PWM_temp2-=5;
					TIM1->CCR4=PWM_temp2;       //PB7  <---������ת 
					LCD_ShowNum(20,40,PWM_temp2,4,12); 					
				}
				if(key_val==8)
				{
					PWM_temp2+=5;
					TIM1->CCR4=PWM_temp2;       //PB7  <---������ת
					LCD_ShowNum(20,40,PWM_temp2,4,12);
				} 
        if(key_val==11)
				{
         jiguang_flag=1;
				}
				if(key_val==12)//<---�����˳�����ť�������һֱ������Ȼ������ĿҪ��Χ
				{
				jiguang=0; 
				}
				if(key_val==15)
				{
				 shoudong_flag=0;
         Show_Str(240,120,BLUE,PINK,"zidong",32,0);					
				}
			}//<---�ֶ�ģʽ��		
      else
			{
					if(key_val==1)//������ʮ����ť
				{
					 PWM_temp1=1440;
					 TIM1->CCR3=PWM_temp1;
           PWM_temp2=1220;       //�ȴ��Եĵ������λ��
					 TIM1->CCR4=PWM_temp2;
           jiguang=0;     //���������
           delay_ms(1000);//<---��ʱ������
					 delay_ms(1000);
					 delay_ms(1000);
					 camera1_refresh();
           jiguang=1;				
					
					loop1:		if(ba_dian_x>119+12)//<---�����ұߣ��������
					{
					PWM_temp1+=4;
					TIM1->CCR3=PWM_temp1;       	
					}
					if(ba_dian_x<119-12)//<---������ߣ����ҵ���
					{
					PWM_temp1-=4;
					TIM1->CCR3=PWM_temp1;       	
					}
					if(ba_dian_y<119-12)//<---�����ϱߣ����µ���
					{
					PWM_temp2+=4;
					TIM1->CCR4=PWM_temp2;       //PB7  <---������ת
					}
					if(ba_dian_y>119+12)//<---�����±ߣ����ϵ���
					{
					PWM_temp2-=4;
					TIM1->CCR4=PWM_temp2;       //PB7  <---������ת
					}
					
					delay_ms(500);						
					jiguang=0;     //���������
          delay_ms(1000);//<---��ʱ������
					 delay_ms(1000);
					 delay_ms(1000);					
					camera1_refresh();
          jiguang=1;	
					chuansong();
				 if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//�����Զ�ģʽ����
		    {
				 key_val=(tmp_buf_2[0]-'0')*10+(tmp_buf_2[1]-'0');
				 if(key_val==13)
				 {
					 goto loop7;
				 }
			  }
          if(ba_dian_x<131&&ba_dian_x>107&&ba_dian_y>107&&ba_dian_y<131)
					{}
          else
					{						
						goto loop1;
					}
          					
				}//<---ʮ����ť	
				if(key_val==2)//���¾Ż�
				{			
					jiguang=0;     //���������
//          delay_ms(1000);//<---��ʱ������
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop2: if(xiangxian_flag==1)
					{
						if(length>34*34)//<---�����ұߣ��������
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=17*17)//<---������ߣ����ҵ���
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					else
					{
						if(length>34*34)//<---������ߣ����ҵ���
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=17*17)//<---�����ұߣ��������
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					delay_ms(500);
					jiguang=0;     //���������
//          delay_ms(1000);//<---��ʱ������
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//�����Զ�ģʽ����
		      {
							key_val=(tmp_buf_2[0]-'0')*10+(tmp_buf_2[1]-'0');
						 if(key_val==13)
						 {
							 goto loop7;
						 }
			    }
					length_jisuan();
					LCD_ShowNum(20,160,length,5,12);
          if(length>17*17&&length<=34*34)
					{
						jiguang=1;     //Ϩ�𼤹��
					}
          else
					{						
						goto loop2;
					}
          					
				}//<---�Ż���ť	
					if(key_val==3)//���°˻�
				{
          jiguang=0;     //���������
//          delay_ms(1000);//<---��ʱ������
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop3: if(xiangxian_flag==1)
					{						
						if(length>51*51)//<---�����ұߣ��������
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=34*34)//<---������ߣ����ҵ���
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					else
					{
						if(length>51*51)
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=34*34)
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					delay_ms(500);	
					jiguang=0;     //���������
//          delay_ms(1000);//<---��ʱ������
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//�����Զ�ģʽ����
		      {
							key_val=(tmp_buf_2[0]-'0')*10+(tmp_buf_2[1]-'0');
						 if(key_val==13)
						 {
							 goto loop7;
						 }
			    }					
					length_jisuan();
					LCD_ShowNum(20,160,length,5,12);					
          if(length>34*34&&length<=51*51)
					{
							jiguang=1;     //Ϩ�𼤹��
					}
          else
					{						
						goto loop3;
					}
          					
				}//<---�˻���ť	
					if(key_val==4)//�����߻�
				{
          jiguang=0;     //���������
//          delay_ms(1000);//<---��ʱ������
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop4:	if(xiangxian_flag==1)
					{						
						if(length>68*68)//<---�����ұߣ��������
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=51*51)//<---������ߣ����ҵ���
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
				  }
					else
					{
						 if(length>68*68)//<---�����ұߣ��������
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=51*51)//<---������ߣ����ҵ���
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					delay_ms(500);	
					jiguang=0;     //���������
//          delay_ms(1000);//<---��ʱ������
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//�����Զ�ģʽ����
		      {
							key_val=(tmp_buf_2[0]-'0')*10+(tmp_buf_2[1]-'0');
						 if(key_val==13)
						 {
							 goto loop7;
						 }
			    }					
					length_jisuan();
					LCD_ShowNum(20,160,length,5,12);						
          if(length>51*51&&length<=68*68)
					{
							jiguang=1;     //Ϩ�𼤹��
					}
          else
					{						
						goto loop4;
					}
          					
				}//<---�߻���ť	
					if(key_val==5)//��������
				{
          jiguang=0;     //���������
//          delay_ms(1000);//<---��ʱ������
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop5:	if(xiangxian_flag==1)
					{						
						if(length>85*85)//<---�����ұߣ��������
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=68*68)//<---������ߣ����ҵ���
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					else
					{
						if(length>85*85)
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=68*68)
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}						
					}
					delay_ms(500);	
					jiguang=0;     //���������
//          delay_ms(1000);//<---��ʱ������
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//�����Զ�ģʽ����
		      {
							key_val=(tmp_buf_2[0]-'0')*10+(tmp_buf_2[1]-'0');
						 if(key_val==13)
						 {
							 goto loop7;
						 }
			    }					
					length_jisuan();
					LCD_ShowNum(20,160,length,5,12);						
          if(length>68*68&&length<=85*85)
					{
							jiguang=1;     //Ϩ�𼤹��
					}
          else
					{						
						goto loop5;
					}
          					
				}//<---������ť	
	     if(key_val==6)//�����廷
				{
          jiguang=0;     //���������
//          delay_ms(1000);//<---��ʱ������
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop6:	if(xiangxian_flag==1)
					{						
						if(length>102*102)//<---�����ұߣ��������
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=85*85)//<---������ߣ����ҵ���
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
				  }
					else
					{
						if(length>102*102)//<---�����ұߣ��������
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=85*85)//<---������ߣ����ҵ���
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					delay_ms(500);
					jiguang=0;     //���������
//          delay_ms(1000);//<---��ʱ������
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;	
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//�����Զ�ģʽ����
		      {
							key_val=(tmp_buf_2[0]-'0')*10+(tmp_buf_2[1]-'0');
						 if(key_val==13)
						 {
							 goto loop7;
						 }
			    }					
					length_jisuan();
					LCD_ShowNum(20,160,length,5,12);
          if(ba_dian_x==0&&ba_dian_y==0)
					{
						goto loop6;
					}						
          if(length>85*85&&length<=102*102)
					{
							jiguang=1;     //Ϩ�𼤹��
					}
          else
					{						
						goto loop6;
					}         					
				}//<---�廷��ť	
				loop7:  if(key_val==11)//<---�������������������ʱ��
				{
					jiguang_flag=1;
				}
				if(key_val==12)//<---�����˳�����ť�������һֱ������Ȼ������ĿҪ��Χ
				{
				jiguang=0; 
				}
				if(key_val==16)
				{
				 shoudong_flag=1;//��Ϊ�ֶ�ģʽ	
				 Show_Str(240,120,BLUE,PINK,"shoudong",32,0);	
				}
			}				
    }//���յ���ֵ
  }//<---while��ѭ��		
   
    			 
	
	
 }
void chuansong()
{
		if(jiao_zhen==1)//<---�Ѵ���У��״̬��//�������00���ʾû���յ����
		{
	     	tmp_buf[0]= ba_dian_x/100+'0';
				tmp_buf[1]= ba_dian_x%100/10+'0';
				tmp_buf[2]= ba_dian_x%10+'0';     //<---����Ϊ�е��������λ���ɸߵ���
				tmp_buf[3]= ba_dian_y/100+'0';
				tmp_buf[4]= ba_dian_y%100/10+'0';
				tmp_buf[5]= ba_dian_y%10+'0';     //<---����Ϊ�е��������λ���ɸߵ���			
				tmp_buf[6]=0;
				if(NRF24L01_TxPacket(tmp_buf)==TX_OK)//<---ֱ�����ͳɹ��˳�
				{
					Gui_StrCenter(0,30,RED,BLUE,"SUCC",16,0);//������ʾ	
				}
				else
				{
					Gui_StrCenter(0,30,RED,BLUE,"LOSE",16,0);//������ʾ	
				}			
	  }	
}
void  length_jisuan()//б�ߵ�ƽ��
{
		 if(ba_dian_x>=119&&ba_dian_y>=119)
		 {
		 length=(ba_dian_x-119)*(ba_dian_x-119)+(ba_dian_y-119)*(ba_dian_y-119);
		 xiangxian_flag=1;//��һ����
		 }
		 if(ba_dian_x<=119&&ba_dian_y>=119)
		 {
		 length=(119-ba_dian_x)*(119-ba_dian_x)+(ba_dian_y-119)*(ba_dian_y-119);
		 xiangxian_flag=1;
		 }
		 if(ba_dian_x<=119&&ba_dian_y<=119)
		 {
		 length=(119-ba_dian_x)*(119-ba_dian_x)+(119-ba_dian_y)*(119-ba_dian_y);
		 xiangxian_flag=2;
		 }
		 if(ba_dian_x>=119&&ba_dian_y<=119)
		 {
		 length=(ba_dian_x-119)*(ba_dian_x-119)+(119-ba_dian_y)*(119-ba_dian_y);
		 xiangxian_flag=2;
		 }
}

