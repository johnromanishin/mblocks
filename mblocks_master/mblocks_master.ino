/**
    This includes the main code for the ESP8266 board, aka the "Master Board"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/

// External Libraries
#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal
#include <painlessMesh.h>
#include <Arduino.h>

// Header Files
#include "Defines.h"
#include "Initialization.h"       // Includes .h files for each of the "tabs" in arduino
#include "Cube.h"                 // Includes .h files for each of the "tabs" in arduino
#include "Face.h"                 // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"                // Includes .h files for each of the "tabs" in arduino
#include "Communication.h"        // Includes wifi
#include "Behavior.h"
#include "SerialDecoder.h"
#include "MagTag.h"

// Global Objects
Cube c; // Initialize the Cube Object c globally so that things don't crash
// This is a temporary location where the contents of WIFI messages are stores
                 // will be replaced soon with a fancy string buffer thing

char storage[256];
SerialDecoderBuffer buf = {storage, 256, 0}; //Struct used to detect serial messages from Kyles Board

// Beginning Behavior
Behavior behavior = CHILLING; // initial Behavior Cube impliments

void setup() // Actually the main loop...
{
  int loopCounter = 0;
  long timerCounter = millis(); // start time
  bool shutDown = false;
  initializeCube(); // Runs this code once to setup input/outputs, communication networks...
                    // (Wifi, i2c, serial) and instantiates classes and calibration values
  c.updateSensors(); // populates initial readings for variables such as which face is up, and # of neighbors
  ///////////////////////ACTUAL LOOP////////////////////
  while((millis() < c.shutDownTime) && (!shutDown))
  {
    if      (behavior == SOLO_LIGHT_TRACK)
      behavior = soloSeekLight(&c, &buf);
    else if (behavior == DUO_LIGHT_TRACK)
      behavior = duoSeekLight();
    else if (behavior == FOLLOW_ARROWS)
      behavior = followArrows();
    else if (behavior == TEST_TESTING_THANGS)
      behavior = testTestingThangs(&c, &buf);
    else if (behavior == CHILLING)
      behavior = chilling(&c);
    else if (behavior == ATTRACTIVE)
      behavior = attractive(&c);
    else if(behavior == SHUT_DOWN)
      shutDown = true;
  }

  //
  
  c.blockingBlink(0,0,1);
  c.shutDown();
}

// This is here only becuase arduino won't compile without it, but it is never used, the real loop is "while(1)" in the void setup() Function
void loop()
{
}
