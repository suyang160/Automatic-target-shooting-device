#include  "Keypad.h"
#include  "delay.h"
typedef unsigned char uchar;
typedef unsigned int  uint;

/***************ȫ�ֱ���***************/
u16 key_Pressed;      //�����Ƿ񱻰���:1--�ǣ�0--��
u16 key_val;          //��ż�ֵ//
u16 key_Flag;         //�����Ƿ��ѷſ���1--�ǣ�0--��
//���ü����߼���ֵ���������ֵ��ӳ��
u16 key_Map[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

/*******************************************
�������ƣ�Init_Keypad
��    �ܣ���ʼ��ɨ����̵�IO�˿�
��    ������
����ֵ  ����
********************************************/
void Init_Keypad(void)
{
    RCC->APB2ENR|=1<<7;     //ʹ��Fʱ��	
    GPIOF->CRL&=0X00000000;
    GPIOF->CRL|=0X88883333;//<--����PF0~PF3�����PF4~PF7����
    GPIOF->ODR|=15<<4;	   //PF4~7����
    key_Flag = 0;       
    key_Pressed = 0;   
    key_val = 0;
}
/*******************************************
�������ƣ�Check_Key
��    �ܣ�ɨ����̵�IO�˿ڣ���ü�ֵ
��    ������
����ֵ  ����
********************************************/

void Check_Key(void)
{
    u16 row ,col,tmp1,tmp2;
    
    tmp1 = 0x0001;
    for(col = 0;col < 4;col++)              //��ɨ��
    {
        GPIOF->ODR &= 0xfff0;                //PF0~4�����Ϊ�� 
        GPIOF->ODR |= (~tmp1);                   //PF0~4�����λ����һ��Ϊ0
        tmp1 <<=1;                              
        if ((GPIOF->IDR& 0x00F0) < 0x00F0)           //�Ƿ�P4IN��P4.0~P4.4����һλΪ0
        {                       
            tmp2 = 0x0010;                         // tmp2���ڼ�����һλΪ0  
            for(row = 0;row < 4;row ++)              // �м�� 
            {          
                if((GPIOF->IDR& tmp2) == 0x00)           // �Ƿ��Ǹ���,����0Ϊ��
                {          
                    key_val = key_Map[row * 4 + col];  // ��ȡ��ֵ
                    while((GPIOF->IDR& 0xF0) < 0xF0)    //�����ɿ��ı�ֵ����һ
                    {
                      while((GPIOF->IDR& 0xF0)==0xF0)
                      {
                        return;                         // �˳�ѭ��
                      }
                    }
                }
                tmp2 <<= 1;                        // tmp2����1λ 
            }
        }
     }
}
/*******************************************
�������ƣ�delay
��    �ܣ���ʱԼ15ms�������������
��    ������
����ֵ  ����
********************************************/
void delay()
{
    uint tmp;
     
    for(tmp = 12000;tmp > 0;tmp--);
}




unsigned char key_flag()         //�ú����ڼ��̳�ʼ��������а������£��򷵻�1������Ϊ0
{
    GPIOF->ODR&=0xfff0;              // ����PF0~3Ϊ�͵�ƽ
    if((GPIOF->IDR& 0x00F0) < 0x00F0)
    {
      delay_ms(20);             //��ֹ����
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

