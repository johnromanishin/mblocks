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
char storage[512]; // This creates storage space for decoding serial messages

void setup() // Actually the main loop...
{
  int mainLoopCounter = 0;
  int timerCounter = millis(); // start time
  bool shutDown = false;
  ///////////////////ACTUAL LOOP////////////////////
  while(millis() < 20*60*1000)
  {
    mainLoopCounter++;
  }
}
// This is here only becuase arduino won't compile without it, but it is never used, 
//the real loop is "while(1)" in the void setup() Function
void loop()
{
}
