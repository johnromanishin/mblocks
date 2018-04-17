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
#include <VL6180X.h>

// Header Files
char storage[512]; // This creates storage space for decoding serial messages
//extern inboxEntry inbox;
//extern outboxEntry outbox[2];

void setup()
{
  Serial.begin(115200);
  initializeWifiMesh();
  initializeRangeSensor();
  wifiDelay(2000);
  Serial.print("\n WIFI ID: ");
  Serial.println(mesh.getNodeId());
  initializeOutboxes();
  espconn_tcp_set_max_con(6); // this is supposed to increase the maximum number of WIFI connections to 6
  
  testOutboxes();
  
}
bool lineOfThree = false;

void loop()
{
  updateBoxes(); // checks messages from the WiFi Message queue
  interactWithRangeSensor(); // checks the range value to send messages based on user input
  wifiDelay(5);
  for (int face=1; face<5; face<<1)
  {
    if ( (cubesState[face] != 0) && (cubesState[oppositeFace(face)] != 0) )
      if (lineOfThree == false)
      {
        lineOfThree = true;
        pushReverseMessage(TESTCUBE_ID);
      }
    else if (lineOfThree == true)
    {
      lineOfThree = false;
      pushForwardMessage(TESTCUBE_ID);
    }
  }
}

void interactWithRangeSensor()
{
  int rangeValue = readRangeSensor();
  if (rangeValue < 20)
  {
    //sendBroadcastMessage(createJsonStringFlood(-1, "sleep"));
    Serial.println("Putting the cubes to sleep...");
  }
  else if (rangeValue > 20 && rangeValue < 50)
  {
    //sendBroadcastMessage(createJsonStringFlood(-1, "lightSeek"));
  }
  
  else if (rangeValue > 50 && rangeValue < 100)
  {
    //sendBroadcastMessage(createJsonStringFlood(-1, "attractive"));
  }
  
  else if (rangeValue > 100 && rangeValue < 200)
  {
    pushReverseMessage(TESTCUBE_ID);
    pushForwardMessage(TESTCUBE_ID);
    pushBlinkMessage(TESTCUBE_ID);
    wifiDelay(100);
  }
}

void testOutboxes() 
{
  pushReverseMessage(TESTCUBE_ID);
  pushForwardMessage(TESTCUBE_ID);
  pushBlinkMessage(TESTCUBE_ID);
  pushReverseMessage(TESTCUBE_ID);
  pushForwardMessage(TESTCUBE_ID);
  pushBlinkMessage(TESTCUBE_ID);
  pushReverseMessage(TESTCUBE_ID);
  pushForwardMessage(TESTCUBE_ID);
  pushBlinkMessage(TESTCUBE_ID);
}

/////////////////
// cubesState[6];
// bool areFacesOpposite(int face1, int face2); // defined in defines.cpp
// int oppositeFace(int face);                  // defined in defines.cpp

void checkIfLine()
{
  int numberOfNeighbors = 0;
  int firstNeighborFace = -1;
  int secondNeighborFace = -1;
  for(int i = 0; i < 6; i ++)
  {
    if(cubesState[i] > 0)
    {
      if(numberOfNeighbors > 1)
        SecondNeighborFace = i;
      else
        firstNeighbor = i;
      
      if(numberOfNeighbors > 0)
        firstNeighbor = 
        
      numberOfNeighbors++;
    }
  }
  if(numberOfNeighbors)
}