/**
    This includes the main code for the ESP8266 board, aka the "Master Board"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/

#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal - used to communicate with all of the sensors on the Mblocks
#include <painlessMesh.h>
#include <Arduino.h>
#include "Defines.h"
#include "Initialization.h"       // Includes .h files for each of the "tabs" in arduino
#include "Cube.h"                 // Includes .h files for each of the "tabs" in arduino
#include "Face.h"                 // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"                // Includes .h files for each of the "tabs" in arduino
#include "Communication.h"        // Includes wifi 
#include "Behavior.h"
#include "SerialDecoder.h"

//WiFi::BSSIDstr(i);

Behavior behavior = SOLO_LIGHT_TRACK; // initial Behavior Cube impliments
Cube c; // Initialize the Cube Object c // globally so that things don't crash
char storage[256];    
SerialDecoderBuffer buf = {storage, 256, 0}; //Struct used to detect serial messages from Kyles Board 

void setup() // Actually the main loop...
{
  int loopCounter = 0;
  long timerCounter = 0; // start time
  initializeCube(); // Runs this code once to setup input/outputs, communication networks... 
                    // (Wifi, i2c, serial) and instantiates classes and calibration values
  c.updateSensors(); // populates initial readings for variables such as which face is up, and # of neighbors  
  if(c.numberOfNeighbors() > 0)
  behavior = TEST_TESTING_THANGS;                  
  //Serial.println(ESP.getChipId());
  Serial.println("Starting Main Loop"); 
  ///////////////////////ACTUAL LOOP////////////////////
  while(millis() < c.shutDownTime)
  { 
    if(behavior == SOLO_LIGHT_TRACK)    
      behavior = soloSeekLight(&c, &buf);
    else if(behavior == DUO_LIGHT_TRACK)
      behavior = duoSeekLight();
    else if (behavior == FOLLOW_ARROWS)
      behavior = followArrows(); 
    else if (behavior == TEST_TESTING_THANGS)
      behavior = testTestingThangs(&buf);
        
    if(millis() > timerCounter)
    {
      //face1.updateAmbient();
      //Serial.println(face1.returnAmbientValue(0));
      String msg_new = "";
      msg_new = ("HEY BRO");// + String(msg_id); //+ " Sensor Magnitude: " +String(read_5048_agc(address4)) + "   Angle: " + String(read_5048_angle(address4)/45.5) + "  ");
      timerCounter += 5000;
      mesh.sendBroadcast(msg_new);
    }
    mesh.update();
  }
  c.blockingBlink(0,0,1,30,200);
  c.shutDown();
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
