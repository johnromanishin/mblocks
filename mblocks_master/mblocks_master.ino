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

// Global Objects
Cube c; // Initialize the Cube Object c globally so that things don't crash

char storage[512]; // This creates storage space for decoding serial messages
//SerialDecoderBuffer buf = {storage, 256, 0}; //Struct used to detect serial messages from Kyles Board

// Beginning Behavior
Behavior behavior = DEMO; // initial Behavior Cube implements

void setup() // Actually the main loop...
{
  int timerCounter = millis(); // start time
  bool shutDown = false;
  initializeCube(); // Runs this code once to setup input/outputs, communication networks...
                    // (Wifi, i2c, serial) and instantiates classes and calibration values
  c.update(); // populates initial readings for variables such as which face is up, and # of neighbors
  c.updateCubeID(GlobalCubeID); // updated variable c.cubeID from initialization loop up table...
  c.superSpecialBlink(&purple, 100);
  c.superSpecialBlink(&blue, 40);
  c.superSpecialBlink(&white, 40);
  c.blinkRingAll();
  c.lightCube(&off);
  ///////////////////ACTUAL LOOP////////////////////
//  while((millis() < c.shutDownTime) && (!shutDown))
//  {
//    mainLoopCounter++;
//    behavior = checkForBehaviors(&c, behavior);
//  }
  //c.blockingBlink(&red);
  //c.shutDown();
}
// This is here only becuase arduino won't compile without it, but it is never used, 
//the real loop is "while(1)" in the void setup() Function
void loop()
{
  behavior = checkForBehaviors(&c, behavior);
  if(millis() < c.shutDownTime)
  {
    c.blockingBlink(&red);
    c.shutDown();
  }
}
