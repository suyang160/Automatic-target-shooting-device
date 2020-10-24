# Handheld device
This project is developed in keil.

**The connection between TFTILI9341 and STM32F149ZET6**

*We use FSMC of STM32 to driver TFT*
TFTILI9341 | Stm32F149ZET6 |  Pin function
:-: | :-: | :-: 
DB0       | PD14 | data
DB1       | PD15 | data
DB2       | PD0  | data
DB3       | PD1  | data
DB4~DB12  | PE7~PE15|data
DB13      | PD8  | data
DB14      | PD9  | data
DB15      | PD10 | data
LCD_CS	  | PG12 | CS	
LCD_RS 	  | PG0	 | RS/Data
LCD_WR	  | PD5	 | Write
LCD_RD	  | PD4	 | Read
LCD_RST	  | PE5	 | Reset 
LCD_LED	  | PB0	 | LED

**The connection between NRF24L01 and STM32F149ZET6**

NRF24L01 | Stm32F149ZET6 
:-: | :-: 
SPI1_SCK   | PB13     
SPI1_MISO  | PB14     
SPI1_MOSI  | PB15    
INT        | PG6   
CE         | PB12    
CS         | PG7   

**The connection between OV7670 and STM32F149ZET6**
OV7670 | Stm32F149ZET6 
:-: | :-: 
SIOC          |PD3
SIOD          |PG13
VSYNC         |PA8
D0~D7         |PC0~PC7
RST           |PE0(Normal high level)
PWDN          |PE1(Normal low level)
STR(Flashlight ）   |NC
RCK           |PB4
WR            |PB3
OE            |PG15
WRST          |PD6
RRST          |PG14

**TIMER1_CH3 and TIMER1_CH4 of STM32 was used to generate PWM to control the Steering engine.**



