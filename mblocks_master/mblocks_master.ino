/**
    This includes the main code for the ESP8266 board, aka the "master"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/

#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal - used to communicate with all of the sensors on the Mblocks
#include <painlessMesh.h>
#include <Arduino.h>
#include "Initialization.h"       // Includes .h files for each of the "tabs" in arduino
#include "Cube.h"     // Includes .h files for each of the "tabs" in arduino
#include "Face.h"     // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"                // Includes .h files for each of the "tabs" in arduino
#include "Communication.h"        // Includes wifi 
#include "Behavior.h"

//WiFi::BSSIDstr(i);

String behavior = "soloSeekLight";
Cube c; // Initialize the Cube Object c // globally so that things don't crash

void setup() // Actually the main loop...
{
  long timer_counter = 0;
  initializeCube(); // Runs this code once to setup input/outputs, communication networks... 
                    // (Wifi, i2c, serial) and instantiates classes and calibration values
  Serial.println(ESP.getChipId());
  Serial.println("Starting Main Loop");
  
  c.updateCoreMagnetSensor();
  int initialMagnetReadingOffset = c.coreMagnetAngleBuffer.access(0);

  Serial.print("Face Version: ");Serial.println(faceVersion);
  
  while(millis() < c.shutDownTime && !(c.numberOfNeighbors(0,0)))
  {
      mesh.update();
  }
}

// This is here only becuase arduino won't compile without it, but it is never used, the real loop is "while(1)" in the void setup() Function
void loop() 
{  
}
/////////// Global Variables
int faceVersion = 1;
int cubeID = 0;
int planeChangeTime = 60;
int planeChangeRPM = 5000;
int traverseBrakeCurrent_F = 2800;
int traverseBrakeCurrent_R = 2800;
int cornerClimbBrakeCurrent_F = 3000;
int cornerClimbBrakeCurrent_R = 3000;
int plane0321Magnet = 0;
int plane0425Magnet = 120;
int plane1435Magnet = 240;
