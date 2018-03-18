/**
    This includes the main code for the ESP8266 board, aka the "Master Board"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/
// External Libraries
#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal
#include <painlessMesh.h>         // Wifi Mesh Library found on the internet  
#include <ArduinoJson.h>
#include <Arduino.h>              // library with basic arduino commands

#include "Communication.h"
#include "Defines.h"

// Header Files
char storage[512]; // This creates storage space for decoding serial messages
//
// int getCubeIDFromEsp(int espID)
// int getEspIDFromCube(int GlobalCubeID)
void setup() // Actually the main loop...
{
  Serial.begin(115200);
  initializeWifiMesh();
  Serial.println("Hey, just getting started");
  Serial.print("ESP Chip ID: ");
  Serial.println(ESP.getChipId());
  Serial.print("WIFI ID: ");
  Serial.println(mesh.getNodeId());
  int mainLoopCounter = 0;
  espconn_tcp_set_max_con(6);
  ///////////////////ACTUAL LOOP////////////////////
  Serial.println("Beginning actual loop");
  while(millis() < 20*60*1000)
  {
    if(mesh.isConnected(2139793359))
    {
      Serial.println("BIG BAD BREADBOARD CUBE IS UP AND RUNNING!!");
    }
    messageSingle(2139793359);
    //makeThemBlink(99);
    countdown(mainLoopCounter, 99);
    wifiDelay(1000);
    mainLoopCounter++;
    if(mainLoopCounter == 7)
    {
      mainLoopCounter = 0;
    }
  }
}
// This is here only becuase arduino won't compile without it, but it is never used, 
//the real loop is "while(1)" in the void setup() Function
void loop()
{
}
