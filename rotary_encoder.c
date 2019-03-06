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
// current temperature rotary state
volatile unsigned char tempState=1;
// current temperature value
volatile unsigned char temp=50;
// direction for temperature rotating direction
volatile unsigned char tempDirection=0;

// current moisture rotary state
volatile unsigned char moisState=1;
// cuurent moisture value
volatile unsigned char mois=70;
// current moisture direction
volatile unsigned char moisDirection=0;

//old A value for the moisture rotary encoder
volatile unsigned char oldMA=0;
//old B value for the moisture rotary encoder
volatile unsigned char oldMB=0;

//old A value for the temperature rotary encoder
volatile unsigned char oldTA=0;
//old B value for the temperature rotary encoder
volatile unsigned char oldTB=0;

//string moisture level representatives
char* moistureLevel[]={"dry","avg","wet"};
//used to stringout rotary encoder values
char str[4];

void variable_delay_us(int);


int main(void) {
	lcd_init();
	lcd_init_display();
	
	//initilize rotaryencoder interrupts
	PORTD |=(1<<PD0)|(1<<PD3)|(1<<PD1)|(1<<PD2);
	PCICR|=(1<<PCIE2);
	PCMSK2|=(1<<PCINT16)|(1<<PCINT17)|(1<<PCINT18)|(1<<PCINT19);
	//enable interrupts
	sei();
	
	while(1){
	}
	return 0;
	
}

ISR(PCINT2_vect)
{
	if(PD0!=oldMA || PD3!=oldMB){
		//checking moisState and changing moisState and moisDirection accordingly
		//PD1: A, PD2: B
		if(moisState==1)
		{
			if((PIND&(1<<PD0))!=0)
			{
				moisState=2;
				moisDirection=1;
			}
			if((PIND&(1<<PD3))!=0)
			{
				moisState=4;
				moisDirection=0;
			}
		}
		else if (moisState==2)
		{
			if((PIND&(1<<PD0))==0)
			{
				moisState=1;
				moisDirection=0;
			}
			if((PIND&(1<<PD3))!=0)
			{
				moisState=3;
				moisDirection=1;
			}
		}
		else if (moisState==3)
		{
			if((PIND&(1<<PD0))==0)
			{
				moisState=4;
				moisDirection=1;
			}
			if((PIND&(1<<PD3))==0)
			{
				moisState=2;
				moisDirection=0;
			}
		}
		else if (moisState==4)
		{
			if((PIND&(1<<PD0))!=0)
			{
				moisState=3;
				moisDirection=0;
			}
			if((PIND&(1<<PD3))==0)
			{
				moisState=1;
				moisDirection=1;
			}			
		}
		
			//decrementing
		if(moisDirection==0)
		{
			_delay_ms(10);
			if(mois<=0)
			{
				//lowest mois
				mois=0;
			}
			else
			{
				//decrementing mois
				mois=mois-1;
			}
		}
	//incrementing	
		else
		{
			_delay_ms(10);
			if(mois>=100)
			{
				//highest mois
				mois=100;
			}	
			else
			{
				//incrementing mois
				mois=mois+1;
			}
		}
		lcd_moveto(0,13);
		if(mois<=33){
			lcd_stringout(moistureLevel[0]);
		}
		else if(mois>33 && mois<=66){
			lcd_stringout(moistureLevel[0]);
		}
		else{
			lcd_stringout(moistureLevel[2]);
		}
		oldMA=PD0;
		oldMB=PD3;
	}
	
	if(oldTA!=PD1 || oldTB!=PD2){
		//checking tempState and changing tempState and tempDirection accordingly
		//PD1: A, PD2: B
		if(tempState==1)
		{
			if((PIND&(1<<PD1))!=0)
			{
				tempState=2;
				tempDirection=1;
			}
			if((PIND&(1<<PD2))!=0)
			{
				tempState=4;
				tempDirection=0;
			}
		}
		else if (tempState==2)
		{
			if((PIND&(1<<PD1))==0)
			{
				tempState=1;
				tempDirection=0;
			}
			if((PIND&(1<<PD2))!=0)
			{
				tempState=3;
				tempDirection=1;
			}
		}
		else if (tempState==3)
		{
			if((PIND&(1<<PD1))==0)
			{
				tempState=4;
				tempDirection=1;
			}
			if((PIND&(1<<PD2))==0)
			{
				tempState=2;
				tempDirection=0;
			}
		}
		else if (tempState==4)
		{
			if((PIND&(1<<PD1))!=0)
			{
				tempState=3;
				tempDirection=0;
			}
			if((PIND&(1<<PD2))==0)
			{
				tempState=1;
				tempDirection=1;
			}			
		}
		
		//decrementing
		if(tempDirection==0)
		{
			_delay_ms(10);
			if(temp<=10)
			{
				//lowest temp
				temp=10;
			}
			else
			{
				//decrementing temp
				temp=temp-10;
			}
		}
	//incrementing	
		else
		{
			_delay_ms(10);
			if(temp>=120)
			{
				//highest temp
				temp=120;
			}	
			else
			{
				//incrementing temp
				temp=temp+10;
			}
		}
		lcd_moveto(1,15);
		snprintf(str,4, "%3d", temp);
		lcd_stringout(str);
		oldTA=PD1;
		oldTB=PD2;
	}	
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