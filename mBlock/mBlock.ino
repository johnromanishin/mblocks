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
#include "Defines.h"              // includes defines, helper functions, some global variables
#include "Initialization.h"       // Includes cube specific Parameters
#include "Cube.h"                 // Code relating to low level cube hardware
#include "Face.h"                 // Code related to low leval hardware for each face
#include "CBuff.h"                // Circular buffer library
#include "Communication.h"        // Includes the wifi mesh library
#include "Behavior.h"             // Includes the state machine code

// Global Objects
Cube c; // Initialize the Cube Object c globally so that things don't crash

// Beginning Behavior
Behavior behavior = DEMO; // initial Behavior Cube implements

void setup() // starts up the various electronic hardware...
{
  bool shutDown = false;
  initializeCube(); // Runs this code once to setup input/outputs, communication networks...
                    // (Wifi, i2c, serial) and instantiates classes and calibration values
  c.update(); // populates initial readings for variables such as which face is up, and # of neighbors
  c.updateCubeID(thisCubeID, mesh.getNodeId()); // updated variable c.cubeID from initialization loop up table...
  c.findPlaneStatus(true); // Populate the initial reading for the current plane status

  c.superSpecialBlink(&white, 100);
  c.flashFaceLEDs();
  c.lightCube(&off);
  wifiDelay(500);
  
  Serial.print("WIFI ID: ");
  Serial.println(mesh.getNodeId());
  sendAck(SPECIAL_MID);
  wifiDelay(400);
  sendAck(SPECIAL_MID);
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

