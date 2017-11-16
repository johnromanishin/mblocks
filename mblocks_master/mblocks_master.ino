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

char storage[256]; // This creates storage space for decoding serial messages
SerialDecoderBuffer buf = {storage, 256, 0}; //Struct used to detect serial messages from Kyles Board

// Beginning Behavior
Behavior behavior = TEST_TESTING_THANGS; // initial Behavior Cube implements

void setup() // Actually the main loop...
{
  int loopCounter = 0;
  long timerCounter = millis(); // start time
  bool shutDown = false;
  initializeCube(); // Runs this code once to setup input/outputs, communication networks...
                    // (Wifi, i2c, serial) and instantiates classes and calibration values
  c.updateSensors(); // populates initial readings for variables such as which face is up, and # of neighbors
  if(c.numberOfNeighbors(0,0) > 1) // turns off if the cube has at least 2 neighbors... for ease of dealing with things
  {
    bool shutDown = true;
  }
  ///////////////////////ACTUAL LOOP////////////////////
  while((millis() < c.shutDownTime) && (!shutDown))
  {
    loopCounter++;
    if      (behavior == SOLO_LIGHT_TRACK)
      behavior = soloSeekLight(&c, &buf);
    else if (behavior == DUO_LIGHT_TRACK)
      behavior = duoSeekLight();
    else if (behavior == FOLLOW_ARROWS)
      behavior = followArrows();
    else if (behavior == TEST_TESTING_THANGS)
      behavior = testTestingThangs(&c, &buf);
    else if (behavior == CHILLING)
      behavior = chilling(&c, &buf);
    else if (behavior == ATTRACTIVE)
      behavior = attractive(&c);
    else if (behavior == RELAY_SLEEP)
      behavior = relaySleepMessage(&c);
    else if(behavior == SHUT_DOWN)
      shutDown = true;
    else
    {
      //Serial.println("ERROR: unknown behavior.  Reverting to \"CHILLING\"");
      behavior = CHILLING;
    }
  }
  c.blockingBlink(1,0,0);
  c.shutDown();
}

// This is here only becuase arduino won't compile without it, but it is never used, 
//the real loop is "while(1)" in the void setup() Function
void loop()
{
}
