#include<avr/io.h>
#include<util/delay.h>
int main()
{
  //output DDRB port 5
  DDRB = 0x02;
  PORTB = 0x00; //initialize to off

  // Repeat forever
  while(1){
    // led on
    PORTB = 0x02;
    _delay_ms(5000);
    //led off
    PORTB = (0x00);
    _delay_ms(5000);
  }
  // Never reached
  return 0;
}
