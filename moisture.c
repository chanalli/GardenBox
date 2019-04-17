#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "moisture.h"

int readMoisture(){

  unsigned int value = 0;

  //PB1 high signal
  PORTB |= 0x02;
  //wait 10 milliseconds
  _delay_ms(1000);

  // //test
  // PORTB &= 0xFD;
  // _delay_ms(5000);
  //
  // PORTB |= 0x02;
  // _delay_ms(5000);


  //read value with ADC
  /* Enable the ADC */
  ADCSRA |= 0xC0; //1100 0000
  while((ADCSRA & 0x40) != 0x00){
    //wait for ADC to finish
  }

  //get value
  value = ADCH;

  //PB1 low signal
  PORTB &= 0xFD;
  //return read value
  return value;
}
