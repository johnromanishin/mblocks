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

// Beginning Behavior
Behavior behavior = DEMO; // initial Behavior Cube implements

void setup() // starts up the various electronic hardware...
{
  int timerCounter = millis(); // records when everything started
  bool shutDown = false;
  initializeCube(); // Runs this code once to setup input/outputs, communication networks...
                    // (Wifi, i2c, serial) and instantiates classes and calibration values
  c.update(); // populates initial readings for variables such as which face is up, and # of neighbors
  c.updateCubeID(GlobalCubeID, mesh.getNodeId()); // updated variable c.cubeID from initialization loop up table...
  c.superSpecialBlink(&purple, 100);
  c.superSpecialBlink(&blue, 100);
  c.superSpecialBlink(&white, 100);
  c.blinkRingAll();
  c.lightCube(&off);
}

void loop() // Main Loop... Just continually loops, most of the action happens in checkForBehaviors...
{
  if(MAGIC_DEBUG) Serial.println("Running Main loop Once...");
  behavior = checkForBehaviors(&c, behavior);
  mesh.update();
  if(millis() > c.shutDownTime)
  {
    c.blockingBlink(&red);
    c.shutDown();
  }
}
