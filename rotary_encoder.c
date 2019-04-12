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


//used to stringout rotary encoder values
char str[4];
//string moisture level representatives
char* moistureLevel[]={"very dry","dry","avg", "damp","very damp"};

void temp_update(volatile unsigned char tempDirection, volatile unsigned char *temp){
			//decrementing
		if(tempDirection==0)
		{
			_delay_ms(10);
			if((*temp)<=10)
			{
				//lowest temp
				(*temp)=10;
			}
			else
			{
				//decrementing temp
				(*temp)=(*temp)-10;
			}
		}
	//incrementing
		else
		{
			_delay_ms(10);
			if((*temp)>=120)
			{
				//highest temp
				(*temp)=120;
			}
			else
			{
				//incrementing temp
				(*temp)=(*temp)+10;
			}
		}

		lcd_moveto(1,15);
		snprintf(str,4, "%3d", (*temp));
		lcd_stringout(str);


}
void mois_update(volatile unsigned char moisDirection, volatile unsigned char *mois){
		//decrementing
		if(moisDirection==0)
		{
			_delay_ms(10);

			if((*mois)<=0)
			{
				//lowest mois
				(*mois)=0;
			}
			else
			{
				//decrementing mois
				(*mois)=(*mois)-10;
			}
		}
	//incrementing
		else
		{
			_delay_ms(10);

			if((*mois)>=120)
			{
				//highest mois
				(*mois)=120;
			}
			else
			{
				//incrementing mois
				(*mois)=(*mois)+10;
			}
		}
	lcd_moveto(0,10);
		lcd_stringout("         ");
		lcd_moveto(0,10);

		if((*mois)<=20){
			lcd_stringout(moistureLevel[0]);
		}
		else if((*mois)<=40){
			lcd_stringout(moistureLevel[1]);
		}
		else if((*mois)<=60){
			lcd_stringout(moistureLevel[2]);
		}
		else if((*mois)<=80){
			lcd_stringout(moistureLevel[3]);
		}
		else if((*mois)<=100){
			lcd_stringout(moistureLevel[3]);
		}
		else{
			lcd_stringout(moistureLevel[4]);
		}
		lcd_moveto(0,17);
		snprintf(str,4, "%3d", (*mois));
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
