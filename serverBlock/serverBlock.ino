/*
    This includes the main code for the ESP8266 board, aka the "Master Board"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/

#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal
#include <painlessMesh.h>         // Wifi Mesh Library found on the internet  
#include <ArduinoJson.h>
#include <Arduino.h>              // library with basic arduino commands

#include "Interaction.h"
#include "Communication.h"
#include "Defines.h"
#include "Database.h"             // This tab deals with processing information about the cubes
#include <VL6180X.h>              // This includes the code for the VL6180 

bool LIGHT_TOGGLE = false;
void setup()
/*
 * This runs in the beginning, and initialized a few things, and the passes control to the
 * loop function below
 */
{
  Serial.begin(115200);
  initializeWifiMesh();
  initializeRangeSensor();
  initializeDatabase();
  wifiDelay(500);
  Serial.print("\n WIFI ID: ");
  Serial.println(mesh.getNodeId());
  pinMode(Switch, OUTPUT);
  espconn_tcp_set_max_con(6); // this is supposed to increase the maximum number of WIFI connections to 6
  Serial.println("Exiting Setup");
  wifiDelay(1000);
//  for(int i = 0; i < 2; i++)
//  {
//    digitalWrite(Switch, LOW);
//    wifiDelay(1000);
//    digitalWrite(Switch, HIGH);
//    wifiDelay(1000);

//  }
  digitalWrite(Switch, LOW);
  wifiDelay(1000);
}

/*
 * -----------------------------------------------------------------------------------------------------------
 * ----------------------------------- Global Variables ------------------------------------------------------
 * -----------------------------------------------------------------------------------------------------------
 */
 
int loopCounter = 0;

void resetActiveCubes()
/*
 * This function 
 */
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
        Serial.println(" Reporting for Duty");
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
    Serial.println("Sending message to active cubes...");
    for(int cube = 0; cube < NUM_CUBES; cube++)
    {
      if(database[cube][ACTIVE] == 1)
      {
        pushBlinkMessage(cube);
      }
    }
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
  
  if((loopCounter % 57) == 0)
  {
    //pushBlinkMessage(TESTCUBE_ID);
    //Serial.println("Added message to que");
    testDatabase();
  }
  //  testThingNow();
  loopCounter++;
  if(CONFIRMED && millis() > 30000)
  {
    wifiDelay(4000);
    
    Serial.println("TURNING ON THE LIGHTS");
    digitalWrite(Switch, HIGH);
    CONFIRMED = false;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// OTHER ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void interactWithRangeSensor()
{
  int rangeValue = readRangeSensor();
  if (rangeValue < 20)
  {
//    wifiDelay(300); // wait a bit to make sure it is a valid reading
//    rangeValue = readRangeSensor();

//    if(rangeValue < 20)
//    {
//      discoverActiveCubes(5000);
//    }
  }
  
  else if (rangeValue > 20 && rangeValue < 50)
  {
    if(LIGHT_TOGGLE)
    {
      digitalWrite(Switch, LOW);
      LIGHT_TOGGLE = false;
    }
    else
    {
      digitalWrite(Switch, HIGH);
      LIGHT_TOGGLE = true;
    }
    wifiDelay(500);
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
