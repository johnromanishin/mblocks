/**
    This includes the main code for the ESP8266 board, aka the "master"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/

#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal - used to communicate with all of the sensors on the Mblocks
//#include <ESP8266WiFiMesh.h>
#include <painlessMesh.h>         // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoHardware.h>      // Unsure of what this does, but it seemed like a good idea to keep it in here...
#include "initialization.h"       // Includes .h files for each of the "tabs" in arduino
#include "classDefinitions.h"     // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"                // Includes .h files for each of the "tabs" in arduino

void setup() 
{
  initializeCube(); // Runs code once to 

  Face face1;

  while(1)
  {
    face1.updateAmbient();
    delay(500);
    Serial.println(face1.returnAmbientValue(0));
    //Serial.println(face1.returnAmbientValue(0));
    //stepTowardLight();
  }
}


// This is here only becuase arduino won't compile without it, but it is never used, the real loop is "while(1)" in the void setup() Function
void loop() 
{  
}
