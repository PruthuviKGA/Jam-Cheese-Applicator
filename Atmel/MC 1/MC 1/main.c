
#include <stdio.h>
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"
#include "LCD_I2C.h"

char signal_for_MC2[4][4]= {0x70,0x80,0x90,0xA0,0x40,0x50,0x60,0xB0,0x10,0x20,0x30,0xC0,0xD0,0x00,0x00,0x00};		//PORTB output for signals send to the MC 2
int key[4][4]={7,8,9,10,4,5,6,11,1,2,3,12,13,14,14,14};																//identifying number of slices for display the Number
	
 void search();				//keypad function
 void keypad();				//keypad function
 void number();				//Number display function
 void conveyor();			//conveyor belt function
 
 int r;						//keypad rows
 int x=0;					//for keypad assign PORTD
 int keypad_value;			//keypad values 
 
int main(void)
{
	DDRB|=(1<<4);		//B4 keypad output send to the MC 2
	DDRB|=(1<<5);		//B5 keypad output send to the MC 2
	DDRB|=(1<<6);		//B6 keypad output send to the MC 2
	DDRB|=(1<<7);		//B7 keypad output send to the MC 2
	
	DDRD=0xF0;			//keypad PORTD(1-4) INPUT  (5-8) OUTPUT
	PORTD=0x0F;			//keypad PORTD(1-4) =1  (5-8) =0
	
	DDRC&=~(1<<2);		//C2 input...if BREAD over then signal coming from MC 2
	DDRC|=(1<<3);		//C3 output...ALARM output
	DDRC&=~(1<<4);		//C4 input...BUTTER bucket checking IR
	DDRC&=~(1<<5);		//C5 input...CHEESE bucket checking IR
	DDRC&=~(1<<6);		//C6 input...JAM bucket checking IR
	DDRC|=(1<<7);		//C7 output...If any bucket over,then signal send to the MC 2
	
	DDRA|=(1<<0);		//A0 output..Conveyor Belt
	DDRA|=(1<<1);		//A1 output..Conveyor Belt
	DDRA|=(1<<2);		//A2 output..Conveyor Belt
	DDRA|=(1<<3);		//A3 output..Conveyor Belt
	DDRA&=~(1<<7);		//A7 input...after the whole process signal coming from MC 2
	
	i2c_init();				 //LCD screen ON
	i2c_start();
	i2c_write(0x70);
	lcd_init();

	lcd_cmd(0x80);			//first line
	lcd_msg("WELCOME...");
	_delay_ms(100);
	lcd_cmd(0x01);			//clean display
	_delay_ms(50);
	lcd_cmd(0x80);			//first line
	lcd_msg("Enter Number");
	_delay_ms(50);
	lcd_cmd(0xC0);			//second line
	lcd_msg("of Slices - ");
	keypad();				//keypad input
	number();				//number of slices
	_delay_ms(100);
	PORTD=0x0F;				//keypad RESET...PORTD(1-4) =1  (5-8) =0
	
	lcd_cmd(0x01);			//clean display
	_delay_ms(50);
	lcd_cmd(0x80);			//first line
	lcd_msg("Enter the ");
	_delay_ms(50);
	lcd_cmd(0xC0);			//second line
	lcd_msg("Paste - ");
	keypad();				//keypad input
	number();				//Paste
	_delay_ms(100);
	PORTD=0x0F;				//keypad RESET...PORTD(1-4) =1  (5-8) =0
	
	lcd_cmd(0x01);			//clean display
	_delay_ms(50);
	lcd_cmd(0x80);			//first line
	lcd_msg("Press ON");
	if(PINC & (1<<PINC4) || PINC & (1<<PINC5) || PINC & (1<<PINC6))				//at the Beginning check that any bucket is over
	{
	PORTC|=(1<<7);							//signal send to the MC 2 for stop the process
	}
	keypad();				//keypad input
	lcd_cmd(0x01);
	_delay_ms(50);
	number();				//processing
	_delay_ms(50);
	
	while(1)				//below process execute again and again till end the whole process
	{
		if(PINC & (1<<PINC4) || PINC & (1<<PINC5) || PINC & (1<<PINC6))			//while processing check that if any bucket is over
		{
			PORTC|=(1<<7);					//signal send to the MC 2 for stop the process
			PORTC|=(1<<3);					//ALARM ON
			if(PINC & (1<<PINC4))			//If IR sensor 1(Butter) is ON then...
			{
				lcd_cmd(0x01);				//clean display
				lcd_cmd(0x80);				//first line
				lcd_msg("BUTTER-OVER");
				lcd_cmd(0xC0);				//second line
				lcd_msg("Please Refill");
				_delay_ms(100);
				PORTC&=~(1<<3);				//ALARM OFF
			}
			else if(PINC & (1<<PINC5))		//If IR sensor 2(Cheese) is ON then...
			{
				lcd_cmd(0x01);				//clean display
				lcd_cmd(0x80);				//first line
				lcd_msg("CHEESE-OVER");
				lcd_cmd(0xC0);				//second line
				lcd_msg("Please Refill");
				_delay_ms(100);
				PORTC&=~(1<<3);				//ALARM OFF
			}
			else if(PINC & (1<<PINC6))		//If IR sensor 2(Jam) is ON then...
			{
				lcd_cmd(0x01);				//clean display
				lcd_cmd(0x80);				//first line
				lcd_msg("JAM-OVER");	
				lcd_cmd(0xC0);				//second line
				lcd_msg("Please Refill");
				_delay_ms(100);
				PORTC&=~(1<<3);				//ALARM OFF
			}
		break;								//while processing check that any bucket is over,if any bucket is over then stop the process
		}
		else if(PINC | (1<<PINC4) && PINC | (1<<PINC5) && PINC | (1<<PINC6))		//while processing check that if any bucket is not over
		{
			PORTC&=~(1<<7);					//then send signal to the MC 2 for continue the process
		}
		
		conveyor();					//rotate conveyor belt while end the process
		
		if(PINC & (1<<PINC2))		//while process if BREAD over then signal coming from MC 2
		{
			PORTC|=(1<<3);			//ALARM ON
			lcd_cmd(0x01);
			lcd_cmd(0x80);			//first line
			lcd_msg("BREAD-OVER");
			lcd_cmd(0xC0);			//second line
			lcd_msg("Please Refill");
			_delay_ms(100);
			PORTC&=~(1<<3);			//ALARM OFF
			break;					//stop the process
		}
		if(PINA & (1<<PINA7))		//after the whole process signal coming from MC 2
		{
			_delay_ms(200);
			lcd_cmd(0x01);
			lcd_cmd(0x80);			//first line
			lcd_msg("DONE !");
			lcd_cmd(0xC0);			//second line
			lcd_msg("ENJOY YOUR MEAL");
			break;					//stop the process
		}
	}
}

void keypad()					//function for keypad
{
		while(1)
		{	x=PIND;			
			if(x!=0x0f)			//for keypad properly work PORTD(1-4) must be 1 and PORTD(5-8) must be 0
			{break;				//if It is not stop the keypad process
			}
		}
		PORTD=0xEF;				//Set A row as 0
		x=PIND;
		if(x!=0xEF)				//check keypad signal coming from A row
		{	r=0;				
			search();			//execute search function for assign values(7,8,9,A) for A row
		}
		PORTD=0xDF;				//Set B row as 0
		x=PIND;
		if(x!=0xDF)				//check keypad signal coming from B row
		{	r=1;
			search();			//execute search function for assign values(4,5,6,B) for B row
		}
		PORTD=0xBF;				//Set C row as 0
		x=PIND;
		if(x!=0xBF)				//check keypad signal coming from C row
		{	r=2;
			search();			//execute search function for assign values(1,2,3,C) for B row
		}
		PORTD=0x7F;				//Set D row as 0
		x=PIND;
		if(x!=0x7F)				//check keypad signal coming from D row
		{	r=3;
			search();			//execute search function for assign values(ON,ERROR,ERROR,ERROR) for B row
		}
}

void search()						//function for 'keypad function'
{
	x=x & 0x0F;						//keypad PORTD(1-4) =1  (5-8) =0
	
	if(x==0x0E)						//check keypad signal coming from column 1
	{PORTB=signal_for_MC2[r][0];	//hexadecimal signal send to the MC 2
	keypad_value=key[r][0];}		//assign values(7,4,1,ON) for column 1
	
	if(x==0x0D)						//check keypad signal coming from column 2
	{PORTB=signal_for_MC2[r][1];	//hexadecimal signal send to the MC 2
	keypad_value=key[r][1];}		//assign values(8,5,2,ERROR) for column 2
	
	if(x==0x0B)						//check keypad signal coming from column 3
	{PORTB=signal_for_MC2[r][2];	//hexadecimal signal send to the MC 2
	keypad_value=key[r][2];}		//assign values(9,6,3,ERROR) for column 3
	
	if(x==0x07)						//check keypad signal coming from column 4
	{PORTB=signal_for_MC2[r][3];	//hexadecimal signal send to the MC 2
	keypad_value=key[r][3];}		//assign values(A,B,C,ERROR) for No column 4
}

void number()			//function for display the keypad values
{
	if(keypad_value==1)
	lcd_msg("1");
	else if(keypad_value==2)
	lcd_msg("2");
	else if(keypad_value==3)
	lcd_msg("3");
	else if(keypad_value==4)
	lcd_msg("4");
	else if(keypad_value==5)
	lcd_msg("5");
	else if(keypad_value==6)
	lcd_msg("6");
	else if(keypad_value==7)
	lcd_msg("7");
	else if(keypad_value==8)
	lcd_msg("8");
	else if(keypad_value==9)
	lcd_msg("9");
	else if(keypad_value==10)
	lcd_msg("BUTTER");
	else if(keypad_value==11)
	lcd_msg("CHEESE");
	else if(keypad_value==12)
	lcd_msg("JAM");
	else if(keypad_value==13)
	lcd_msg("PROCESSING...");
	else if(keypad_value==14)
	lcd_msg("ERROR");
}

void conveyor()					//function for conveyor belt
{
	PORTA &=~(1<<0);			//rotate 0 to 90 Conveyor Belt
	PORTA &=~(1<<1);			//rotate 0 to 90 Conveyor Belt
	PORTA &=~(1<<2);			//rotate 0 to 90 Conveyor Belt
	PORTA |=(1<<3);				//rotate 0 to 90 Conveyor Belt
	_delay_ms(150);
	PORTA &=~(1<<0);			//rotate 90 to 180 Conveyor Belt
	PORTA |=(1<<1);				//rotate 90 to 180 Conveyor Belt
	PORTA &=~(1<<2);			//rotate 90 to 180 Conveyor Belt
	PORTA &=~(1<<3);			//rotate 90 to 180 Conveyor Belt
	_delay_ms(150);
	PORTA &=~(1<<0);			//rotate 180 to 270 Conveyor Belt
	PORTA &=~(1<<1);			//rotate 180 to 270 Conveyor Belt
	PORTA |=(1<<2);				//rotate 180 to 270 Conveyor Belt
	PORTA &=~(1<<3);			//rotate 180 to 270 Conveyor Belt
	_delay_ms(150);
	PORTA |=(1<<0);				//rotate 270 to 360 Conveyor Belt
	PORTA &=~(1<<1);			//rotate 270 to 360 Conveyor Belt
	PORTA &=~(1<<2);			//rotate 270 to 360 Conveyor Belt
	PORTA &=~(1<<3);			//rotate 270 to 360 Conveyor Belt
	_delay_ms(150);
}