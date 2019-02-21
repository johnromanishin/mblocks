/**
    This includes the main code for the ESP8266 board, aka the "Master Board"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/

/*
 * There are several ways that the "server" communicates with the M-Block robotic modules,
 * One way is through using WIFI based on the ESP8266 modues, but the network is not that
 * stable with so many nodes in a small area, so the backup method of communicating is to use a global
 * light source to blink on and off. The TOP_LIGHT_THRESHOLD is the level we think of as a "1" on the top face
 */
#define TOP_LIGHT_THRESHOLD 700

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

/* -- Global Objects --
 * Initialize the Cube Object c globally so that things don't crash
 */
Cube c;

// Beginning Behavior
/*
 * There are two levels of the State Machine running the modules
 * 1. the "GAME" - this is the high level goal of the cubes
 *    a. Turn into a line
 *    b. Turn into a specific Shape
 *    c. Follow Light and Build random shape
 *    d. Follow Arrow Path...
 *    
 * 2. Behavior - lower level behavior
 * 
 * We set the initial behavior right here...
 */
Behavior behavior = CHILLING;

void setup() // starts up the various electronic hardware...
{
  bool shutDown = false;
  c.moveSuccessBuffer.push(true);
  c.moveSuccessBuffer.push(true);
  delay(500);
  
  /*
   * InitializeCube sets up:
   *    1. Calibration Values for each cube
   *    2. Hardware - WIFI, I2C, Serial
   *    3. Gives the cube its unique ID
   */
  initializeCube(); 

  /*
   * populates initial readings for variables such as which face is up, and # of neighbors
   * Checks the ID number for itself
   * checks the current plane status
   */
  
  c.updateCubeID(thisCubeID, mesh.getNodeId()); // updated variable c.cubeID from initialization loop up table...
  c.findPlaneStatus(true); // Populate the initial reading for the current plane status

  for(int updateTimes = 0; updateTimes < 2; updateTimes++)
  {
    c.update();
    wifiDelay(200);
  }
  
  startUpLightSequence();
  Serial.print("WIFI ID: ");
  Serial.println(mesh.getNodeId());

  /*
   * This part isn't finished yet, but it will help figure out which 
   * "Game" to start playing
   * 
   */
     /*
   * The "Game" is the overarching program that the cube runs, at startup we define it here
   * It is just a global string that can be defined anywhere...
   * IT is set in defines.cpp
  */
  Game = "PATH";
  //Game = "GRID";
  bool DetermineGame = false;
  if(DetermineGame)
  {
    Game = "LIGHT";
    
    if((TOP_FACE_LIGHT[0] > TOP_LIGHT_THRESHOLD) && 
                (c.numberOfNeighbors(0) == 0) && 
                (c.numberOfNeighbors(1) == 0))
    {
      //Game = "LIGHT";
      
      c.blockingBlink(&yellow);
      for(int i = 0; i < 3; i++);
      {
        c.moveOnLattice(&horizontal_F);
      }
      //
      Game = "LINE";
    }
    
    else if(TOP_FACE_LIGHT[0] > TOP_LIGHT_THRESHOLD)
    {
      Game = "LINE";
    }
  }
  /*
   * Try to let the server know that we exist...
   */
  for (int i = 0; i < 2; i++)
  {
    sendAck(SPECIAL_MID);
    wifiDelay(600);
  }
}

/*  
 *   Main Loop...
 *   Just continually loops, most of the action happens in checkForBehaviors...
 */
void loop()
{
//  while(1)
//  {
//    int sample = random(5);
//    Serial.println(sample);
//    delay(200);
//  }
//  while(1)
//  {
//    c.faces[0].turnOnFaceLEDs(1, 1, 1, 1);
//    wifiDelay(500);
//    c.faces[0].turnOffFaceLEDs();
//    wifiDelay(500);
//  }

  
  behavior = checkForBehaviors(&c, behavior);
  mesh.update();
  if (millis() > c.shutDownTime)
  {
    c.blockingBlink(&red);
    c.shutDown();
  }
}

void startUpLightSequence()
{
  c.superSpecialBlink(&purple, 100);
  c.flashFaceLEDs();
  c.lightCube(&off);
  wifiDelay(500);
}
