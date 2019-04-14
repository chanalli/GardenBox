/***********a**************************************************
* Control C file to call all sensors and LCD display
*
*
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include "rotary_encoder.h"
#include "lcd.h"
#include "moisture.h"
#include "LEDTest.h"


// temperature variables
volatile unsigned char tempState=1;
volatile unsigned char temp=50;
volatile unsigned char tempDirection=0;

// moisture variables
volatile unsigned char moisState=1;
volatile unsigned char mois=50;
volatile unsigned char moisDirection=0;

//old values for A and B
volatile unsigned char oldMA=3;
volatile unsigned char oldMB=3;
volatile unsigned char oldTA=3;
volatile unsigned char oldTB=3;
//current A and B values
volatile unsigned char MA;
volatile unsigned char MB;
volatile unsigned char TA;
volatile unsigned char TB;

//change flags
volatile unsigned char mois_change=0;
volatile unsigned char temp_change=0;

volatile unsigned char D=0;
volatile unsigned char C=0;
//UVRAY registers
uint8_t rdata[2]; 
uint8_t commandCode=0;
uint8_t config[2]={0,0};


int main(void){
  //set all output input here or in each .c file?
  //initilize lcd
  lcd_init();
  lcd_init_display();
  //initilize rotaryencoder interrupts
	PORTD |=(1<<PD3)|(1<<PD2);
	PORTC |=(1<<PC1)|(1<<PC0);
	PCICR|=(1<<PCIE2);
	PCICR|=(1<<PCIE1);
	PCMSK2|=(1<<PCINT18)|(1<<PCINT19);
	PCMSK1|=(1<<PCINT8)|(1<<PCINT9);
	
	//enable interrupts
	oldMA=(PINC&(1<<PC1));
	oldMB=(PIND&(1<<PD3));
	oldTA=(PIND&(1<<PD2));
	oldTB=(PINC&(1<<PC0));
	sei();
	uv_init(&commandCode, config);
	commandCode=7;
	
  while(1){
		if(mois_change){
			mois_change=0;
			mois_update(moisDirection, &mois);
		}
		if(temp_change){
			temp_change=0;
			temp_update(tempDirection, &temp);
		}
		check_uv(&commandCode, rdata);
		
  }
  return 0;
}
ISR(PCINT1_vect)
{
	D=PIND;
	C=PINC;
	MA=	(D&(1<<PD3));
	MB=	(C&(1<<PC1));
	TA=	(C&(1<<PC0));
	TB=	(D&(1<<PD2));

	if((MA != oldMA) || (MB != oldMB)){
		//checking moisState and changing moisState and moisDirection accordingly
		//PD3: A, PC1: B

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
		//PC0: A, PD2: B
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
ISR(PCINT2_vect)
{
	D=PIND;
	C=PINC;
	MA=	(D&(1<<PD3));
	MB=	(C&(1<<PC1));
	TA=	(C&(1<<PC0));
	TB=	(D&(1<<PD2));

	if((MA != oldMA) || (MB != oldMB)){
		//checking moisState and changing moisState and moisDirection accordingly
		//PD3: A, PC1: B

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
		//PC0: A, PD2: B
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
