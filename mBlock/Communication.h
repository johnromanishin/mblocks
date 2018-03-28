#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <painlessMesh.h>  // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <Arduino.h>
#include <ArduinoJson.h>
#include "CBuff.h"

extern painlessMesh mesh; // This is the class for the wireless mesh, usage is mesh.function()
extern CircularBuffer<String, true> jsonCircularBuffer; // we store received messages in a circular buffer

void initializeWifiMesh();
bool sendMessage(String message);
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);


//void checkForMessage(Cube* c, String message);

#endif


// DEBUG MESSAGES
//  bool debug = false;
//  if(ESP.getChipId() == 9086927 ||ESP.getChipId() == 13374829)
//  {
//    debug = false;
//  }



//    if(debug)
//    {
//      StaticJsonBuffer<512> jsonBuffer; //Space Allocated to store json instance
//      JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
//      String message =  "  My ID# is: " + String(ESP.getChipId()) +
//                      " I SUCCEEDED IN UPDATING BOTH IMUs";                      
//      root["msg"] = message;       
//      root["cmd"]  = "debugMSG";  
//      root["cubeID"] = -1;                 
//      String newStr;
//    
//      root.printTo(newStr); 
//      mesh.sendBroadcast(newStr);
//    }

//    StaticJsonBuffer<512> jsonBuffer; //Space Allocated to store json instance
//    JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
//    String message =  "  My ID# is: " + String(ESP.getChipId()) +
//                      " MY i2c BUS LOCKED UP!!! UHOH, RESTARTING...";                      
//    root["msg"] = message;       
//    root["cmd"]  = "debugMSG";  
//    root["cubeID"] = -1;                 
//    String newStr;
//    root.printTo(newStr); 
//    mesh.sendBroadcast(newStr);



//
//
// StaticJsonBuffer<512> jsonBuffer; //Space Allocated to store json instance
//    JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
//    String message =  "  My ID# is: " + String(ESP.getChipId()) +
//                      "  I have no faceboards connected to me =(   ... " +
//                      "  My Battery Voltage is: " + String(voltage);
//    root["msg"] = message;       
//    root["cmd"]  = "debugMSG";  
//    root["cubeID"] = -1;                 
//    String newStr;
//    
//    root.printTo(newStr); 
//    mesh.sendBroadcast(newStr);


//
// while(1)
//  {
//    delay(1000);
//    PlaneEnum likelyStatus = c.findPlaneStatus();
//    c.updateSensors();
//    Serial.println("----------------------------");
//    Serial.print("Number of Neighbors: ");
//    Serial.println(c.numberOfNeighbors(0,0));
//    Serial.println("----------------------------");
//    if(likelyStatus == PLANE0123)       {Serial.println("PLANE0123");}
//    else if(likelyStatus == PLANE0425)  {Serial.println("PLANE0425");}
//    else if(likelyStatus == PLANE1453)  {Serial.println("PLANE1453");}
//    else if(likelyStatus == PLANENONE)  {Serial.println("PLANENONE");}
//    else if(likelyStatus == PLANEMOVING){Serial.println("PLANEMOVING");}
//    else if(likelyStatus == PLANEERROR) {Serial.println("PLANEERROR");}
//  }
