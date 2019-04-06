#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "moisture.h"

int readsoil(int val){
  //turn VDD pin on to a HIGH
  //set pin to HIGH
  _delay_ms(10); //wait 10 ms
  // val = analogRead(input pin of the soil sensor DDRC2);
  //turn VDD pin to a low
  //set pin to LOW
  return val;
}; 

int moistureReader()
{
  int val = 0; //value for storing moisture value

  //set input from soil
  DDRC |= (1 << PC2);
  //set output for VDD
  //turn on and off this pinout for VDD
  //for the moisture sensor
  // DDRB[1] 0x01;
  // //set output to relay
  // DDRB[1] = 0x01;

  while(1){
    val = readsoil(val);
    _delay_ms(1000); //1 sec delay to get value of the soil sensor
  }


  return 0;
}

/*
code for arduino IDE:

int val = 0; //value for storing moisture value
int soilPin = A0;//Declare a variable for the soil moisture sensor
int soilPower = 7;//Variable for Soil moisture Power

//Rather than powering the sensor through the 3.3V or 5V pins,
//we'll use a digital pin to power the sensor. This will
//prevent corrosion of the sensor as it sits in the soil.

void setup()
{
  Serial.begin(9600);   // open serial over USB

  pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT
  digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensor
}

void loop()
{
Serial.print("Soil Moisture = ");
//get soil moisture value from the function below and print it
Serial.println(readSoil());

//This 1 second timefrme is used so you can test the sensor and see it change in real-time.
//For in-plant applications, you will want to take readings much less frequently.
delay(1000);//take a reading every second
}
//This is a function used to get the soil moisture content
int readSoil()
{

    digitalWrite(soilPower, HIGH);//turn D7 "On"
    delay(10);//wait 10 milliseconds
    val = analogRead(soilPin);//Read the SIG value form sensor
    digitalWrite(soilPower, LOW);//turn D7 "Off"
    return val;//send current moisture value
}
*/
