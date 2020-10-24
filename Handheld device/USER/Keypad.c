#include  "Keypad.h"
#include  "delay.h"
typedef unsigned char uchar;
typedef unsigned int  uint;

/***************全局变量***************/
u16 key_Pressed;      //按键是否被按下:1--是，0--否
u16 key_val;          //存放键值//
u16 key_Flag;         //按键是否已放开：1--是，0--否
//设置键盘逻辑键值与程序计算键值的映射
u16 key_Map[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

/*******************************************
函数名称：Init_Keypad
功    能：初始化扫描键盘的IO端口
参    数：无
返回值  ：无
********************************************/
void Init_Keypad(void)
{
    RCC->APB2ENR|=1<<7;     //使能F时钟	
    GPIOF->CRL&=0X00000000;
    GPIOF->CRL|=0X88883333;//<--设置PF0~PF3输出，PF4~PF7输入
    GPIOF->ODR|=15<<4;	   //PF4~7上拉
    key_Flag = 0;       
    key_Pressed = 0;   
    key_val = 0;
}
/*******************************************
函数名称：Check_Key
功    能：扫描键盘的IO端口，获得键值
参    数：无
返回值  ：无
********************************************/

void Check_Key(void)
{
    u16 row ,col,tmp1,tmp2;
    
    tmp1 = 0x0001;
    for(col = 0;col < 4;col++)              //列扫描
    {
        GPIOF->ODR &= 0xfff0;                //PF0~4均输出为高 
        GPIOF->ODR |= (~tmp1);                   //PF0~4输出四位中有一个为0
        tmp1 <<=1;                              
        if ((GPIOF->IDR& 0x00F0) < 0x00F0)           //是否P4IN的P4.0~P4.4中有一位为0
        {                       
            tmp2 = 0x0010;                         // tmp2用于检测出那一位为0  
            for(row = 0;row < 4;row ++)              // 行检测 
            {          
                if((GPIOF->IDR& tmp2) == 0x00)           // 是否是该行,等于0为是
                {          
                    key_val = key_Map[row * 4 + col];  // 获取键值
                    while((GPIOF->IDR& 0xF0) < 0xF0)    //按键松开改变值否则一
                    {
                      while((GPIOF->IDR& 0xF0)==0xF0)
                      {
                        return;                         // 退出循环
                      }
                    }
                }
                tmp2 <<= 1;                        // tmp2右移1位 
            }
        }
     }
}
/*******************************************
函数名称：delay
功    能：延时约15ms，完成消抖功能
参    数：无
返回值  ：无
********************************************/
void delay()
{
    uint tmp;
     
    for(tmp = 12000;tmp > 0;tmp--);
}




unsigned char key_flag()         //该函数在键盘初始化后，如果有按键按下，则返回1，否则为0
{
    GPIOF->ODR&=0xfff0;              // 设置PF0~3为低电平
    if((GPIOF->IDR& 0x00F0) < 0x00F0)
    {
      delay_ms(20);             //防止抖动
      if((GPIOF->IDR& 0x00F0) < 0x00F0)
      {
       return 1;
      }
      else
      {
        return 0;
      }
    }
    else
    {
      return 0;
    }
  
}

