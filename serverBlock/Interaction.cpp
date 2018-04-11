//human interaction on the front end of the server block

#include "Interaction.h" 
#include <Arduino.h>   
#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal
#include <VL6180X.h>

VL6180X sensor;

int initializeRangeSensor()
{
  Wire.begin(2,14);
  sensor.init();
  sensor.configureDefault();
  sensor.setTimeout(500);
}

int readRangeSensor()
{
  int value = sensor.readRangeSingle();
  //Serial.print("Attemting to read sensor: ");
  //Serial.println(value);
  return(value);
}

