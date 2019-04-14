#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "moisture.h"

int readMoisture(){

  int value = 0;

  //PB1 high signal
  PORTB |= 0x02;
  //wait 10 milliseconds
  _delay_ms(1000);

  //read value with ADC TODO!!

  //PB1 low signal
  PORTB &= 0xFD;
  //return read value
  return value;
}
