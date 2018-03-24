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
  Serial.print("ESP Chip ID: ");
  Serial.println(ESP.getChipId());
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
  if(mesh.isConnected(2139793359))
  {
    //Serial.println("BIG BAD BREADBOARD CUBE IS UP AND RUNNING!!");
  }
  int rangeValue = readRangeSensor();
  if(rangeValue < 50)
  {
    sendBroadcastMessage(createJsonStringFlood(-1, "sleep"));
  }
  //messageSingle(2139793359);
  
  makeThemBlink(99);
  countdown(mainLoopCounter, 99);
  wifiDelay(1000);
  mainLoopCounter++;
  if(mainLoopCounter == 7)
  {
    mainLoopCounter = 0;
  }
}

String createJsonStringFlood(int targetID, String cmd)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  //^class type||^ Root         ^class method 
  root["type"] = "cmd";
  root["targetID"] = targetID;
  root["cmd"] = cmd;
  //^ "key"   |  ^ "Value"
  String strang = "";
  root.printTo(strang);
return(strang);
}

//    StaticJsonBuffer<200> jsonBuffer; //Space Allocated to store json instance
//    JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
//    //^class type||^ Root         ^class method                   
//    root["type"] = "cmd";
//    root["targetID"] = 55;
//    root["senderID"] = getCubeIDFromEsp(ESP.getChipId());
//    root["cmd"] = "update";
//    //^ "key"   |  ^ "Value"
    //String str = "yo"; // generate empty string
    //root.printTo(str); // print to JSON readable string...

    
//Behavior cmdToBehaviors(String cmd, Behavior defaultBehavior)
///*  
// *   This function takes in a string, and if it matches to a behavior, it 
// *   returns that behavior. If the string doesn't match, it returns
// */
//{
//  Behavior behaviorToReturn = defaultBehavior;
//       if(cmd == "solo_light_track")    {behaviorToReturn = SOLO_LIGHT_TRACK;}
//  else if(cmd == "duo_light_track")     {behaviorToReturn = DUO_LIGHT_TRACK;}
//  else if(cmd == "follow_arrows")       {behaviorToReturn = FOLLOW_ARROWS;}
//  else if(cmd == "chilling")            {behaviorToReturn = CHILLING;}
//  else if(cmd == "climb")               {behaviorToReturn = CLIMB;}
//  else if(cmd == "attractive")          {behaviorToReturn = ATTRACTIVE;}
//  else if(cmd == "shut_down")           {behaviorToReturn = SHUT_DOWN;}
//  else if(cmd == "sleep")               {behaviorToReturn = SLEEP;}
//  else if(cmd == "multi_light_track")   {behaviorToReturn = MULTI_LIGHT_TRACK;}
//  else if(cmd == "pre_solo_light")      {behaviorToReturn = PRE_SOLO_LIGHT;}
//  
//return(behaviorToReturn);
//}
//
//String behaviorsToCmd(Behavior inputBehavior)
///*  
// *   This function takes in a string, and if it matches to a behavior, it 
// *   returns that behavior. If the string doesn't match, it returns
// */
//{
//  String stringToReturn = " ";
//       if(inputBehavior == SOLO_LIGHT_TRACK)      {stringToReturn = "solo_light_track";} 
//  else if(inputBehavior == DUO_LIGHT_TRACK)       {stringToReturn = "duo_light_track";}
//  else if(inputBehavior == FOLLOW_ARROWS)         {stringToReturn = "follow_arrows";}
//  else if(inputBehavior == CHILLING)              {stringToReturn = "chilling";}
//  else if(inputBehavior == CLIMB)                 {stringToReturn = "climb";}
//  else if(inputBehavior == ATTRACTIVE)            {stringToReturn = "attractive";}
//  else if(inputBehavior == SHUT_DOWN)             {stringToReturn = "shut_down";}
//  else if(inputBehavior == SLEEP)                 {stringToReturn = "sleep";}
//  else if(inputBehavior == MULTI_LIGHT_TRACK)     {stringToReturn = "multi_light_track";}
//  else if(inputBehavior == PRE_SOLO_LIGHT)        {stringToReturn = "pre_solo_light";}
//return(stringToReturn);
//}

