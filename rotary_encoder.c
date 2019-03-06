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
	//enable timer interrupt
	TIMSK1|=(1<<OCIE1A);


	//enable interrupts
	sei();

	//initilize rotaryencoder interrupts
	PORTB |=(1<<PB3)|(1<<PB5);
	PCICR|=(1<<PCIE0);
	PCMSK0|=(1<<PCINT3)|(1<<PCINT5);

	return 0;

}

ISR(PCINT0_vect)
{
	//checking state and changing state and direction accordingly
	if(state==1)
	{
		if((PINB&(1<<PB3))!=0)
		{
			state=2;
			direction=1;
		}
		if((PINB&(1<<PB5))!=0)
		{
			state=4;
			direction=0;
		}
	}
	else if (state==2)
	{
		if((PINB&(1<<PB3))==0)
		{
			state=1;
			direction=0;
		}
		if((PINB&(1<<PB5))!=0)
		{
			state=3;
			direction=1;
		}
	}
	else if (state==3)
	{
		if((PINB&(1<<PB3))==0)
		{
			state=4;
			direction=1;
		}
		if((PINB&(1<<PB5))==0)
		{
			state=2;
			direction=0;
		}
	}
	else if (state==4)
	{
		if((PINB&(1<<PB3))!=0)
		{
			state=3;
			direction=0;
		}
		if((PINB&(1<<PB5))==0)
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
	lcd_stringout(str);
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
