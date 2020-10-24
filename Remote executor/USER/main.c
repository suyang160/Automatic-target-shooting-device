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
//±¾³ÌÐòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßÐí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//²âÊÔÓ²¼þ£ºµ¥Æ¬»úSTM32F103RBT6,ÕýµãÔ­×ÓMiniSTM32¿ª·¢°å,Ö÷Æµ72MHZ
//QDtech-TFTÒº¾§Çý¶¯ for STM32 IOÄ£Äâ
//xiao·ë@ShenZhen QDtech co.,LTD
//¹«Ë¾ÍøÕ¾:www.qdtech.net
//ÌÔ±¦ÍøÕ¾£ºhttp://qdtech.taobao.com
//ÎÒË¾Ìá¹©¼¼ÊõÖ§³Ö£¬ÈÎºÎ¼¼ÊõÎÊÌâ»¶Ó­ËæÊ±½»Á÷Ñ§Ï°
//¹Ì»°(´«Õæ) :+86 0755-23594567 
//ÊÖ»ú:15989313508£¨·ë¹¤£© 
//ÓÊÏä:QDtech2008@gmail.com 
//Skype:QDtech2008
//¼¼Êõ½»Á÷QQÈº:324828016
//´´½¨ÈÕÆÚ:2013/5/13
//°æ±¾£ºV1.1
//°æÈ¨ËùÓÐ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ÉîÛÚÊÐÈ«¶¯µç×Ó¼¼ÊõÓÐÏÞ¹«Ë¾ 2009-2019
//All rights reserved
/****************************************************************************************************
//=======================================Òº¾§ÆÁÊý¾ÝÏß½ÓÏß==========================================//
//DB0       ½ÓPD14 
//DB1       ½ÓPD15 
//DB2       ½ÓPD0 
//DB3       ½ÓPD1 
//DB4~DB12  ÒÀ´Î½ÓPE7~PE15
//DB13      ½ÓPD8 
//DB14      ½ÓPD9
//DB15      ½ÓPD10  
//=======================================Òº¾§ÆÁ¿ØÖÆÏß½ÓÏß==========================================//
//LCD_CS	½ÓPG12	//Æ¬Ñ¡ÐÅºÅ
//LCD_RS	½ÓPG0	//¼Ä´æÆ÷/Êý¾ÝÑ¡ÔñÐÅºÅ
//LCD_WR	½ÓPD5	//Ð´ÐÅºÅ
//LCD_RD	½ÓPD4	//¶ÁÐÅºÅ
//LCD_RST	½ÓPC5	//¸´Î»ÐÅºÅ
//LCD_LED	½ÓPB0	//±³¹â¿ØÖÆÐÅºÅ(¸ßµçÆ½µãÁÁ)
//=========================================´¥ÃþÆÁ´¥½ÓÏß=========================================//
//²»Ê¹ÓÃ´¥Ãþ»òÕßÄ£¿é±¾Éí²»´ø´¥Ãþ£¬Ôò¿É²»Á¬½Ó
//MO(MISO)	½ÓPF8	//SPI×ÜÏßÊä³ö
//MI(MOSI)	½ÓPF9	//SPI×ÜÏßÊäÈë
//PEN		½ÓPF10	//´¥ÃþÆÁÖÐ¶ÏÐÅºÅ
//TCS		½ÓPB2	//´¥ÃþICÆ¬Ñ¡
//CLK		½ÓPB1	//SPI×ÜÏßÊ±ÖÓ
**************************************************************************************************/	
#define jiguang  PAout(12)
extern u16 key_val;
extern u8 ov_sta=0;	//ÔÚexit.cÀïÃæ¶¨Òå
u8 jiao_zhen=0;  //<----Èç¹û»¹Ã»Ð£ÕýºÃ£¬ÔòÎª0;Ð£ÕýºÃÔòÎª1(°ÚÕý)<---Ëü²»Îª1£¬³ÌÐòÒ»Ö±Ð£Õý
u16 x1,y1;
u16 x2,y2;
u16 x3,y3;
u16 x4,y4;           //<---ËÄ¸ö¶¨Î»µãµÄ×îÖÕ×ø±ê
u16 x1_temp,y1_temp;
u16 x2_temp,y2_temp;
u16 x3_temp,y3_temp;
u16 x4_temp,y4_temp;           //<---ËÄ¸ö¶¨Î»µãµÄÁÙÊ±¼ÆËã×ø±ê<--Ã²ËÆÓÐºÜ¶àÔÝÊ±ÓÃ²»µ½
u16 count1;
u16 count2;
u16 count3;
u16 count4;                   //·Ö±ðÎªËÄ¸ö½ÇÏà¼ÓµÄµãµÄ¸öÊý<---³ýÒÔËüÇóÆ½¾ù
u16 red_x1,red_y1;
u16 red_x1_temp,red_y1_temp;
u16 red_count;
u16 ba_dian_x,ba_dian_y;
u32 i_temp;                 //<---ÓÃÓÚ²âÊÔ
u32 j_temp;
u32 color_temp;
u32 i_red;                 //<---ÓÃÓÚ²âÊÔ
u32 j_red;
u32 color_red;
u8  a;
u16 red_max;
u8 kongping_flag;//<---´ËflagÓÃÓÚÔÚÎÞ¼¤¹âµãÊ±²»Ë¢ÆÁ
u32 length;  //<----°Ðµãµ½Ô²ÐÄµÄ¾àÀë
u16 PWM_temp1=1440;
u16 PWM_temp2=1220;
double jiaozhi;
u16 shoudong_flag=1;//<----³õÊ¼×´Ì¬ÏÂ£¬¸ÃÖµÎª1£¬´ú±í´¦ÓÚÊÖ¶¯×´Ì¬
u8 tmp_buf[7]={'0','0','0','0','0','0',0};//<---ÕâÊÇ³õÊ¼»¯×´Ì¬£¬ÊÖ³Ö¶Ëµ¥Æ¬»ú½ÓÊÕµ½¸ÃÊý¾Ý±ãÖªµÀÃ»ÓÐÉ¨Ãèµ½ºìµã
u8 tmp_buf_2[7];//<---ÉãÏñÍ·½ÓÊÕÊý×é£¬Ö»ÒªÓÃÁ½Î»
u16 t;                            //Ê¹ÄÜ½øÈë´ÎÊýt
u8 jiguang_flag;//µ±ÎªÒ»Ê±½øÈë¼ÆÊý
u8 xiangxian_flag;//<---1´ú±íµÚÒ»ÏóÏÞÇøÓò2´ú±íµÚ¶þÏóÏÞÇøÓò

//µ±°´ÏÂÁË11Éä»÷¼üºó£¬Ê¹ÄÜTIM3Ê±ÖÓ£¬ÇåÁãtÔÚ³ÌÐòÖÐ×Ô¶¯Íê³É;
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//Òç³öÖÐ¶Ï
	{
		if(jiguang_flag==1)
		{
			 t++;//Ã»½øÈëÒ»´Î500ms
			 if(t<=8)
			 {
			 jiguang=0;//<---¼¤·¢ËÄÃë
			 }
			 else
			 {
			 jiguang=1;//<---Ï¨Ãð
			 jiguang_flag=0;
			 t=0;     
			 } 
		 }			 
	}				   
	TIM3->SR&=~(1<<0);//Çå³ýÖÐ¶Ï±êÖ¾Î» 	    
}
//Í¨ÓÃ¶¨Ê±Æ÷3ÖÐ¶Ï³õÊ¼»¯
//ÕâÀïÊ±ÖÓÑ¡ÔñÎªAPB1µÄ2±¶£¬¶øAPB1Îª36M
//arr£º×Ô¶¯ÖØ×°Öµ¡£
//psc£ºÊ±ÖÓÔ¤·ÖÆµÊý
//ÕâÀïÊ¹ÓÃµÄÊÇ¶¨Ê±Æ÷3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3Ê±ÖÓÊ¹ÄÜ    
 	TIM3->ARR=arr;  	//Éè¶¨¼ÆÊýÆ÷×Ô¶¯ÖØ×°Öµ//¸ÕºÃ1ms    
	TIM3->PSC=psc;  	//Ô¤·ÖÆµÆ÷7200,µÃµ½10KhzµÄ¼ÆÊýÊ±ÖÓ		  
	TIM3->DIER|=1<<0;   //ÔÊÐí¸üÐÂÖÐ¶Ï	  
	TIM3->CR1|=0x01;    //Ê¹ÄÜ¶¨Ê±Æ÷3
  MY_NVIC_Init(1,3,0x1D,2);//ÇÀÕ¼1£¬×ÓÓÅÏÈ¼¶3£¬×é2									 
}

void chuansong();
void  length_jisuan();
void EXTI8_Init(void)
{												  
	Ex_NVIC_Config(GPIOA,8,2);		 		//ÉÏÉýÑØ´¥·¢			  
	MY_NVIC_Init(0,0,0x17,2);		//ÇÀÕ¼0,×ÓÓÅÏÈ¼¶0£¬×é2	   
}
void camera_refresh(void);
void EXTI9_5_IRQHandler(void)
{		 		
	if(EXTI->PR&(1<<8))//ÊÇ8ÏßµÄÖÐ¶Ï
	{     
		if(ov_sta<2)
		{
			if(ov_sta==0)
			{
				OV7670_WRST=0;	 	//¸´Î»Ð´Ö¸Õë		  		 
				OV7670_WRST=1;	
				OV7670_WREN=1;		//ÔÊÐíÐ´ÈëFIFO
			}else 
			{OV7670_WREN=0;
			 OV7670_WRST=0;	 	//¸´Î»Ð´Ö¸Õë		  		 
			 OV7670_WRST=1;		//½ûÖ¹Ð´ÈëFIFO 
     //  camera_refresh();			
			} 
			ov_sta++;
		}
	}
	EXTI->PR=1<<8;     //Çå³ýLINE8ÉÏµÄÖÐ¶Ï±êÖ¾Î»						  
}

	


 void camera1_refresh(void)
{
	u32 i;//<---ÏÔÊ¾ÐÐÊý
	u32 j;//<---ÏÔÊ¾ÁÐÊý
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
  if(jiao_zhen==0)//»¹´¦ÓÚÐ£Õý×´Ì¬
  {
		if(ov_sta==2)
		{
		LCD_SetCursor(0x00,0x0000);	 
		LCD_WriteRAM_Prepare(); 
		OV7670_RRST=0;				//¿ªÊ¼¸´Î»¶ÁÖ¸Õë 
		OV7670_RCK=0;
		OV7670_RCK=1;
		OV7670_RCK=0;
		OV7670_RRST=1;				//¸´Î»¶ÁÖ¸Õë½áÊø 
		OV7670_RCK=1;
		for(i=0;i<240;i++)
		{
			for(j=0;j<320;j++)
			{
				OV7670_RCK=0;
				__nop();__nop();
				color=GPIOC->IDR&0XFF;	//¶ÁÊý¾Ý
				OV7670_RCK=1;
				__nop();__nop(); 			
				color<<=8;  
				OV7670_RCK=0;
				__nop();__nop();
				color|=GPIOC->IDR&0XFF;	//¶ÁÊý¾Ý
				OV7670_RCK=1;
				LCD->LCD_RAM=color;
				if(i<235&&i>5&&j>10&&j<310)//<---×óÉÏ½Ç
				{					
					if(color>=60000&&color<=65000)//<---Óöµ½ÁËºìµã
					{
						i_temp=i;
						j_temp=j;
						color_temp=color;
					}
			  }
        if(i<120&&i>5&&j>5&&j<160)//<---×óÉÏ½Ç<---Í¼ÏñÅÄÉãÖÜÎ§Ò»È¦´íÎó
				{					
					if(color<30000)//<---Óöµ½ÁËºÚµã
					{
						x1_temp+=j;
						y1_temp+=i;
						count1++;
					}
			  }
			  if(i<120&&i>5&&j>160&&j<315)
				{
					if(color<30000)//<---Óöµ½ÁËºÚµã
					{
						x2_temp+=j;
						y2_temp+=i;
						count2++;
					}
				}
				if(i>120&&i<235&&j<160&&j>5)
				{
					if(color<30000)//<---Óöµ½ÁËºÚµã
					{
						x3_temp+=j;
						y3_temp+=i;
						count3++;
					}
				}
				if(i>120&&i<235&&j>160&&j<315)
				{
					if(color<30000)//<---Óöµ½ÁËºÚµã
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
          if(((x1_temp<x3_temp&&x3_temp-x1_temp<2)||(x3_temp<x1_temp&&x1_temp-x3_temp<2))&&((y1_temp<y2_temp&&y2_temp-y1_temp<2)||(y2_temp<y1_temp&&y1_temp-y2_temp<2)))//<---Ð£ÕýµÄÅÐ¶ÏÌõ¼þ	
					{
					jiao_zhen=1;
          x1=x1_temp;y1=y1_temp;
          x2=x2_temp;y2=y2_temp;
          x3=x3_temp;y3=y3_temp;            //×ø±êµÄÈ·ÈÏ
          x4=x4_temp;y4=y4_temp;
          a=1;						
					}
           else	//Èç¹ûµ±Ç°ÖµÊÇ´íÎóµÄ
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
						 x3_temp=0;y3_temp=0;          //½«ÁÙÊ±ÖµÇåÁã
						 x4_temp=0;y4_temp=0;
             count1=0;count2=0;
             count3=0;count4=0;						
					}
//          LCD_ShowNum(20,20,i_temp,3,12);
//				  LCD_ShowNum(20,40,j_temp,3,12);
//					LCD_ShowNum(20,60,color_temp,5,12);
					color_temp=0;
          i_temp=0;
          j_temp=0;//<---½«±êÖ¾Î»ÇåÁã  					
				}					
			}
		}//<---forÑ­»·ÄÚ²¿			
		EXTI->PR=1<<8;     			//Çå³ýLINE8ÉÏµÄÖÐ¶Ï±êÖ¾Î»
		ov_sta=0;					//¿ªÊ¼ÏÂÒ»´Î²É¼
	}
}//<---jiaozhen_flag
 else
 { 
		if(ov_sta==2)
		{
			OV7670_RRST=0;				//¿ªÊ¼¸´Î»¶ÁÖ¸Õë 
			OV7670_RCK=0;
			OV7670_RCK=1;
			OV7670_RCK=0;
			OV7670_RRST=1;				//¸´Î»¶ÁÖ¸Õë½áÊø 
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
				color=GPIOC->IDR&0XFF;	//¶ÁÊý¾Ý
				OV7670_RCK=1;
				__nop();__nop(); 			
				color<<=8;  
				OV7670_RCK=0;
				__nop();__nop();
				color|=GPIOC->IDR&0XFF;	//¶ÁÊý¾Ý
				OV7670_RCK=1;
        if(j>x1&&j<x2&&i>y1&&i<y3)//±ØÐëÔÚ·¶Î§ÄÚ
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
//			 		//<--×ªÒÆ£¬Èôx1,y1=0,ÔòÃ»É¨µ½ºìµã£¬²»ÏÔÊ¾
//          red_x1_temp=0;
//          red_x1_temp=0;					
//          red_count=0;
//          LCD_ShowNum(20,20,red_x1,3,12);
//				  LCD_ShowNum(20,40,red_y1,3,12);         					
				}					
			}   							 
		 }//<---forÑ­»·×îÍâ²ã
		 EXTI->PR=1<<8;     			//Çå³ýLINE8ÉÏµÄÖÐ¶Ï±êÖ¾Î»
		 ov_sta=0;					//¿ªÊ¼ÏÂÒ»´Î²É¼¯
	  }
 }//<--else
 if(jiao_zhen==1)//´¦ÓÚÒÑÐ£Õý×´Ì¬
{

	if(red_x1==0&&red_y1==0)//Ã»É¨µ½£¬ÏÔÊ¾¸Ãµã<--»»Ëã±ÈÀýºó£¬ÏÔÊ¾Ç°ÏÈÇåÆÁ
	{
   ba_dian_x=0;
	 ba_dian_y=0;//<---Èç¹ûÃ»ÓÐÉ¨Ãèµ½ºìµã£¬Ôò·¢ËÍ00
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
//			 Gui_StrCenter(0,30,RED,BLUE,"error",16,1);//¾ÓÖÐÏÔÊ¾
//		 }
//		 if(ba_dian_y>240)
//		 {
//			 Gui_StrCenter(0,100,RED,BLUE,"error",16,1);//¾ÓÖÐÏÔÊ¾
//		 }
//		 gui_circle(ba_dian_x,ba_dian_y,RED,2,1);
		
		 LCD_ShowNum(20,20,ba_dian_x,3,12);
		 LCD_ShowNum(20,40,ba_dian_y,3,12);
		 LCD_ShowNum(20,120,key_val,3,12);
		 LCD_ShowNum(20,160,length,5,12);
//¸ù¾Ýred_x1,red_y1µÄ²»Í¬Öµ£¬ÏÔÊ¾ÏàÓ¦ÄÚÈÝ<--¸ù¾Ý°Ðµãµ½Ô²ÐÄµÄ¾àÀë
//			LCD_ShowNum(20,20,i_red,3,12);
//			LCD_ShowNum(20,40,j_red,3,12);
//			LCD_ShowNum(20,60,color_red,5,12);
//			color_red=0;
//			i_red=0;
//			j_red=0;//<---½«±êÖ¾Î»ÇåÁã 	
	}		
}

 
}
void camera2_refresh(void)//<---ÊÔÑéº¯ÊýÓÃÀ´²â¼¤¹â±ÊÖÐµÄÑÕÉ«Öµ£¬µ÷ÊÔº¯Êý
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
	NRF24L01_Init();          //³õÊ¼»¯·¢ËÍµã×ø±êÎÞÏß
  NRF24L01_Init_2();    	//³õÊ¼»¯NRF24L01<---½ÓÊÕ°´¼üÖµ
  TIM3_Int_Init(4999,7199);//10KhzµÄ¼ÆÊýÆµÂÊ£¬¼ÆÊý5K´ÎÎª500ms  
	TIM1_PWM_Init(20000,71);	//1MHZµÄ·ÖÆµ£¬Ã¿¸öÊý¾ÍÊÇ1us,200000¼´ÖÜÆÚ20ms
	TIM1->CCR3=PWM_temp1;       //PA10       
	TIM1->CCR4=PWM_temp2;       //PA11
 	Init_Keypad();
	while(OV7670_Init())//³õÊ¼»¯OV7670
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
	
		while(NRF24L01_Check())	//¼ì²é·¢ËÍµÄNRF24L01ÊÇ·ñÔÚÎ».	
	{
		Gui_StrCenter(0,80,RED,BLUE,"NRF24L01 Init error",16,1);
		delay_ms(200);
	}
	 Gui_StrCenter(0,80,RED,BLUE,"NRF24L01 Init OK",16,1);
	 	NRF24L01_TX_Mode();
	
  	while(NRF24L01_Check_2())	//¼ì²é½ÓÊÕµÄNRF24L01ÊÇ·ñÔÚÎ».	
	{
		Gui_StrCenter(0,80,RED,BLUE,"NRF24L01*2 Init error",16,1);
		delay_ms(200);
	}
	 Gui_StrCenter(0,80,RED,BLUE,"NRF24L01*2 Init OK",16,1);
	 	NRF24L01_RX_Mode_2();


    RCC->APB2ENR|=1<<2;     //Ê¹ÄÜAÊ±ÖÓ
    GPIOA->CRH&=0XFFF0FFFF;
    GPIOA->CRH|=0X00030000;
    GPIOA->ODR&=0X11101111;  //½«PA12ÖÃµÍ£¬PA12¸ßµçÆ½Ê±µãÁÁ	
    Show_Str(240,120,BLUE,PINK,"shoudong",32,0);//<---³õÊ¼»¯ÊÖ¶¯Ä£Ê
	
	 while(1)
	 {
		camera1_refresh();
    chuansong();
		length_jisuan();
//		if(shoudong_flag==0)//<---¸Ã´úÂëÊÇ±£Ö¤ÔÚ×Ô¶¯Ä£Ê½ÏÂ´ò°Ð×¼È·
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
//			if(key_val==3)//<--°Ë»·
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
		  if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//Ò»µ©½ÓÊÕµ½ÐÅÏ¢,ÔòÏÔÊ¾³öÀ´.
		 {
			 key_val=(tmp_buf_2[0]-'0')*10+(tmp_buf_2[1]-'0');
       if(shoudong_flag==1)
			{//<---ÊÖ¶¯Ä£Ê½ÏÂ				
				if(key_val==9)//PWM_temp1³õÖµ1500
				{
					PWM_temp1+=5;
					TIM1->CCR3=PWM_temp1;       //PB6 <--ÉÏÃæ×ó×ª
					LCD_ShowNum(20,20,PWM_temp1,4,12);  
				}
				if(key_val==10)
				{
					PWM_temp1-=5;
					TIM1->CCR3=PWM_temp1;       //PB6  <--ÉÏÃæÓÒ×ª
					LCD_ShowNum(20,20,PWM_temp1,4,12); 
				}
				if(key_val==7)
				{
					PWM_temp2-=5;
					TIM1->CCR4=PWM_temp2;       //PB7  <---ÏÂÃæÉÏ×ª 
					LCD_ShowNum(20,40,PWM_temp2,4,12); 					
				}
				if(key_val==8)
				{
					PWM_temp2+=5;
					TIM1->CCR4=PWM_temp2;       //PB7  <---ÏÂÃæÏÂ×ª
					LCD_ShowNum(20,40,PWM_temp2,4,12);
				} 
        if(key_val==11)
				{
         jiguang_flag=1;
				}
				if(key_val==12)//<---°´ÏÂÁË³£ÁÁ°´Å¥£¬¼¤¹â±ÊÒ»Ö±ÁÁ£¬µ±È»²»ÔÚÌâÄ¿ÒªÇó·¶Î§
				{
				jiguang=0; 
				}
				if(key_val==15)
				{
				 shoudong_flag=0;
         Show_Str(240,120,BLUE,PINK,"zidong",32,0);					
				}
			}//<---ÊÖ¶¯Ä£Ê½ÏÂ		
      else
			{
					if(key_val==1)//°´ÏÂÁËÊ®»·°´Å¥
				{
					 PWM_temp1=1440;
					 TIM1->CCR3=PWM_temp1;
           PWM_temp2=1220;       //ÏÈ´ÖÂÔµÄµ÷µ½Õâ¸öÎ»ÖÃ
					 TIM1->CCR4=PWM_temp2;
           jiguang=0;     //µãÁÁ¼¤¹â±Ê
           delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
					 delay_ms(1000);
					 delay_ms(1000);
					 camera1_refresh();
           jiguang=1;				
					
					loop1:		if(ba_dian_x>119+12)//<---µãÔÚÓÒ±ß£¬Ïò×óµ÷Õû
					{
					PWM_temp1+=4;
					TIM1->CCR3=PWM_temp1;       	
					}
					if(ba_dian_x<119-12)//<---µãÔÚ×ó±ß£¬ÏòÓÒµ÷Õû
					{
					PWM_temp1-=4;
					TIM1->CCR3=PWM_temp1;       	
					}
					if(ba_dian_y<119-12)//<---µãÔÚÉÏ±ß£¬ÏòÏÂµ÷Õû
					{
					PWM_temp2+=4;
					TIM1->CCR4=PWM_temp2;       //PB7  <---ÏÂÃæÏÂ×ª
					}
					if(ba_dian_y>119+12)//<---µãÔÚÏÂ±ß£¬ÏòÉÏµ÷Õû
					{
					PWM_temp2-=4;
					TIM1->CCR4=PWM_temp2;       //PB7  <---ÏÂÃæÉÏ×ª
					}
					
					delay_ms(500);						
					jiguang=0;     //µãÁÁ¼¤¹â±Ê
          delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
					 delay_ms(1000);
					 delay_ms(1000);					
					camera1_refresh();
          jiguang=1;	
					chuansong();
				 if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//Ìø³ö×Ô¶¯Ä£Ê½´úÂë
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
          					
				}//<---Ê®»·°´Å¥	
				if(key_val==2)//°´ÏÂ¾Å»·
				{			
					jiguang=0;     //µãÁÁ¼¤¹â±Ê
//          delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop2: if(xiangxian_flag==1)
					{
						if(length>34*34)//<---µãÔÚÓÒ±ß£¬Ïò×óµ÷Õû
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=17*17)//<---µãÔÚ×ó±ß£¬ÏòÓÒµ÷Õû
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					else
					{
						if(length>34*34)//<---µãÔÚ×ó±ß£¬ÏòÓÒµ÷Õû
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=17*17)//<---µãÔÚÓÒ±ß£¬Ïò×óµ÷Õû
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					delay_ms(500);
					jiguang=0;     //µãÁÁ¼¤¹â±Ê
//          delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//Ìø³ö×Ô¶¯Ä£Ê½´úÂë
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
						jiguang=1;     //Ï¨Ãð¼¤¹â±Ê
					}
          else
					{						
						goto loop2;
					}
          					
				}//<---¾Å»·°´Å¥	
					if(key_val==3)//°´ÏÂ°Ë»·
				{
          jiguang=0;     //µãÁÁ¼¤¹â±Ê
//          delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop3: if(xiangxian_flag==1)
					{						
						if(length>51*51)//<---µãÔÚÓÒ±ß£¬Ïò×óµ÷Õû
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=34*34)//<---µãÔÚ×ó±ß£¬ÏòÓÒµ÷Õû
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
					jiguang=0;     //µãÁÁ¼¤¹â±Ê
//          delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//Ìø³ö×Ô¶¯Ä£Ê½´úÂë
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
							jiguang=1;     //Ï¨Ãð¼¤¹â±Ê
					}
          else
					{						
						goto loop3;
					}
          					
				}//<---°Ë»·°´Å¥	
					if(key_val==4)//°´ÏÂÆß»·
				{
          jiguang=0;     //µãÁÁ¼¤¹â±Ê
//          delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop4:	if(xiangxian_flag==1)
					{						
						if(length>68*68)//<---µãÔÚÓÒ±ß£¬Ïò×óµ÷Õû
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=51*51)//<---µãÔÚ×ó±ß£¬ÏòÓÒµ÷Õû
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
				  }
					else
					{
						 if(length>68*68)//<---µãÔÚÓÒ±ß£¬Ïò×óµ÷Õû
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=51*51)//<---µãÔÚ×ó±ß£¬ÏòÓÒµ÷Õû
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					delay_ms(500);	
					jiguang=0;     //µãÁÁ¼¤¹â±Ê
//          delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//Ìø³ö×Ô¶¯Ä£Ê½´úÂë
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
							jiguang=1;     //Ï¨Ãð¼¤¹â±Ê
					}
          else
					{						
						goto loop4;
					}
          					
				}//<---Æß»·°´Å¥	
					if(key_val==5)//°´ÏÂÁù»·
				{
          jiguang=0;     //µãÁÁ¼¤¹â±Ê
//          delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop5:	if(xiangxian_flag==1)
					{						
						if(length>85*85)//<---µãÔÚÓÒ±ß£¬Ïò×óµ÷Õû
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=68*68)//<---µãÔÚ×ó±ß£¬ÏòÓÒµ÷Õû
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
					jiguang=0;     //µãÁÁ¼¤¹â±Ê
//          delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//Ìø³ö×Ô¶¯Ä£Ê½´úÂë
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
							jiguang=1;     //Ï¨Ãð¼¤¹â±Ê
					}
          else
					{						
						goto loop5;
					}
          					
				}//<---Áù»·°´Å¥	
	     if(key_val==6)//°´ÏÂÎå»·
				{
          jiguang=0;     //µãÁÁ¼¤¹â±Ê
//          delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop6:	if(xiangxian_flag==1)
					{						
						if(length>102*102)//<---µãÔÚÓÒ±ß£¬Ïò×óµ÷Õû
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=85*85)//<---µãÔÚ×ó±ß£¬ÏòÓÒµ÷Õû
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
				  }
					else
					{
						if(length>102*102)//<---µãÔÚÓÒ±ß£¬Ïò×óµ÷Õû
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=85*85)//<---µãÔÚ×ó±ß£¬ÏòÓÒµ÷Õû
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					delay_ms(500);
					jiguang=0;     //µãÁÁ¼¤¹â±Ê
//          delay_ms(1000);//<---ÑÓÊ±ÈýºÁÃë
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;	
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//Ìø³ö×Ô¶¯Ä£Ê½´úÂë
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
							jiguang=1;     //Ï¨Ãð¼¤¹â±Ê
					}
          else
					{						
						goto loop6;
					}         					
				}//<---Îå»·°´Å¥	
				loop7:  if(key_val==11)//<---°´ÏÂÁËÉä»÷¼ü£¬Ôò¿ªÆô¶¨Ê±Æ÷
				{
					jiguang_flag=1;
				}
				if(key_val==12)//<---°´ÏÂÁË³£ÁÁ°´Å¥£¬¼¤¹â±ÊÒ»Ö±ÁÁ£¬µ±È»²»ÔÚÌâÄ¿ÒªÇó·¶Î§
				{
				jiguang=0; 
				}
				if(key_val==16)
				{
				 shoudong_flag=1;//¸ÄÎªÊÖ¶¯Ä£Ê½	
				 Show_Str(240,120,BLUE,PINK,"shoudong",32,0);	
				}
			}				
    }//½ÓÊÕµ½¼üÖµ
  }//<---while´óÑ­»·		
   
    			 
	
	
 }
void chuansong()
{
		if(jiao_zhen==1)//<---ÒÑ´¦ÓÚÐ£Õý×´Ì¬ÏÂ//Èç¹û·¢ËÍ00Ôò±íÊ¾Ã»½ÓÊÕµ½ºìµã
		{
	     	tmp_buf[0]= ba_dian_x/100+'0';
				tmp_buf[1]= ba_dian_x%100/10+'0';
				tmp_buf[2]= ba_dian_x%10+'0';     //<---ÒÀ´ÎÎª°Ðµã×ø±êµÄÈýÎ»£¬ÓÉ¸ßµ½µÍ
				tmp_buf[3]= ba_dian_y/100+'0';
				tmp_buf[4]= ba_dian_y%100/10+'0';
				tmp_buf[5]= ba_dian_y%10+'0';     //<---ÒÀ´ÎÎª°Ðµã×ø±êµÄÈýÎ»£¬ÓÉ¸ßµ½µÍ			
				tmp_buf[6]=0;
				if(NRF24L01_TxPacket(tmp_buf)==TX_OK)//<---Ö±µ½·¢ËÍ³É¹¦ÍË³ö
				{
					Gui_StrCenter(0,30,RED,BLUE,"SUCC",16,0);//¾ÓÖÐÏÔÊ¾	
				}
				else
				{
					Gui_StrCenter(0,30,RED,BLUE,"LOSE",16,0);//¾ÓÖÐÏÔÊ¾	
				}			
	  }	
}
void  length_jisuan()//Ð±±ßµÄÆ½·½
{
		 if(ba_dian_x>=119&&ba_dian_y>=119)
		 {
		 length=(ba_dian_x-119)*(ba_dian_x-119)+(ba_dian_y-119)*(ba_dian_y-119);
		 xiangxian_flag=1;//µÚÒ»ÏóÏÞ
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

