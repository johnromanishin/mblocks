#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <painlessMesh.h>  // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <Arduino.h>
#include <ArduinoJson.h>

#include "Defines.h"
#include "CBuff.h"
#include "Cube.h"
//#include "Face.h"

// Wifi Infrastructure
extern painlessMesh mesh; // This is the class for the wireless mesh, usage is mesh.function()
extern CircularBuffer<String, true> jsonCircularBuffer; // we store received messages in a circular buffer
uint32_t initializeWifiMesh();
void receivedCallback(uint32_t from, String & msg);

///// Sending Messages Utilities
bool sendMessage(int cubeID, String msg);
void sendAck(uint32_t messageID);

///// 
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////NOTES////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////



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



//   String newmsg = "Angle: " + String(c.coreMagnetAngleBuffer.access(0) - initialMagnetReadingOffset)
//   + " core.ax: " + String(c.axCoreBuffer.access(0))
//   + " core.ay: " + String(c.ayCoreBuffer.access(0))
//   + " core.az: " + String(c.azCoreBuffer.access(0))
//   + " Frame.ax: " + String(c.axFrameBuffer.access(0))
//   + " Frame.ay: " + String(c.ayFrameBuffer.access(0))
//   + " Frame.az: " + String(c.azFrameBuffer.access(0))
//   + " CoreMagAGC: " + String(c.coreMagnetStrengthBuffer.access(0));


////======Temporarily Generated a Broadcast message =========
//  StaticJsonBuffer<264> jsonBuffer; //Space Allocated to store json instance
//  JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
//  //^class type||^ Root         ^class method                   
//  root["type"] = "cmd";
//  root["cubeID"] = cubeToRelayTo;
//  root["cmd"] = behaviorsToCmd(behaviorToRelay);
//  //^ "key"   |  ^ "Value"
//  String str; // generate empty string
//  root.printTo(str); // print to JSON readable string...
//  //======== End Generating of Broadcast message ==========
//  

