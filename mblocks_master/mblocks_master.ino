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

char storage[256]; // This creates storage space for decoding serial messages
//SerialDecoderBuffer buf = {storage, 256, 0}; //Struct used to detect serial messages from Kyles Board

// Beginning Behavior
Behavior behavior = DEMO; // initial Behavior Cube implements

void setup() // Actually the main loop...
{
  int mainLoopCounter = 0;
  int timerCounter = millis(); // start time
  bool shutDown = false;
  initializeCube(); // Runs this code once to setup input/outputs, communication networks...
                    // (Wifi, i2c, serial) and instantiates classes and calibration values
  c.update(); // populates initial readings for variables such as which face is up, and # of neighbors
  c.updateCubeID(GlobalCubeID); // updated variable c.cubeID from initialization loop up table...
  c.superSpecialBlink(&purple, 100);
  c.superSpecialBlink(&red, 75);
  c.superSpecialBlink(&yellow, 55);
  c.superSpecialBlink(&teal, 50);
  c.superSpecialBlink(&blue, 40);
  c.superSpecialBlink(&white, 40);
  c.blinkRingAll();
  c.lightCube(&off);

   ///// BUNCH OF RANDOM TESTING ////
//  for(int i = 0; i < 10; i++)
//  {
//    c.updateSensors();
//    if(c.numberOfNeighbors() > 1)
//    {
//      Serial.print("c.whichFaceHasNeighbor(0) : ");Serial.println(c.whichFaceHasNeighbor(0));
//      Serial.print("c.whichFaceHasNeighbor(1) : ");Serial.println(c.whichFaceHasNeighbor(1));
//      if(c.areFacesOpposite(c.whichFaceHasNeighbor(0), c.whichFaceHasNeighbor(1)) == true)
//      {
//        c.blockingBlink(&green);
//      }
//      else
//      {
//        c.blockingBlink(&red);
//      }
//    }
//  }
//  
//  c.lightPlaneRing(PLANE0123);
//  delay(500);
//  c.lightPlaneRing(PLANE0425);
//  delay(500);
//  c.lightPlaneRing(PLANE1453);
//  delay(500);
//  Serial.println("ver");
//  if(c.anythingOnSerial(400) == true)
//  {
//    c.blockingBlink(&green, 10);
//  }
//  else
//  {
//    c.blockingBlink(&red, 10);
//  }
//  
//  Serial.println("vet");
//  if(c.anythingOnSerial(400) == false)
//  {
//    c.blockingBlink(&green, 10);
//  }
//  else
//  {
//    c.blockingBlink(&red, 10);
//  }
  
  ///// BUNCH OF RANDOM TESTING ////
  //if(c.cubeID != 12)
  //c.blinkOutMessageWholeCube(3, 2);
//  if(c.cubeID > 30)
//  {
//    for(int i = 0; i < 20; i++)
//    {
//      c.updateSensors();
//      delay(300);
//    }
//  }
//  else
//  {
//    for(int i = 0; i < 50; i ++)
//    {
//      c.faces[2].blinkRingDigit(3, 20);
//      c.faces[2].blinkRingDigit(2, 20);
//      c.faces[2].blinkRingDigit(1, 20);
//    }
//  }
  ///////////////////ACTUAL LOOP////////////////////
  while((millis() < c.shutDownTime) && (!shutDown))
  {
    mainLoopCounter++;
    behavior = checkForBehaviors(&c, behavior);
  }
  c.blockingBlink(&red);
  c.shutDown();
}
// This is here only becuase arduino won't compile without it, but it is never used, 
//the real loop is "while(1)" in the void setup() Function
void loop()
{
}
