/********************************************
 *
 *  Name:
 *  Email:
 *  Section:
 *  Assignment: Rotary Encoder
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "rotary_encoder.h"
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
volatile unsigned char mois=50;
// current moisture direction
volatile unsigned char moisDirection=0;

//old A value for the moisture rotary encoder
volatile unsigned char oldMA=3;

//old B value for the moisture rotary encoder
volatile unsigned char oldMB=3;


//old A value for the temperature rotary encoder
volatile unsigned char oldTA=3;
//old B value for the temperature rotary encoder
volatile unsigned char oldTB=3;

volatile unsigned char MA;
volatile unsigned char MB;
volatile unsigned char TA;
volatile unsigned char TB;

//change flag for the moisture rotary encoder
volatile unsigned char mois_change=0;
//old B value for the temperature rotary encoder
volatile unsigned char temp_change=0;

volatile unsigned char D=0;
//string moisture level representatives
char* moistureLevel[]={"very dry","dry","avg", "damp","very damp"};
//used to stringout rotary encoder values
char str[4];

void mainRE(void) {
	lcd_init();
	lcd_init_display();

	//initilize rotaryencoder interrupts
	PORTD |=(1<<PD0)|(1<<PD3)|(1<<PD1)|(1<<PD2);
	PCICR|=(1<<PCIE2);
	PCMSK2|=(1<<PCINT16)|(1<<PCINT17)|(1<<PCINT18)|(1<<PCINT19);
	//enable interrupts
	oldMA=(PIND&(1<<PD0));
	oldMB=(PIND&(1<<PD3));
	oldTA=(PIND&(1<<PD2));
	oldTB=(PIND&(1<<PD1));
	sei();
	lcd_moveto(0,0);

	while(1){
		if(mois_change){
			mois_change=0;
			mois_update();
		}
		if(temp_change){
			temp_change=0;
			temp_update();
		}


	}
}

ISR(PCINT2_vect)
{
	D=PIND;

		/* if((PIND&(1<<PD0))!=oldMA){
			lcd_stringout("0");
		}
		if((PIND&(1<<PD3))!=oldMB){
			lcd_stringout("1");
		}
		if((PIND&(1<<PD1))!=oldTA){
			lcd_stringout("2");
		}
		if((PIND&(1<<PD2))!=oldTB){
			lcd_stringout("3");
		} */
	MA=	(D&(1<<PD3));
	MB=	(D&(1<<PD0));
	TA=	(D&(1<<PD1));
	TB=	(D&(1<<PD2));

	if((MA != oldMA) || (MB != oldMB)){
		//checking moisState and changing moisState and moisDirection accordingly
		//PD3: A, PD0: B

		if(moisState==1)
		{
			if(MA!=0)
			{
				moisState=2;
				moisDirection=1;
			}
			if(MB!=0)
			{
				moisState=4;
				moisDirection=0;
			}
		}
		else if (moisState==2)
		{
			if(MA==0)
			{
				moisState=1;
				moisDirection=0;
			}
			if(MB!=0)
			{
				moisState=3;
				moisDirection=1;
			}
		}
		else if (moisState==3)
		{
			if(MA==0)
			{
				moisState=4;
				moisDirection=1;
			}
			if(MB==0)
			{
				moisState=2;
				moisDirection=0;
			}
		}
		else if (moisState==4)
		{
			if(MA!=0)
			{
				moisState=3;
				moisDirection=0;
			}
			if(MB==0)
			{
				moisState=1;
				moisDirection=1;
			}
		}


		oldMA=MA;
		oldMB=MB;
		mois_change=1;

	}

	if((TA != oldTA) || (TB != oldTB)){
		//checking tempState and changing tempState and tempDirection accordingly
		//PD1: A, PD2: B
		if(tempState==1)
		{
			if(TA!=0)
			{
				tempState=2;
				tempDirection=1;
			}
			if(TB!=0)
			{
				tempState=4;
				tempDirection=0;		}
		}
		else if (tempState==2)
		{
			if(TA==0)
			{
				tempState=1;
				tempDirection=0;
			}
			if(TB!=0)
			{
				tempState=3;
				tempDirection=1;
			}
		}
		else if (tempState==3)
		{
			if(TA==0)
			{
				tempState=4;
				tempDirection=1;
			}
			if(TB==0)
			{
				tempState=2;
				tempDirection=0;
			}
		}
		else if (tempState==4)
		{
			if(TA!=0)
			{
				tempState=3;
				tempDirection=0;
			}
			if(TB==0)
			{
				tempState=1;
				tempDirection=1;
			}
		}
		oldTA=(TA);
		oldTB=(TB);
		temp_change=1;

	}

}

void temp_update(){
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


}
void mois_update(){
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
				mois=mois-10;
			}
		}
	//incrementing
		else
		{
			_delay_ms(10);

			if(mois>=120)
			{
				//highest mois
				mois=120;
			}
			else
			{
				//incrementing mois
				mois=mois+10;
			}
		}
	lcd_moveto(0,10);
		lcd_stringout("         ");
		lcd_moveto(0,10);

		if(mois<=20){
			lcd_stringout(moistureLevel[0]);
		}
		else if(mois<=40){
			lcd_stringout(moistureLevel[1]);
		}
		else if(mois<=60){
			lcd_stringout(moistureLevel[2]);
		}
		else if(mois<=80){
			lcd_stringout(moistureLevel[3]);
		}
		else if(mois<=100){
			lcd_stringout(moistureLevel[3]);
		}
		else{
			lcd_stringout(moistureLevel[4]);
		}
		lcd_moveto(0,17);
		snprintf(str,4, "%3d", mois);
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
