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
uint8_t UVA[2];
uint8_t UVB[2];
uint8_t UVComp1[2];
uint8_t UVComp2[2];
uint8_t IDnum[2];
float index;
//UVRAY codes
uint8_t configCode=0;
uint8_t UVACode=7;
uint8_t UVBCode=9;
uint8_t UVComp1Code=10;
uint8_t UVComp2Code=11;
uint8_t IDnumCode=12;
uint8_t config[2]={0,0};

//moisture
void initPorts();
int moisture = 0;

int main(void){
  //set all output input here or in each .c file?
  initPorts();

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
	//setting up i2c
	uv_init(&configCode, config);


  while(1){
		//checking rotary encoders
		if(mois_change){
			mois_change=0;
			mois_update(moisDirection, &mois);
		}
		if(temp_change){
			temp_change=0;
			temp_update(tempDirection, &temp);
		}
		//checking UV
		i2c_io(0x20, &UVACode, 1, NULL, 0, UVA, 2);
		i2c_io(0x20, &UVBCode, 1, NULL, 0, UVB, 2);
		i2c_io(0x20, &UVComp1Code, 1, NULL, 0, UVComp1, 2);
		i2c_io(0x20, &UVComp2Code, 1, NULL, 0, UVComp2, 2);
		index = get_index(UVA, UVB, UVComp1, UVComp2);
		display_UV_level(index);

    //check moisture
    moisture = readMoisture();
    if(moisture < 50){
      //turn on pump
      PORTB |= 0x01;
    }
    else if(moisture > 200){
      //turn off pump
      PORTB &= 0xFE;
    }

  }

  return 0;
}

void initPorts(){
  //PB1 & PB0 OUTPUT to moisture sensor & relay/pump
  DDRB = 0x03;
  //makes relay and moisture OUTPUT low at first
  PORTB  = 0x00;

  //make PC2 analog INPUT
  DDRC &= 0xFB; //1111 1011

  //initiaize ADC registers
  //ADMUX: 01(AVCC input)0/1(10bit/8bits)0 0000(MUX3:MUX0 for input channel)
  // 0110 0010 = 0x62
  ADMUX = 0x62;
  //ADCSRA: 1 (enable conversation) 0 (start conversation = 1) 00 0(enables interrupts) 000(prescaler value 11 for 128)
  // 1000 0111 = 0x87;
  ADCSRA = 0x87;

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
