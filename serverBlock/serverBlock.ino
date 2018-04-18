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
//extern inboxEntry inbox;
//extern outboxEntry outbox[2];

void testOutboxes()
{
  pushBlinkMessage(TESTCUBE_ID);
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
    pushStatusMessage(TESTCUBE_ID);
    wifiDelay(300);
  }
}


void setup()
{
  Serial.begin(115200);
  initializeWifiMesh();
  initializeRangeSensor();
  wifiDelay(1000);
  Serial.print("\n WIFI ID: ");
  Serial.println(mesh.getNodeId());
  initializeOutboxes();
  espconn_tcp_set_max_con(6); // this is supposed to increase the maximum number of WIFI connections to 6
  wifiDelay(1000);

  testOutboxes();

}
bool lineOfThree = false;
int foundFlag;
void loop()
{
  updateBoxes(); // checks messages from the WiFi Message queue
  interactWithRangeSensor(); // checks the range value to send messages based on user input
  wifiDelay(5);

  foundFlag = 0;
  for (int face = 1; face < 5; face <<= 1)
  {
    if ( (cubesState[TESTCUBE_ID][face] != 0) && (cubesState[TESTCUBE_ID][oppositeFace(face)] != 0) )
    {
      foundFlag = face;
    }
  }

  if (foundFlag != 0)
  {
    if (lineOfThree == false)
    {
      lineOfThree = true;
      pushReverseMessage(TESTCUBE_ID);
      pushReverseMessage(cubesState[TESTCUBE_ID][foundFlag]/100);
      pushReverseMessage(cubesState[TESTCUBE_ID][oppositeFace(foundFlag)]/100);

    }
  }
  else if (lineOfThree == true)
  {
    lineOfThree = false;
    pushBlinkMessage(-1);
  }

}
