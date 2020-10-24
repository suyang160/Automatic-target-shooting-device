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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STM32F103RBT6,正点原子MiniSTM32开发板,主频72MHZ
//QDtech-TFT液晶驱动 for STM32 IO模拟
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtech.net
//淘宝网站：http://qdtech.taobao.com
//我司提供技术支持，任何技术问题欢迎随时交流学习
//固话(传真) :+86 0755-23594567 
//手机:15989313508（冯工） 
//邮箱:QDtech2008@gmail.com 
//Skype:QDtech2008
//技术交流QQ群:324828016
//创建日期:2013/5/13
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2009-2019
//All rights reserved
/****************************************************************************************************
//=======================================液晶屏数据线接线==========================================//
//DB0       接PD14 
//DB1       接PD15 
//DB2       接PD0 
//DB3       接PD1 
//DB4~DB12  依次接PE7~PE15
//DB13      接PD8 
//DB14      接PD9
//DB15      接PD10  
//=======================================液晶屏控制线接线==========================================//
//LCD_CS	接PG12	//片选信号
//LCD_RS	接PG0	//寄存器/数据选择信号
//LCD_WR	接PD5	//写信号
//LCD_RD	接PD4	//读信号
//LCD_RST	接PC5	//复位信号
//LCD_LED	接PB0	//背光控制信号(高电平点亮)
//=========================================触摸屏触接线=========================================//
//不使用触摸或者模块本身不带触摸，则可不连接
//MO(MISO)	接PF8	//SPI总线输出
//MI(MOSI)	接PF9	//SPI总线输入
//PEN		接PF10	//触摸屏中断信号
//TCS		接PB2	//触摸IC片选
//CLK		接PB1	//SPI总线时钟
**************************************************************************************************/	
u16 ba_dian_x_last,ba_dian_y_last;
u16 ba_dian_x,ba_dian_y;
u16 length;  //<----靶点到圆心的距离
double jiaozhi;
extern u16 key_val;
u16 a,b;
u8 tmp_buf[7];
u8 tmp_buf_2[7]={'0','0','0','0','0','0',0};//<---摄像头接收数组，只要用两位
u16 dian_flag=1;
u16 shoudong_flag=1;
u16 ba_dian_flag=1;

void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//溢出中断
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
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值//刚好1ms    
	TIM3->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  
	TIM3->DIER|=1<<0;   //允许更新中断	  
	TIM3->CR1|=0x01;    //使能定时器3
  MY_NVIC_Init(1,3,0x1D,2);//抢占1，子优先级3，组2									 
}

int main(void)
{	u16 i=0;
  u8 mode;
  u16 t=0;			 
	SystemInit();//初始化RCC 设置系统主频为72MHZ
	delay_init();	     //延时初始化
	LED_Init();
	LCD_Init();
	NRF24L01_Init();  //<---用于接收数据
	NRF24L01_Init_2();  //<---用于发送数据
	Init_Keypad();    //初始化键盘
	TIM3_Int_Init(4999,7199);//10Khz的计数频率，计数5K次为500ms 
  	while(NRF24L01_Check())	//检查NRF24L01是否在位.	
	{
		Gui_StrCenter(0,30,RED,BLUE,"NRF24L01 Error",16,0);//居中显示	
		delay_ms(200);
	}
    Gui_StrCenter(0,30,RED,BLUE,"NRF24L01 OK",16,0);	
	  NRF24L01_RX_Mode();
	
	while(NRF24L01_Check_2())	//检查NRF24L01是否在位.	
	{
		Gui_StrCenter(0,30,RED,BLUE,"NRF24L01*2 Error",16,0);//居中显示	
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
//				while(NRF24L01_TxPacket_2(tmp_buf_2)==TX_OK);//<---直到发送成功退出
//			}	
//		}
   Show_Str(240,180,BLUE,PINK,"手动模式",16,0);
	 while(1)
		{
      if(key_flag())
			{
				Check_Key();
				if(key_val==15)
				{
					shoudong_flag=0;
					Show_Str(240,180,BLUE,PINK,"自动模式",16,0);
				}
				if(key_val==16)
				{
					shoudong_flag=1;
					Show_Str(240,180,BLUE,PINK,"手动模式",16,0);
				}
				tmp_buf_2[0]=key_val/10+'0';
				tmp_buf_2[1]=key_val%10+'0';				
				if(NRF24L01_TxPacket_2(tmp_buf_2)==TX_OK)//<---直到发送成功退出
				{
					Show_Str(240,200,BLUE,PINK,"发送成功",16,0);
				}
				else
				{
					Show_Str(240,200,BLUE,PINK,"发送失败",16,0);
				}
				
			}		
			if(NRF24L01_RxPacket(tmp_buf)==0)//一旦接收到信息,则显示出来.
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
				//刷新界面
			//if((ba_dian_x==ba_dian_x_last||ba_dian_x==ba_dian_x_last+1||ba_dian_x==ba_dian_x_last-1)&&(ba_dian_y==ba_dian_y_last||ba_dian_y==ba_dian_y_last+1||ba_dian_y==ba_dian_y_last-1))//如果值没改变什么都不做
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
			 Gui_StrCenter(0,30,RED,BLUE,"error",16,1);//居中显示
		 }
		 if(ba_dian_y>240)
		 {
			 Gui_StrCenter(0,100,RED,BLUE,"error",16,1);//居中显示
		 }
		 if(ba_dian_flag==1)
		 {
			 gui_circle(ba_dian_x,ba_dian_y,RED,2,1);
		 }
//根据red_x1,red_y1的不同值，显示相应内容<--根据靶点到圆心的距离
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
     Show_Str(240,0,BLUE,PINK,"十环",32,0);//背景粉色
		 }
     else if(length<=34*34)
		 {
     Show_Str(240,0,BLUE,PINK,"九环",32,0);//背景粉色
		 }
     else if(length<=51*51)
		 {
     Show_Str(240,0,BLUE,PINK,"八环",32,0);//背景粉色
		 }
     else if(length<=68*68)
		 {
     Show_Str(240,0,BLUE,PINK,"七环",32,0);//背景粉色
		 }
     else if(length<=85*85)
		 {
     Show_Str(240,0,BLUE,PINK,"六环",32,0);//背景粉色
		 }
     else if(length<=102*102)
		 {
     Show_Str(240,0,BLUE,PINK,"五环",32,0);//背景粉色
		 }
     else
		 {
		 Show_Str(240,0,BLUE,PINK,"脱靶",32,0);//背景粉色
		 }
//接下来判断正上，中心等先试试直接浮点型比较
		 if(length>102*102)//脱靶不显示方位信息
		 {
		 }
		 else
		 {
		 if(ba_dian_x==119&&ba_dian_y==119)
		 {
			Show_Str(240,45,BLUE,PINK,"中心",32,0);//背景粉色 
		 }
		 if(ba_dian_x>119&&ba_dian_y<119)//右上区域
		 {
			 jiaozhi=(double)(119-ba_dian_y)/(ba_dian_x-119);
			 if(jiaozhi<0.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"正右",32,0);//背景粉色 
			 }
			 else if(jiaozhi<2.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"右上",32,0);//背景粉色  
			 }
			 else
			 {
				Show_Str(240,45,BLUE,PINK,"正上",32,0);//背景粉色   
			 }
		 }
		 if(ba_dian_x<119&&ba_dian_y<119)//左上区域
		 {
			 jiaozhi=(double)(119-ba_dian_y)/(119-ba_dian_x);
			 if(jiaozhi<0.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"正左",32,0);//背景粉色 
			 }
			 else if(jiaozhi<2.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"左上",32,0);//背景粉色  
			 }
			 else
			 {
				Show_Str(240,45,BLUE,PINK,"正上",32,0);//背景粉色   
			 }
		 }
		 if(ba_dian_x<119&&ba_dian_y>119)//左下区域
		 {
			 jiaozhi=(double)(ba_dian_y-119)/(119-ba_dian_x);
			 if(jiaozhi<0.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"正左",32,0);//背景粉色 
			 }
			 else if(jiaozhi<2.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"左下",32,0);//背景粉色  
			 }
			 else
			 {
				Show_Str(240,45,BLUE,PINK,"正下",32,0);//背景粉色   
			 }
		 }
		 if(ba_dian_x>119&&ba_dian_y>119)//右下区域
		 {
			 jiaozhi=(double)(ba_dian_y-119)/(ba_dian_x-119);
			 if(jiaozhi<0.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"正右",32,0);//背景粉色 
			 }
			 else if(jiaozhi<2.414)
			 {
				 Show_Str(240,45,BLUE,PINK,"右下",32,0);//背景粉色  
			 }
			 else
			 {
				Show_Str(240,45,BLUE,PINK,"正下",32,0);//背景粉色   
			 }
		 }
	 }
		   if(shoudong_flag==1)
				{
					Show_Str(240,180,BLUE,PINK,"手动模式",16,0);
				}
				else
				{
					Show_Str(240,180,BLUE,PINK,"自动模式",16,0);
				}
			
		 ba_dian_x_last=ba_dian_x;
		 ba_dian_y_last=ba_dian_y;
	  }
		 //刷新界面
			}//<---接收到数据
			else//没接收到数据则表明没有红点，靶子不显示
			{
				
		  }		
		}


 }

