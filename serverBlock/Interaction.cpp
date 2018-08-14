//human interaction on the front end of the server block

#include "Interaction.h" 
#include <Arduino.h>   
#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal
#include <VL6180X.h>

/*
 * This creates a globally acessable class called "sensor"
 * methods of the class, defined in the library #include <VL6180X.h>
 * Can be called using sensor.[function]();
 */
VL6180X sensor;


/*
 * Initialize the sensor - this calls the code in the library
 */
int initializeRangeSensor()
{
  /*
   * Wire.begin initializes the i2c bus, called Wire in arduino land
   */
  Wire.begin(2,14);
  sensor.init();
  sensor.configureDefault();
  sensor.setTimeout(500);
}

/*
 * This function takes a reading from the sensor
 * and returns the value as an integer
 * The value is in mm, and ranges from 10 to 250 mm.
 */
int readRangeSensor()
{
  int value = sensor.readRangeSingle();
  //Serial.print("Attemting to read sensor: ");
  //Serial.println(value);
  return(value);
}

