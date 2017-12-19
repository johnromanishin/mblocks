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
Behavior behavior = PRE_SOLO_LIGHT; // initial Behavior Cube implements

void setup() // Actually the main loop...
{
  int mainLoopCounter = 0;
  int timerCounter = millis(); // start time
  bool shutDown = false;
  initializeCube(); // Runs this code once to setup input/outputs, communication networks...
                    // (Wifi, i2c, serial) and instantiates classes and calibration values
  c.updateSensors(); // populates initial readings for variables such as which face is up, and # of neighbors
  c.updateCubeID(GlobalCubeID); // updated variable c.cubeID from initialization loop up table...
  c.superSpecialBlink(&purple, 100);
  c.superSpecialBlink(&red, 75);
  c.superSpecialBlink(&yellow, 55);
  c.superSpecialBlink(&teal, 50);
  c.superSpecialBlink(&blue, 40);
  c.superSpecialBlink(&white, 40);
  c.blinkRingAll();
  //if(c.cubeID != 12)
  //c.blinkOutMessageWholeCube(3, 2);
  c.lightCube(&off);
//  for(int i = 0; i < 10; i++)
//  {
//    c.debugAccelerometers();
//    delay(500);
//  }
  ///////////////////ACTUAL LOOP////////////////////
  while((millis() < c.shutDownTime) && (!shutDown))
  {
    mainLoopCounter++;
    behavior = checkForBehaviors(&c, &buf, behavior);
  }
  c.blockingBlink(&red);
  c.shutDown();
}
// This is here only becuase arduino won't compile without it, but it is never used, 
//the real loop is "while(1)" in the void setup() Function
void loop()
{
}
