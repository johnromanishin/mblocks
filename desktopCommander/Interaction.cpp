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
  Serial.println("Attemting to read sensor: ");
  return(sensor.readRangeSingle());
}

