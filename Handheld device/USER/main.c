#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "led.h"
#include "touch.h"
#include "gui.h"
#include "test.h"
#include "24l01.h"
#include "timer.h"
#include "24l01_2.h"
#include "keypad.h"
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
u16 ba_dian_x_last,ba_dian_y_last;
u16 ba_dian_x,ba_dian_y;
u16 length;  //<----�е㵽Բ�ĵľ���
double jiaozhi;
extern u16 key_val;
u16 a,b;
u8 tmp_buf[7];
u8 tmp_buf_2[7]={'0','0','0','0','0','0',0};//<---����ͷ�������飬ֻҪ����λ
u16 dian_flag=1;
u16 shoudong_flag=1;
u16 ba_dian_flag=1;

void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//����ж�
	{
	 if(dian_flag==1)
	 {
		if(ba_dian_flag==1)
		{
		gui_circle(ba_dian_x,ba_dian_y,RED,2,1);
    dian_flag=0;
		}			
	 }
   else
	 {
		 if(ba_dian_flag==1)
		{
		gui_circle(ba_dian_x,ba_dian_y,WHITE,2,1);
    dian_flag=1;
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

int main(void)
{	u16 i=0;
  u8 mode;
  u16 t=0;			 
	SystemInit();//��ʼ��RCC ����ϵͳ��ƵΪ72MHZ
	delay_init();	     //��ʱ��ʼ��
	LED_Init();
	LCD_Init();
	NRF24L01_Init();  //<---���ڽ�������
	NRF24L01_Init_2();  //<---���ڷ�������
	Init_Keypad();    //��ʼ������
	TIM3_Int_Init(4999,7199);//10Khz�ļ���Ƶ�ʣ�����5K��Ϊ500ms 
  	while(NRF24L01_Check())	//���NRF24L01�Ƿ���λ.	
	{
		Gui_StrCenter(0,30,RED,BLUE,"NRF24L01 Error",16,0);//������ʾ	
		delay_ms(200);
	}
    Gui_StrCenter(0,30,RED,BLUE,"NRF24L01 OK",16,0);	
	  NRF24L01_RX_Mode();
	
	while(NRF24L01_Check_2())	//���NRF24L01�Ƿ���λ.	
	{
		Gui_StrCenter(0,30,RED,BLUE,"NRF24L01*2 Error",16,0);//������ʾ	
		delay_ms(200);
	}
    Gui_StrCenter(0,30,RED,BLUE,"NRF24L01*2 OK",16,0);	
	  NRF24L01_TX_Mode_2();
	
	
	  LCD_Fill(0,0,239,239,WHITE);
	  LCD_Fill(240,0,319,239,PINK);		
		gui_circle(119,119,BLACK,17,0);
    gui_circle(119,119,BLACK,34,0);
    gui_circle(119,119,BLACK,51,0);
    gui_circle(119,119,BLACK,68,0);
    gui_circle(119,119,BLACK,85,0);
    gui_circle(119,119,BLACK,102,0);
//	while(1)
//		{
//      if(key_flag())
//			{
//				Check_Key();
//				tmp_buf_2[0]=key_val/10+'0';
//				tmp_buf_2[1]=key_val%10+'0';
//				Show_Str(0,20,RED,BLUE,tmp_buf_2,16,0);
//				while(NRF24L01_TxPacket_2(tmp_buf_2)==TX_OK);//<---ֱ�����ͳɹ��˳�
//			}	
//		}
   Show_Str(240,180,BLUE,PINK,"�ֶ�ģʽ",16,0);
	 while(1)
		{
      if(key_flag())
			{
				Check_Key();
				if(key_val==15)
				{
					shoudong_flag=0;
					Show_Str(240,180,BLUE,PINK,"�Զ�ģʽ",16,0);
				}
				if(key_val==16)
				{
					shoudong_flag=1;
					Show_Str(240,180,BLUE,PINK,"�ֶ�ģʽ",16,0);
				}
				tmp_buf_2[0]=key_val/10+'0';
				tmp_buf_2[1]=key_val%10+'0';				
				if(NRF24L01_TxPacket_2(tmp_buf_2)==TX_OK)//<---ֱ�����ͳɹ��˳�
				{
					Show_Str(240,200,BLUE,PINK,"���ͳɹ�",16,0);
				}
				else
				{
					Show_Str(240,200,BLUE,PINK,"����ʧ��",16,0);
				}
				
			}		
			if(NRF24L01_RxPacket(tmp_buf)==0)//һ�����յ���Ϣ,����ʾ����.
			{
				ba_dian_x=(tmp_buf[0]-'0')*100+(tmp_buf[1]-'0')*10+(tmp_buf[2]-'0');
				ba_dian_y=(tmp_buf[3]-'0')*100+(tmp_buf[4]-'0')*10+(tmp_buf[5]-'0');
				if(ba_dian_x==0&&ba_dian_y==0)
				{
					ba_dian_flag=0;
				}
				else
				{
					ba_dian_flag=1;
				}
				//ˢ�½���
			//if((ba_dian_x==ba_dian_x_last||ba_dian_x==ba_dian_x_last+1||ba_dian_x==ba_dian_x_last-1)&&(ba_dian_y==ba_dian_y_last||ba_dian_y==ba_dian_y_last+1||ba_dian_y==ba_dian_y_last-1))//���ֵû�ı�ʲô������
			   if(ba_dian_x==ba_dian_x_last&&ba_dian_y==ba_dian_y_last)
				{
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
//       Show_Str(240,135,BLUE,PINK,tmp_buf,16,0);
		 if(ba_dian_x>320)
		 {
			 Gui_StrCenter(0,30,RED,BLUE,"error",16,1);//������ʾ
		 }
		 if(ba_dian_y>240)
		 {
			 Gui_StrCenter(0,100,RED,BLUE,"error",16,1);//������ʾ
		 }
		 if(ba_dian_flag==1)
		 {
			 gui_circle(ba_dian_x,ba_dian_y,RED,2,1);
		 }
//����red_x1,red_y1�Ĳ�ֵͬ����ʾ��Ӧ����<--���ݰе㵽Բ�ĵľ���
		 if(ba_dian_x>=119&&ba_dian_y>=119)
		 {
		 length=(ba_dian_x-119)*(ba_dian_x-119)+(ba_dian_y-119)*(ba_dian_y-119);
		 }
		 if(ba_dian_x<=119&&ba_dian_y>=119)
		 {
		 length=(119-ba_dian_x)*(119-ba_dian_x)+(ba_dian_y-119)*(ba_dian_y-119);
		 }
		 if(ba_dian_x<=119&&ba_dian_y<=119)
		 {
		 length=(119-ba_dian_x)*(119-ba_dian_x)+(119-ba_dian_y)*(119-ba_dian_y);
		 }
		 if(ba_dian_x>=119&&ba_dian_y<=119)
		 {
		 length=(ba_dian_x-119)*(ba_dian_x-119)+(119-ba_dian_y)*(119-ba_dian_y);
		 }
		 if(length<=17*17)
		 {
     Show_Str(240,0,BLUE,PINK,"ʮ��",32,0);//������ɫ
		 }
     else if(length<=34*34)
		 {
     Show_Str(240,0,BLUE,PINK,"�Ż�",32,0);//������ɫ
		 }
     else if(length<=51*51)
		 {
     Show_Str(240,0,BLUE,PINK,"�˻�",32,0);//������ɫ
		 }
     else if(length<=68*68)
		 {
     Show_Str(240,0,BLUE,PINK,"�߻�",32,0);//������ɫ
		 }
     else if(length<=85*85)
		 {
     Show_Str(240,0,BLUE,PINK,"����",32,0);//������ɫ
		 }
     else if(length<=102*102)
		 {
     Show_Str(240,0,BLUE,PINK,"�廷",32,0);//������ɫ
		 }
     else
		 {
		 Show_Str(240,0,BLUE,PINK,"�Ѱ�",32,0);//������ɫ
		 }
//�������ж����ϣ����ĵ�������ֱ�Ӹ����ͱȽ�
		 if(length>102*102)//�Ѱв���ʾ��λ��Ϣ
		 {
		 }
		 else
		 {
		 if(ba_dian_x==119&&ba_dian_y==119)
		 {
			Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ 
		 }
		 if(ba_dian_x>119&&ba_dian_y<119)//��������
		 {
			 jiaozhi=(double)(119-ba_dian_y)/(ba_dian_x-119);
			 if(jiaozhi<0.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ 
			 }
			 else if(jiaozhi<2.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ  
			 }
			 else
			 {
				Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ   
			 }
		 }
		 if(ba_dian_x<119&&ba_dian_y<119)//��������
		 {
			 jiaozhi=(double)(119-ba_dian_y)/(119-ba_dian_x);
			 if(jiaozhi<0.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ 
			 }
			 else if(jiaozhi<2.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ  
			 }
			 else
			 {
				Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ   
			 }
		 }
		 if(ba_dian_x<119&&ba_dian_y>119)//��������
		 {
			 jiaozhi=(double)(ba_dian_y-119)/(119-ba_dian_x);
			 if(jiaozhi<0.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ 
			 }
			 else if(jiaozhi<2.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ  
			 }
			 else
			 {
				Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ   
			 }
		 }
		 if(ba_dian_x>119&&ba_dian_y>119)//��������
		 {
			 jiaozhi=(double)(ba_dian_y-119)/(ba_dian_x-119);
			 if(jiaozhi<0.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ 
			 }
			 else if(jiaozhi<2.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ  
			 }
			 else
			 {
				Show_Str(240,45,BLUE,PINK,"����",32,0);//������ɫ   
			 }
		 }
	 }
		   if(shoudong_flag==1)
				{
					Show_Str(240,180,BLUE,PINK,"�ֶ�ģʽ",16,0);
				}
				else
				{
					Show_Str(240,180,BLUE,PINK,"�Զ�ģʽ",16,0);
				}
			
		 ba_dian_x_last=ba_dian_x;
		 ba_dian_y_last=ba_dian_y;
	  }
		 //ˢ�½���
			}//<---���յ�����
			else//û���յ����������û�к�㣬���Ӳ���ʾ
			{
				
		  }		
		}


 }

