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
#define jiguang  PAout(12)
extern u16 key_val;
extern u8 ov_sta=0;	//在exit.c里面定义
u8 jiao_zhen=0;  //<----如果还没校正好，则为0;校正好则为1(摆正)<---它不为1，程序一直校正
u16 x1,y1;
u16 x2,y2;
u16 x3,y3;
u16 x4,y4;           //<---四个定位点的最终坐标
u16 x1_temp,y1_temp;
u16 x2_temp,y2_temp;
u16 x3_temp,y3_temp;
u16 x4_temp,y4_temp;           //<---四个定位点的临时计算坐标<--貌似有很多暂时用不到
u16 count1;
u16 count2;
u16 count3;
u16 count4;                   //分别为四个角相加的点的个数<---除以它求平均
u16 red_x1,red_y1;
u16 red_x1_temp,red_y1_temp;
u16 red_count;
u16 ba_dian_x,ba_dian_y;
u32 i_temp;                 //<---用于测试
u32 j_temp;
u32 color_temp;
u32 i_red;                 //<---用于测试
u32 j_red;
u32 color_red;
u8  a;
u16 red_max;
u8 kongping_flag;//<---此flag用于在无激光点时不刷屏
u32 length;  //<----靶点到圆心的距离
u16 PWM_temp1=1440;
u16 PWM_temp2=1220;
double jiaozhi;
u16 shoudong_flag=1;//<----初始状态下，该值为1，代表处于手动状态
u8 tmp_buf[7]={'0','0','0','0','0','0',0};//<---这是初始化状态，手持端单片机接收到该数据便知道没有扫描到红点
u8 tmp_buf_2[7];//<---摄像头接收数组，只要用两位
u16 t;                            //使能进入次数t
u8 jiguang_flag;//当为一时进入计数
u8 xiangxian_flag;//<---1代表第一象限区域2代表第二象限区域

//当按下了11射击键后，使能TIM3时钟，清零t在程序中自动完成;
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//溢出中断
	{
		if(jiguang_flag==1)
		{
			 t++;//没进入一次500ms
			 if(t<=8)
			 {
			 jiguang=0;//<---激发四秒
			 }
			 else
			 {
			 jiguang=1;//<---熄灭
			 jiguang_flag=0;
			 t=0;     
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

void chuansong();
void  length_jisuan();
void EXTI8_Init(void)
{												  
	Ex_NVIC_Config(GPIOA,8,2);		 		//上升沿触发			  
	MY_NVIC_Init(0,0,0x17,2);		//抢占0,子优先级0，组2	   
}
void camera_refresh(void);
void EXTI9_5_IRQHandler(void)
{		 		
	if(EXTI->PR&(1<<8))//是8线的中断
	{     
		if(ov_sta<2)
		{
			if(ov_sta==0)
			{
				OV7670_WRST=0;	 	//复位写指针		  		 
				OV7670_WRST=1;	
				OV7670_WREN=1;		//允许写入FIFO
			}else 
			{OV7670_WREN=0;
			 OV7670_WRST=0;	 	//复位写指针		  		 
			 OV7670_WRST=1;		//禁止写入FIFO 
     //  camera_refresh();			
			} 
			ov_sta++;
		}
	}
	EXTI->PR=1<<8;     //清除LINE8上的中断标志位						  
}

	


 void camera1_refresh(void)
{
	u32 i;//<---显示行数
	u32 j;//<---显示列数
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
  if(jiao_zhen==0)//还处于校正状态
  {
		if(ov_sta==2)
		{
		LCD_SetCursor(0x00,0x0000);	 
		LCD_WriteRAM_Prepare(); 
		OV7670_RRST=0;				//开始复位读指针 
		OV7670_RCK=0;
		OV7670_RCK=1;
		OV7670_RCK=0;
		OV7670_RRST=1;				//复位读指针结束 
		OV7670_RCK=1;
		for(i=0;i<240;i++)
		{
			for(j=0;j<320;j++)
			{
				OV7670_RCK=0;
				__nop();__nop();
				color=GPIOC->IDR&0XFF;	//读数据
				OV7670_RCK=1;
				__nop();__nop(); 			
				color<<=8;  
				OV7670_RCK=0;
				__nop();__nop();
				color|=GPIOC->IDR&0XFF;	//读数据
				OV7670_RCK=1;
				LCD->LCD_RAM=color;
				if(i<235&&i>5&&j>10&&j<310)//<---左上角
				{					
					if(color>=60000&&color<=65000)//<---遇到了红点
					{
						i_temp=i;
						j_temp=j;
						color_temp=color;
					}
			  }
        if(i<120&&i>5&&j>5&&j<160)//<---左上角<---图像拍摄周围一圈错误
				{					
					if(color<30000)//<---遇到了黑点
					{
						x1_temp+=j;
						y1_temp+=i;
						count1++;
					}
			  }
			  if(i<120&&i>5&&j>160&&j<315)
				{
					if(color<30000)//<---遇到了黑点
					{
						x2_temp+=j;
						y2_temp+=i;
						count2++;
					}
				}
				if(i>120&&i<235&&j<160&&j>5)
				{
					if(color<30000)//<---遇到了黑点
					{
						x3_temp+=j;
						y3_temp+=i;
						count3++;
					}
				}
				if(i>120&&i<235&&j>160&&j<315)
				{
					if(color<30000)//<---遇到了黑点
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
          if(((x1_temp<x3_temp&&x3_temp-x1_temp<2)||(x3_temp<x1_temp&&x1_temp-x3_temp<2))&&((y1_temp<y2_temp&&y2_temp-y1_temp<2)||(y2_temp<y1_temp&&y1_temp-y2_temp<2)))//<---校正的判断条件	
					{
					jiao_zhen=1;
          x1=x1_temp;y1=y1_temp;
          x2=x2_temp;y2=y2_temp;
          x3=x3_temp;y3=y3_temp;            //坐标的确认
          x4=x4_temp;y4=y4_temp;
          a=1;						
					}
           else	//如果当前值是错误的
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
						 x3_temp=0;y3_temp=0;          //将临时值清零
						 x4_temp=0;y4_temp=0;
             count1=0;count2=0;
             count3=0;count4=0;						
					}
//          LCD_ShowNum(20,20,i_temp,3,12);
//				  LCD_ShowNum(20,40,j_temp,3,12);
//					LCD_ShowNum(20,60,color_temp,5,12);
					color_temp=0;
          i_temp=0;
          j_temp=0;//<---将标志位清零  					
				}					
			}
		}//<---for循环内部			
		EXTI->PR=1<<8;     			//清除LINE8上的中断标志位
		ov_sta=0;					//开始下一次采�
	}
}//<---jiaozhen_flag
 else
 { 
		if(ov_sta==2)
		{
			OV7670_RRST=0;				//开始复位读指针 
			OV7670_RCK=0;
			OV7670_RCK=1;
			OV7670_RCK=0;
			OV7670_RRST=1;				//复位读指针结束 
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
				color=GPIOC->IDR&0XFF;	//读数据
				OV7670_RCK=1;
				__nop();__nop(); 			
				color<<=8;  
				OV7670_RCK=0;
				__nop();__nop();
				color|=GPIOC->IDR&0XFF;	//读数据
				OV7670_RCK=1;
        if(j>x1&&j<x2&&i>y1&&i<y3)//必须在范围内
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
//			 		//<--转移，若x1,y1=0,则没扫到红点，不显示
//          red_x1_temp=0;
//          red_x1_temp=0;					
//          red_count=0;
//          LCD_ShowNum(20,20,red_x1,3,12);
//				  LCD_ShowNum(20,40,red_y1,3,12);         					
				}					
			}   							 
		 }//<---for循环最外层
		 EXTI->PR=1<<8;     			//清除LINE8上的中断标志位
		 ov_sta=0;					//开始下一次采集
	  }
 }//<--else
 if(jiao_zhen==1)//处于已校正状态
{

	if(red_x1==0&&red_y1==0)//没扫到，显示该点<--换算比例后，显示前先清屏
	{
   ba_dian_x=0;
	 ba_dian_y=0;//<---如果没有扫描到红点，则发送00
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
//			 Gui_StrCenter(0,30,RED,BLUE,"error",16,1);//居中显示
//		 }
//		 if(ba_dian_y>240)
//		 {
//			 Gui_StrCenter(0,100,RED,BLUE,"error",16,1);//居中显示
//		 }
//		 gui_circle(ba_dian_x,ba_dian_y,RED,2,1);
		
		 LCD_ShowNum(20,20,ba_dian_x,3,12);
		 LCD_ShowNum(20,40,ba_dian_y,3,12);
		 LCD_ShowNum(20,120,key_val,3,12);
		 LCD_ShowNum(20,160,length,5,12);
//根据red_x1,red_y1的不同值，显示相应内容<--根据靶点到圆心的距离
//			LCD_ShowNum(20,20,i_red,3,12);
//			LCD_ShowNum(20,40,j_red,3,12);
//			LCD_ShowNum(20,60,color_red,5,12);
//			color_red=0;
//			i_red=0;
//			j_red=0;//<---将标志位清零 	
	}		
}

 
}
void camera2_refresh(void)//<---试验函数用来测激光笔中的颜色值，调试函数
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
	NRF24L01_Init();          //初始化发送点坐标无线
  NRF24L01_Init_2();    	//初始化NRF24L01<---接收按键值
  TIM3_Int_Init(4999,7199);//10Khz的计数频率，计数5K次为500ms  
	TIM1_PWM_Init(20000,71);	//1MHZ的分频，每个数就是1us,200000即周期20ms
	TIM1->CCR3=PWM_temp1;       //PA10       
	TIM1->CCR4=PWM_temp2;       //PA11
 	Init_Keypad();
	while(OV7670_Init())//初始化OV7670
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
	
		while(NRF24L01_Check())	//检查发送的NRF24L01是否在位.	
	{
		Gui_StrCenter(0,80,RED,BLUE,"NRF24L01 Init error",16,1);
		delay_ms(200);
	}
	 Gui_StrCenter(0,80,RED,BLUE,"NRF24L01 Init OK",16,1);
	 	NRF24L01_TX_Mode();
	
  	while(NRF24L01_Check_2())	//检查接收的NRF24L01是否在位.	
	{
		Gui_StrCenter(0,80,RED,BLUE,"NRF24L01*2 Init error",16,1);
		delay_ms(200);
	}
	 Gui_StrCenter(0,80,RED,BLUE,"NRF24L01*2 Init OK",16,1);
	 	NRF24L01_RX_Mode_2();


    RCC->APB2ENR|=1<<2;     //使能A时钟
    GPIOA->CRH&=0XFFF0FFFF;
    GPIOA->CRH|=0X00030000;
    GPIOA->ODR&=0X11101111;  //将PA12置低，PA12高电平时点亮	
    Show_Str(240,120,BLUE,PINK,"shoudong",32,0);//<---初始化手动模�
	
	 while(1)
	 {
		camera1_refresh();
    chuansong();
		length_jisuan();
//		if(shoudong_flag==0)//<---该代码是保证在自动模式下打靶准确
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
//			if(key_val==3)//<--八环
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
		  if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//一旦接收到信息,则显示出来.
		 {
			 key_val=(tmp_buf_2[0]-'0')*10+(tmp_buf_2[1]-'0');
       if(shoudong_flag==1)
			{//<---手动模式下				
				if(key_val==9)//PWM_temp1初值1500
				{
					PWM_temp1+=5;
					TIM1->CCR3=PWM_temp1;       //PB6 <--上面左转
					LCD_ShowNum(20,20,PWM_temp1,4,12);  
				}
				if(key_val==10)
				{
					PWM_temp1-=5;
					TIM1->CCR3=PWM_temp1;       //PB6  <--上面右转
					LCD_ShowNum(20,20,PWM_temp1,4,12); 
				}
				if(key_val==7)
				{
					PWM_temp2-=5;
					TIM1->CCR4=PWM_temp2;       //PB7  <---下面上转 
					LCD_ShowNum(20,40,PWM_temp2,4,12); 					
				}
				if(key_val==8)
				{
					PWM_temp2+=5;
					TIM1->CCR4=PWM_temp2;       //PB7  <---下面下转
					LCD_ShowNum(20,40,PWM_temp2,4,12);
				} 
        if(key_val==11)
				{
         jiguang_flag=1;
				}
				if(key_val==12)//<---按下了常亮按钮，激光笔一直亮，当然不在题目要求范围
				{
				jiguang=0; 
				}
				if(key_val==15)
				{
				 shoudong_flag=0;
         Show_Str(240,120,BLUE,PINK,"zidong",32,0);					
				}
			}//<---手动模式下		
      else
			{
					if(key_val==1)//按下了十环按钮
				{
					 PWM_temp1=1440;
					 TIM1->CCR3=PWM_temp1;
           PWM_temp2=1220;       //先粗略的调到这个位置
					 TIM1->CCR4=PWM_temp2;
           jiguang=0;     //点亮激光笔
           delay_ms(1000);//<---延时三毫秒
					 delay_ms(1000);
					 delay_ms(1000);
					 camera1_refresh();
           jiguang=1;				
					
					loop1:		if(ba_dian_x>119+12)//<---点在右边，向左调整
					{
					PWM_temp1+=4;
					TIM1->CCR3=PWM_temp1;       	
					}
					if(ba_dian_x<119-12)//<---点在左边，向右调整
					{
					PWM_temp1-=4;
					TIM1->CCR3=PWM_temp1;       	
					}
					if(ba_dian_y<119-12)//<---点在上边，向下调整
					{
					PWM_temp2+=4;
					TIM1->CCR4=PWM_temp2;       //PB7  <---下面下转
					}
					if(ba_dian_y>119+12)//<---点在下边，向上调整
					{
					PWM_temp2-=4;
					TIM1->CCR4=PWM_temp2;       //PB7  <---下面上转
					}
					
					delay_ms(500);						
					jiguang=0;     //点亮激光笔
          delay_ms(1000);//<---延时三毫秒
					 delay_ms(1000);
					 delay_ms(1000);					
					camera1_refresh();
          jiguang=1;	
					chuansong();
				 if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//跳出自动模式代码
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
          					
				}//<---十环按钮	
				if(key_val==2)//按下九环
				{			
					jiguang=0;     //点亮激光笔
//          delay_ms(1000);//<---延时三毫秒
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop2: if(xiangxian_flag==1)
					{
						if(length>34*34)//<---点在右边，向左调整
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=17*17)//<---点在左边，向右调整
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					else
					{
						if(length>34*34)//<---点在左边，向右调整
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=17*17)//<---点在右边，向左调整
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					delay_ms(500);
					jiguang=0;     //点亮激光笔
//          delay_ms(1000);//<---延时三毫秒
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//跳出自动模式代码
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
						jiguang=1;     //熄灭激光笔
					}
          else
					{						
						goto loop2;
					}
          					
				}//<---九环按钮	
					if(key_val==3)//按下八环
				{
          jiguang=0;     //点亮激光笔
//          delay_ms(1000);//<---延时三毫秒
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop3: if(xiangxian_flag==1)
					{						
						if(length>51*51)//<---点在右边，向左调整
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=34*34)//<---点在左边，向右调整
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
					jiguang=0;     //点亮激光笔
//          delay_ms(1000);//<---延时三毫秒
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//跳出自动模式代码
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
							jiguang=1;     //熄灭激光笔
					}
          else
					{						
						goto loop3;
					}
          					
				}//<---八环按钮	
					if(key_val==4)//按下七环
				{
          jiguang=0;     //点亮激光笔
//          delay_ms(1000);//<---延时三毫秒
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop4:	if(xiangxian_flag==1)
					{						
						if(length>68*68)//<---点在右边，向左调整
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=51*51)//<---点在左边，向右调整
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
				  }
					else
					{
						 if(length>68*68)//<---点在右边，向左调整
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=51*51)//<---点在左边，向右调整
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					delay_ms(500);	
					jiguang=0;     //点亮激光笔
//          delay_ms(1000);//<---延时三毫秒
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//跳出自动模式代码
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
							jiguang=1;     //熄灭激光笔
					}
          else
					{						
						goto loop4;
					}
          					
				}//<---七环按钮	
					if(key_val==5)//按下六环
				{
          jiguang=0;     //点亮激光笔
//          delay_ms(1000);//<---延时三毫秒
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop5:	if(xiangxian_flag==1)
					{						
						if(length>85*85)//<---点在右边，向左调整
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=68*68)//<---点在左边，向右调整
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
					jiguang=0;     //点亮激光笔
//          delay_ms(1000);//<---延时三毫秒
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//跳出自动模式代码
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
							jiguang=1;     //熄灭激光笔
					}
          else
					{						
						goto loop5;
					}
          					
				}//<---六环按钮	
	     if(key_val==6)//按下五环
				{
          jiguang=0;     //点亮激光笔
//          delay_ms(1000);//<---延时三毫秒
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;
          length_jisuan();					
					loop6:	if(xiangxian_flag==1)
					{						
						if(length>102*102)//<---点在右边，向左调整
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=85*85)//<---点在左边，向右调整
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
				  }
					else
					{
						if(length>102*102)//<---点在右边，向左调整
						{
						PWM_temp1-=2;
						TIM1->CCR3=PWM_temp1;       	
						}
						if(length<=85*85)//<---点在左边，向右调整
						{
						PWM_temp1+=2;
						TIM1->CCR3=PWM_temp1;       	
						}
					}
					delay_ms(500);
					jiguang=0;     //点亮激光笔
//          delay_ms(1000);//<---延时三毫秒
//					delay_ms(1000);
//					delay_ms(1000);					
					camera1_refresh();
//          jiguang=1;	
					chuansong();
					if(NRF24L01_RxPacket_2(tmp_buf_2)==0)//跳出自动模式代码
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
							jiguang=1;     //熄灭激光笔
					}
          else
					{						
						goto loop6;
					}         					
				}//<---五环按钮	
				loop7:  if(key_val==11)//<---按下了射击键，则开启定时器
				{
					jiguang_flag=1;
				}
				if(key_val==12)//<---按下了常亮按钮，激光笔一直亮，当然不在题目要求范围
				{
				jiguang=0; 
				}
				if(key_val==16)
				{
				 shoudong_flag=1;//改为手动模式	
				 Show_Str(240,120,BLUE,PINK,"shoudong",32,0);	
				}
			}				
    }//接收到键值
  }//<---while大循环		
   
    			 
	
	
 }
void chuansong()
{
		if(jiao_zhen==1)//<---已处于校正状态下//如果发送00则表示没接收到红点
		{
	     	tmp_buf[0]= ba_dian_x/100+'0';
				tmp_buf[1]= ba_dian_x%100/10+'0';
				tmp_buf[2]= ba_dian_x%10+'0';     //<---依次为靶点坐标的三位，由高到低
				tmp_buf[3]= ba_dian_y/100+'0';
				tmp_buf[4]= ba_dian_y%100/10+'0';
				tmp_buf[5]= ba_dian_y%10+'0';     //<---依次为靶点坐标的三位，由高到低			
				tmp_buf[6]=0;
				if(NRF24L01_TxPacket(tmp_buf)==TX_OK)//<---直到发送成功退出
				{
					Gui_StrCenter(0,30,RED,BLUE,"SUCC",16,0);//居中显示	
				}
				else
				{
					Gui_StrCenter(0,30,RED,BLUE,"LOSE",16,0);//居中显示	
				}			
	  }	
}
void  length_jisuan()//斜边的平方
{
		 if(ba_dian_x>=119&&ba_dian_y>=119)
		 {
		 length=(ba_dian_x-119)*(ba_dian_x-119)+(ba_dian_y-119)*(ba_dian_y-119);
		 xiangxian_flag=1;//第一象限
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

