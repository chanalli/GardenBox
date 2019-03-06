/********************************************
 *
 *  Name:
 *  Email:
 *  Section:
 *  Assignment: Lab 7 - Rotary Encoder
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include <avr/interrupt.h>

#include "lcd.h"
volatile unsigned char state=1;
volatile unsigned char count=5;
volatile unsigned char direction=0;
char str[4];
void variable_delay_us(int);


int main(void) {
	lcd_init();
	lcd_init_display();
	
	
	
	//initilize rotaryencoder interrupts
	PORTD |=(1<<PD1)|(1<<PD2);
	PCICR|=(1<<PCIE2);
	PCMSK2|=(1<<PCINT17)|(1<<PCINT18);
	//enable interrupts
	sei();
	
	lcd_moveto(4,0);
	lcd_stringout("hello");
	while(1){}
	return 0;
	
}

ISR(PCINT2_vect)
{
	lcd_moveto(0,0);
	lcd_stringout("hi");
	//checking state and changing state and direction accordingly
	if(state==1)
	{
		if((PIND&(1<<PD3))!=0)
		{
			state=2;
			direction=1;
		}
		if((PIND&(1<<PD5))!=0)
		{
			state=4;
			direction=0;
		}
	}
	else if (state==2)
	{
		if((PIND&(1<<PD3))==0)
		{
			state=1;
			direction=0;
		}
		if((PIND&(1<<PD5))!=0)
		{
			state=3;
			direction=1;
		}
	}
	else if (state==3)
	{
		if((PIND&(1<<PD3))==0)
		{
			state=4;
			direction=1;
		}
		if((PIND&(1<<PD5))==0)
		{
			state=2;
			direction=0;
		}
	}
	else if (state==4)
	{
		if((PIND&(1<<PD3))!=0)
		{
			state=3;
			direction=0;
		}
		if((PIND&(1<<PD5))==0)
		{
			state=1;
			direction=1;
		}			
	}
	
	//decrementing
	if(direction==0)
	{
		if(count<=0)
		{
			//lowest count
			count=0;
		}
		else
		{
			//decrementing count
			count=count-1;
		}
	}
//incrementing	
	else
	{
		if(count>=25)
		{
			//highest count
			count=25;
		}	
		else
		{
			//incrementing count
			count=count+1;
		}
	}
	//writecommand(0x01);
	lcd_moveto(3,0);
	sprintf(str, "%d", count);
	lcd_stringout("hello");
}



/*
    variable_delay_us - Delay a variable number of microseconds
*/
void variable_delay_us(int delay)
{
    int i = (delay + 5) / 10;

    while (i--)
        _delay_us(10);
}