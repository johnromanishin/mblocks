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
#include "COMMAND.h"

// Header Files
char storage[512]; // This creates storage space for decoding serial messages
//
// int getCubeIDFromEsp(int espID)
// int getEspIDFromCube(int GlobalCubeID)
int mainLoopCounter = 0;

void setup() // Actually the main loop...
{
  Serial.begin(115200);
  initializeWifiMesh();
  initializeRangeSensor();
  Serial.println("Hey, just getting started");
  Serial.print("WIFI ID: ");
  Serial.println(mesh.getNodeId());
  espconn_tcp_set_max_con(6);
  ///////////////////ACTUAL LOOP////////////////////
  Serial.println("Beginning actual loop");
}
// This is here only becuase arduino won't compile without it, but it is never used,
//the real loop is "while(1)" in the void setup() Function
void loop()
{
  sendMessage(14, "hey Bro");

  int rangeValue = readRangeSensor();
  
  if (rangeValue < 20)
  {
    //sendBroadcastMessage(createJsonStringFlood(-1, "sleep"));
    Serial.println("Putting the cubes to sleep...");
  }
  else if (rangeValue > 20 && rangeValue < 50)
  {
    //sendBroadcastMessage(createJsonStringFlood(-1, "lightSeek"));
    //Serial.println("Putting the cubes to sleep...");
  }
  
  else if (rangeValue > 50 && rangeValue < 100)
  {
    //sendBroadcastMessage(createJsonStringFlood(-1, "attractive"));
    //Serial.println("Putting the cubes to sleep...");
  }
  
  else if (rangeValue > 100 && rangeValue < 200)
  {
    //sendBroadcastMessage(createJsonStringFlood(-1, "blink"));
    //Serial.println("Putting the cubes to sleep...");
  }
  wifiDelay(100);
  mainLoopCounter++;
  if (mainLoopCounter == 7)
  {
    mainLoopCounter = 0;
  }
}


//checkForBasicWifiCommands()
//{
//  int attempts = 5;
//  while (!jsonCircularBuffer.empty() && attempts > 0) // while there are still messages, and we haven't tried 5 times
//  {
//    StaticJsonBuffer<700> jb; // Create a buffer to store our Jason Objects...
//    JsonObject& root = jb.parseObject(jsonCircularBuffer.pop());
//    if (root["targetID"] == 69)       // or if message is brodcast
//    {
//      // At this point, we have determined that the message is for us... so now we try to decode the contents
//      String receivedCMD = root["cmd"]; // this extracts the contents of "cmd" and puts it into a local variable
//      if (receivedCMD == "update")
//      {
//
//      }
//
//      /*
//         If the first element is a digit, we light up LED's and wait
//      */
//      //  else if(isDigit(receivedCMD[0]))
//      //  {
//      //  int targetFace = receivedCMD.toInt();
//
//      if () // cubeID's over 40 means it is attached by a cable... not a real cube // so we print
//      {
//        String targetID = root["targetID"];
//        String receivedCMD = root["cmd"];
//        String senderID = root["senderID"];
//        String messageString = "Message: From: " + senderID + " to: " + targetID + " Command is: " + receivedCMD;// + " Command is: ";
//        Serial.println(messageString);
//      }
//    }
//    attempts--;
//  }
//  return (resultBehavior);
//}

