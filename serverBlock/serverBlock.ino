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

#include "Interaction.h"
#include "Communication.h"
#include "Defines.h"
#include "Database.h"
#include <VL6180X.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////Setup////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  initializeWifiMesh();
  initializeRangeSensor();
  initializeDatabase();
  wifiDelay(500);
  Serial.print("\n WIFI ID: ");
  Serial.println(mesh.getNodeId());
  espconn_tcp_set_max_con(6); // this is supposed to increase the maximum number of WIFI connections to 6
  wifiDelay(1000);  
  Serial.println("Exiting Setup");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// Global Vriables ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int loopCounter = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// MAIN LOOP ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * The main loops goal is to:
 * 1. 
 */
void resetActiveCubes()
{
  for(int cube = 0; cube < NUM_CUBES; cube++)
  {
    database[cube][ACTIVE] = 0;
  }
}

void discoverActiveCubes(int waitTime)
{
  resetActiveCubes();
  String msg = "q";
  sendMessage(-1, msg);
  wifiDelay(waitTime);
  Serial.println("------------------------------");
  Serial.println("Ending discover Cubes, we found: ");
  for(int cube = 0; cube < NUM_CUBES; cube++)
  {
    if(database[cube][ACTIVE] == 1)
      {
        Serial.print("Cube: ");
        Serial.print(cube);
        Serial.println(" Reporting for Cuty");
      }
  }
  Serial.println("------------------------------");
}

void loop()
{
  updateBoxes(); // checks messages from the WiFi Message queue
  interactWithRangeSensor(); // checks the range value to send messages based on user input
  wifiDelay(5);
  if((loopCounter % 500) == 0)
  {
    for(int cube = 0; cube < NUM_CUBES; cube++)
    {
      if(database[cube][ACTIVE] == 1)
      {
        pushBlinkMessage(cube);
      }
    }
  }
  
  if((loopCounter % 9999) == 0)
  {
    //pushBlinkMessage(TESTCUBE_ID);
    
    //pushBlinkMessage(4);
    //pushBlinkMessage(5);
    //pushBlinkMessage(7);
    //pushBlinkMessage(8);
    //Serial.println("Added message to que");
  }
  if((loopCounter % 9399) == 0)
  {    
    //pushBlinkMessage(10);
    //pushBlinkMessage(13);
    
    //pushBlinkMessage(2);
    //pushBlinkMessage(1);
    //pushBlinkMessage(3);
    //pushBlinkMessage(9);
    
    //Serial.println("Added message to que");
  }
  if((loopCounter % 99999) == 0)
  {
    //pushBlinkMessage(TESTCUBE_ID);
    
    //pushBlinkMessage(6);
    //pushBlinkMessage(12);
    //pushBlinkMessage(16);
    //pushBlinkMessage(11);
    //Serial.println("Added message to que");
  }
  if((loopCounter % 50) == 0)
  {
    //pushBlinkMessage(TESTCUBE_ID);
    //Serial.println("Added message to que");
    testDatabase();
  }
  //  testThingNow();
  loopCounter++;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// OTHER ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void interactWithRangeSensor()
{
  int rangeValue = readRangeSensor();
  if (rangeValue < 20)
  {
    wifiDelay(300); // wait a bit to make sure it is a valid reading
    rangeValue = readRangeSensor();
    if(rangeValue < 20)
    {
      discoverActiveCubes(5000);
    }
  }
  
  else if (rangeValue > 20 && rangeValue < 50)
  {
//    for(int cube = 0; cube < NUM_CUBES; cube++)
//    {
//      if(database[cube][ACTIVE] == 1)
//      {
//        pushForwardMessage(cube);
//      }
//    }
  }

  else if (rangeValue > 50 && rangeValue < 100)
  {
    //sendBroadcastMessage(createJsonStringFlood(-1, "attractive"));
  }

//  else if (rangeValue > 100 && rangeValue < 200)
//  {
//    for(int cube = 0; cube < NUM_CUBES; cube++)
//    {
//      if(database[cube][ACTIVE] == 1)
//      {
//        pushForwardMessage(cube);
//      }
//    }
//    wifiDelay(1000);
//  }
}
