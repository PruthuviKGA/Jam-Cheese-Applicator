
#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

void number();				//function for identifying keypad input

int No_of_slices;			//No.of slices(1-9)
int Paste;					//Paste (10=BUTTER , 11=CHEESE , 12=JAM)

int main(void)
{
    DDRB &=~(1<<4);     //DEFINE PORT B4 AS A INPUT..hexadecimal keypad values coming from MC 1
    DDRB &=~(1<<5);		//DEFINE PORT B5 AS A INPUT..hexadecimal keypad values coming from MC 1
    DDRB &=~(1<<6);		//DEFINE PORT B6 AS A INPUT..hexadecimal keypad values coming from MC 1
    DDRB &=~(1<<7);		//DEFINE PORT B7 AS A INPUT..hexadecimal keypad values coming from MC 1
	DDRB|=(1<<0);		//DEFINE PORT B0 AS A OUTPUT..after the whole process signal send to the MC 1
	
	DDRA =0xFF;			//DEFINE PORTA AS A OUTPUT...for stepper motors
	DDRC =0xF0;			//DEFINE PORTA(0-4) AS A INPUT AND (5-8) AS A OUTPUT...for Ir sensors and stepper motor
	DDRD =0xFF;			//DEFINE PORDD AS A OUTPUT...for servo and stepper motors
	
	TCNT1 = 0;			//Set timer1 count zero for servo motors
	ICR1 = 2499;		// Set TOP count for timer1 in ICR1 register for servo motors
	
	while(1)
	{	
		number();		//keypad values(No.of slices,paste and start command(ON))
		if(PINB==0xD0)  //after ON signal coming from MC 1 to start the process
		{
			int i;
			for(i=0;i<No_of_slices;i++)     //this loop execute No.of slices value times
			{
					if(PINC & (1<<PINC0))	//if any bucket over signal coming from MC 1
					{
						break;				//then stop the process
					}
					PORTA &=~(1<<0);		//rotate 0 to 90..x motion for push the bread slices
					PORTA &=~(1<<1);
					PORTA &=~(1<<2);
					PORTA |=(1<<3);
					_delay_ms(50);
					PORTA &=~(1<<0);		//rotate 90 to 180
					PORTA |=(1<<1);
					PORTA &=~(1<<2);
					PORTA &=~(1<<3);
					_delay_ms(350);
					PORTA &=~(1<<0);		//rotate 180 to 90
					PORTA &=~(1<<1);
					PORTA &=~(1<<2);
					PORTA |=(1<<3);
					_delay_ms(50);
					PORTA |=(1<<0);			//rotate 90 to 0
					PORTA &=~(1<<1);
					PORTA &=~(1<<2);
					PORTA &=~(1<<3);
					_delay_ms(250);
				
					if(Paste==10)			//when we select paste as BUTTER
					{
						if(PINC & (1<<PINC1))		//if bread slices detect from IR sensor 4
						{
							if(PINC|(1<<PINC4) && PINC|(1<<PINC5) && PINC&(1<<PINC6) && PINC|(1<<PINC7))	//if butter paste push motor at 270 degree position
							{
								PORTC |=(1<<4);			//then rotate it to the 0 degree position
								PORTC &=~(1<<5);
								PORTC &=~(1<<6);
								PORTC &=~(1<<7);
								_delay_ms(200);
							}
							else if(PINC|(1<<PINC4) && PINC&(1<<PINC5) && PINC|(1<<PINC6) && PINC|(1<<PINC7))	//if butter paste push motor at 180 degree position
							{
								PORTC &=~(1<<4);		//then rotate it to the 270 degree position
								PORTC &=~(1<<5);
								PORTC |=(1<<6);
								PORTC &=~(1<<7);
								_delay_ms(200);
							}
							else if(PINC|(1<<PINC4) && PINC|(1<<PINC5) && PINC|(1<<PINC6) && PINC&(1<<PINC7))	//if butter paste push motor at 90 degree position
							{
								PORTC &=~(1<<4);		//then rotate it to the 180 degree position
								PORTC |=(1<<5);
								PORTC &=~(1<<6);
								PORTC &=~(1<<7);
								_delay_ms(200);
							}
							else if(PINC|(1<<PINC4) && PINC|(1<<PINC5) && PINC|(1<<PINC6) && PINC|(1<<PINC7))	//if butter paste push motor at 0 degree position
							{
								PORTC &=~(1<<4);		//then rotate it to the 90 degree position
								PORTC &=~(1<<5);
								PORTC &=~(1<<6);
								PORTC |=(1<<7);
								_delay_ms(200);
							}
							TCCR1A = (1<<WGM11)|(1<<COM1A1);	// Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clock/64
							TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
							
							OCR1A = 255;	//Set servo motor at 60 position 
							_delay_ms(200);
							OCR1A = 65;		//Set servo motor at 0 position
						}
						else if(PINC | (1<<PINC1))		//if bread slices not detect from IR sensor 4
						{
							PORTD|=(1<<6);				//then send BREAD OVER signal to the MC 1
							break;						//then stop the whole process
						}
					}	
					
					else if(Paste==11)		//when we select paste as CHEESE
					{
						if(PINC & (1<<PINC2))		//if bread slices detect from IR sensor 5
						{
							if(PIND|(1<<PIND0) && PIND|(1<<PIND1) && PIND&(1<<PIND2) && PIND|(1<<PIND3))		//if cheese paste push motor at 270 degree position
							{
								PORTD |=(1<<0);			//then rotate it to the 0 degree position
								PORTD &=~(1<<1);
								PORTD &=~(1<<2);
								PORTD &=~(1<<3);
								_delay_ms(200);
							}
							else if(PIND|(1<<PIND0) && PIND&(1<<PIND1) && PIND|(1<<PIND2) && PIND|(1<<PIND3))	//if cheese paste push motor at 180 degree position
							{
								PORTD &=~(1<<0);		//then rotate it to the 270 degree position
								PORTD &=~(1<<1);
								PORTD |=(1<<2);
								PORTD &=~(1<<3);
								_delay_ms(200);
							}
							else if(PIND|(1<<PIND0) && PIND|(1<<PIND1) && PIND|(1<<PIND2) && PIND&(1<<PIND3))	//if cheese paste push motor at 90 degree position
							{
								PORTD &=~(1<<0);		//then rotate it to the 180 degree position
								PORTD |=(1<<1);
								PORTD &=~(1<<2);
								PORTD &=~(1<<3);
								_delay_ms(200);
							}
							else if(PIND|(1<<PIND0) && PIND|(1<<PIND1) && PIND|(1<<PIND2) && PIND|(1<<PIND3))	//if cheese paste push motor at 0 degree position
							{
								PORTD &=~(1<<0);		//then rotate it to the 90 degree position
								PORTD &=~(1<<1);
								PORTD &=~(1<<2);
								PORTD |=(1<<3);
								_delay_ms(200);
							}
							TCCR1A = (1<<WGM11)|(1<<COM1B1);	// Set Fast PWM, TOP in ICR1, Clear OC1B on compare match, clock/64
							TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
							
							OCR1B = 255;	//Set servo motor at 60 position 
							_delay_ms(200);
							OCR1B = 65;		//Set servo motor at 0 position 
						}
						else if(PINC | (1<<PINC2))		//if bread slices not detect from IR sensor 5
						{
							PORTD|=(1<<6);				//then send BREAD OVER signal to the MC 1
							break;						//then stop the whole process
						}
					}
					
					else if(Paste==12)		//when we select paste as JAM
					{
						if(PINC & (1<<PINC3))		//if bread slices detect from IR sensor 6
						{
							if(PINA|(1<<PINA4) && PINA|(1<<PINA5) && PINA&(1<<PINA6) && PINA|(1<<PINA7))		//if cheese paste push motor at 270 degree position
							{
								PORTA |=(1<<4);			//then rotate it to the 0 degree position
								PORTA &=~(1<<5);
								PORTA &=~(1<<6);
								PORTA &=~(1<<7);
								_delay_ms(200);
							}
							else if(PINA|(1<<PINA4) && PINA&(1<<PINA5) && PINA|(1<<PINA6) && PINA|(1<<PINA7))	//if cheese paste push motor at 180 degree position
							{
								PORTA &=~(1<<4);		//then rotate it to the 270 degree position
								PORTA &=~(1<<5);
								PORTA |=(1<<6);
								PORTA &=~(1<<7);
								_delay_ms(200);
							}
							else if(PINA|(1<<PINA4) && PINA|(1<<PINA5) && PINA|(1<<PINA6) && PINA&(1<<PINA7))	//if cheese paste push motor at 90 degree position
							{
								PORTA &=~(1<<4);		//then rotate it to the 180 degree position
								PORTA |=(1<<5);
								PORTA &=~(1<<6);
								PORTA &=~(1<<7);
								_delay_ms(200);
							}
							else if(PINA|(1<<PINA4) && PINA|(1<<PINA5) && PINA|(1<<PINA6) && PINA|(1<<PINA7))	//if cheese paste push motor at 0 degree position
							{
								PORTA &=~(1<<4);		//then rotate it to the 90 degree position
								PORTA &=~(1<<5);
								PORTA &=~(1<<6);
								PORTA |=(1<<7);
								_delay_ms(200);
							}
							TCCR2 |= (1<<WGM20)|(1<<WGM21);		// Set Fast PWM, TOP in ICR1, Clear OC2 on compare match, clock/64
							TCCR2 |=(1<<COM21)|(0<<CS20)|(0<<CS21)|(1<<CS22);
							
							OCR2 = 65;		//Set servo motor at 60 position
							_delay_ms(200);
							OCR2 = 187;		//Set servo motor at 0 position
							_delay_ms(200);
						}
						else if(PINC | (1<<PINC3))		//if bread slices not detect from IR sensor 6
						{
							PORTD|=(1<<6);				//then send BREAD OVER signal to the MC 1
							break;						//then stop the whole process
						}
					}	
			}//for
			while(1)
			{	
				if(i==No_of_slices)			//after execute No.of slices value times then process stop
				{
					PORTB|=(1<<0);			//when process stop signal send to the MC 1
					break;					//stop the whole process
				}
			}
			
		}//if (press ON)
		
	}//while
	
}//function

void number()		//function for assign keypad values
{
	int x=PINB;		//hexadecimal signal coming from MC 1
	if(x==0x10)
	No_of_slices=1;
	else if(x==0x20)
	No_of_slices=2;
	else if(x==0x30)
	No_of_slices=3;
	else if(x==0x40)
	No_of_slices=4;
	else if(x==0x50)
	No_of_slices=5;
	else if(x==0x60)
	No_of_slices=6;
	else if(x==0x70)
	No_of_slices=7;
	else if(x==0x80)
	No_of_slices=8;
	else if(x==0x90)
	No_of_slices=9;
	if(x==0xA0)
	Paste=10;
	else if(x==0xB0)
	Paste=11;
	else if(x==0xC0)
	Paste=12;
}