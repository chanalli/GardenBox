/********************************************
*
*  Name: Xiaoying Guo
*  Section: Wed 3:30-4:50
*  Assignment: Lab 6 - Write to LCD display
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#define Maskbits 0x1e
#include "lcd.h"

void writenibble(unsigned char);

//int main(void) {

    /* Main program goes here */
	//initiating both adc conversion and lcd
	
	//init_lcd();
	
	

   // return 0;   /* never reached */
//}

/*
  init_lcd - Do various things to initialize the LCD display
*/
void init_lcd()
{
	DDRC|=(1<<6)
	DDRD|=(1<<0);
	DDRD|=(1<<1)|(1<<2)|(1<<3)|(1<<4);
    _delay_ms(15);              // Delay at least 15ms

    writenibble(0x03);                   // Use writenibble to send 0011
    _delay_ms(5);               // Delay at least 4msec

	writenibble(0x03);                 // Use writenibble to send 0011
    _delay_us(120);             // Delay at least 100usec

    writenibble(0x03);// Use writenibble to send 0011, no delay needed

    writenibble(0x02);                  // Use writenibble to send 0010
    _delay_ms(2);               // Delay at least 2ms
    
    writecommand(0x28);         // Function Set: 4-bit interface, 2 lines

    writecommand(0x0f);         // Display and cursor on

}

/*
  moveto - Move the cursor to the row and column given by the arguments.
*/
void moveto(unsigned char row, unsigned char col)
{
if(row==1)
	writecommand(0x80+col-1);
else
	writecommand(0xc0+col-1);

}

/*
  stringout - Print the contents of the character string "str"
  at the current cursor position.
*/
void stringout(char *str)

{	int i=0;                                                                                                                                                         
	while (str[i]!=0)
	{
	writedata(str[i]);
	i++;
	}
}

/*
  writecommand - Output a byte to the LCD command register.
*/
void writecommand(unsigned char x)
{
	PORTB &= ~(1<<PB0);
	writenibble(x>>4);
	writenibble(x);
	_delay_ms(2);
}

/*
  writedata - Output a byte to the LCD data register
*/
void writedata(unsigned char x)
{
	PORTB |=(1<<PB0);
	writenibble(x>>4);
	writenibble(x);
	_delay_ms(2);
}

/*
  writenibble - Output four bits from "x" to the LCD
 
*/

void writenibble(unsigned char x)
{
	PORTD &= ~Maskbits;
	PORTD |=(x<<PD1)&Maskbits;
	PORTB |=(1<<PB1);
	PORTB|=(1<<PB1);
	PORTB &= ~(1<<PB1);
}


