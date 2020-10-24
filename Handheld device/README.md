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
SPI1_SCK   | PA5     
SPI1_MISO  | PA6     
SPI1_MOSI  | PA7     
INT        | PG5   
CE         | PA4    
CS         | PG4   



