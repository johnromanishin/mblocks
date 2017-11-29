/**
    This includes the main code for the ESP8266 board, aka the "Master Board"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/

// External Libraries
#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal
#include <painlessMesh.h>         // Wifi Mesh Library found on the internet  
#include <Arduino.h>              // library with basic arduino commands

// Header Files
#include "Defines.h"
#include "Initialization.h"       // Includes .h files for each of the "tabs" in arduino
#include "Cube.h"                 // Includes .h files for each of the "tabs" in arduino
#include "Face.h"                 // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"                // Includes .h files for each of the "tabs" in arduino
#include "Communication.h"        // Includes wifi
#include "Behavior.h"
#include "SerialDecoder.h"

// Global Objects
Cube c; // Initialize the Cube Object c globally so that things don't crash

char storage[256]; // This creates storage space for decoding serial messages
SerialDecoderBuffer buf = {storage, 256, 0}; //Struct used to detect serial messages from Kyles Board

// Beginning Behavior
Behavior behavior = CHILLING; // initial Behavior Cube implements

void setup() // Actually the main loop...
{
  int loopCounter = 0;
  long timerCounter = millis(); // start time
  bool shutDown = false;
  initializeCube(); // Runs this code once to setup input/outputs, communication networks...
                    // (Wifi, i2c, serial) and instantiates classes and calibration values
  c.updateSensors(); // populates initial readings for variables such as which face is up, and # of neighbors
//  lightRainbow();
  if(c.numberOfNeighbors(0,0) > 1) // turns off if the cube has at least 2 neighbors... for ease of dealing with things
  {
    bool shutDown = true;
  }
  ///////////////////////ACTUAL LOOP////////////////////
  while((millis() < c.shutDownTime) && (!shutDown))
  {
    loopCounter++;
    behavior = checkForBehaviors(&c, &buf, behavior);
  }
  c.blockingBlink(&red);
  c.shutDown();
}
//void lightRainbow(int delayTime)
//{
//  c.lightCube2(&purple);
//  delay(delayTime);
//  c.lightCube2(&red);
//  delay(delayTime);
//  c.lightCube2(&yellow);
//  delay(delayTime);
//  c.lightCube2(&green);
//  delay(delayTime);
//  c.lightCube2(&teal);
//  delay(delayTime);
//  c.lightCube2(&blue);
//  delay(delayTime);
//  c.lightCube2(&white);
//  delay(delayTime);
//  c.lightCube2(&off);
//  delay(delayTime);
//}
// This is here only becuase arduino won't compile without it, but it is never used, 
//the real loop is "while(1)" in the void setup() Function
void loop()
{
}

//Behavior checkForBehaviors(Behavior behavior)
//{
//  if (behavior == SOLO_LIGHT_TRACK)
//    behavior = soloSeekLight(&c, &buf);
//  else if (behavior == DUO_LIGHT_TRACK)
//    behavior = duoSeekLight();
//  else if (behavior == FOLLOW_ARROWS)
//    behavior = followArrows();
//  else if (behavior == TEST_TESTING_THANGS)
//    behavior = testTestingThangs(&c, &buf);
//  else if (behavior == CHILLING)
//    behavior = chilling(&c, &buf);
//  else if (behavior == ATTRACTIVE)
//    behavior = attractive(&c);
//  else if (behavior == SLEEP)
//    behavior = sleep();
//  else if (behavior == YELLOW)
//    behavior = Yellow(&c, &buf);
//  else if (behavior == PURPLE)
//    behavior = Purple(&c, &buf);
//  else if (behavior == TEAL)
//    behavior = Teal(&c, &buf);
//  else
//  {
//    //Serial.println("ERROR: unknown behavior.  Reverting to \"CHILLING\"");
//    behavior = CHILLING;
//  }
//  return(behavior);
//}

