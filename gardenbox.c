/*************************************************************
* Control C file to call all sensors and LCD display
*
*
**************************************************************/
#include "moisture.c"
#include "lcd.h"
#include "lcd.c"
#include "rotaryencoder.c"

int main(){

  //set all output input here or in each .c file?

  //call initialization of rotary encoder which calls LCD
  mainRE();
  return 0;
}
