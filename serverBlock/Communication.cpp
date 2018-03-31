#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoJson.h>
#include "Communication.h"
#include "espconn.h"
#include "CBuff.h"

#define   BLINK_PERIOD    3000000 // microseconds until cycle repeat
#define   BLINK_DURATION  100000  // microseconds LED is on for
#define   MESH_SSID       "mblocks3d"
#define   MESH_PASSWORD   "mblocks3d"
#define   MESH_PORT       5555

painlessMesh  mesh;

String jsonBufferSpace[500];
CircularBuffer<String, true> jsonCircularBuffer(500, jsonBufferSpace);

bool calc_delay = false;
SimpleList<uint32_t> nodes;
void sendMessage() ; // Prototype

void initializeWifiMesh()
{
    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
    mesh.onNodeDelayReceived(&delayReceivedCallback);
    randomSeed(analogRead(A0));
}

bool sendBroadcastMessage(String message)
{
  String msg = ""; // create empty string
  msg = message;   // assign contents of "message"
  bool error = mesh.sendBroadcast(msg);
//
//  if (calc_delay) {
//    SimpleList<uint32_t>::iterator node = nodes.begin();
//    while (node != nodes.end()) {
//      mesh.startDelayMeas(*node);
//      node++;
//    }
//    calc_delay = false;
//  }
}

bool sendMessage(int cubeID, String msg)
{
  if(cubeID == -1)
  {
    return(mesh.sendBroadcast(msg));
  }
  else
  {
    uint32_t address = getAddressFromCubeID(cubeID);
    return(mesh.sendSingle(address, msg));
  }
}

void receivedCallback(uint32_t from, String & msg)
{
  jsonCircularBuffer.push(msg);
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() 
{
//  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
//  nodes = mesh.getNodeList();
//  Serial.printf("Num nodes: %d\n", nodes.size());
//  Serial.printf("Connection list:");
//  SimpleList<uint32_t>::iterator node = nodes.begin();
//  while (node != nodes.end()) {
//    Serial.printf(" %u", *node);
//    node++;
//  }
//  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  //Serial.printf("Delay to node %u is %d us\n", from, delay);
}


void makeThemBlink(int recipientCube)
{
  //======Temporarily Generated a Broadcast message =========
  StaticJsonBuffer<512> jsonBuffer; //Space Allocated to store json instance
  JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
  //^class type||^ Root         ^class method  
  root["type"] = "cmd";
  root["targetID"] = recipientCube;
  root["senderID"] = 55; //getCubeIDFromEsp(mesh.getNodeId());
  root["cmd"] = "blink";
  //^ "key"   |  ^ "Value"
  String str; // generate empty string
  root.printTo(str); // print to JSON readable string...
  //======== End Generating of Broadcast message ==========
  sendMessage(recipientCube, str);
}

//void (int targetID, String cmd)
//{
//  StaticJsonBuffer<200> jsonBuffer;
//  JsonObject& root = jsonBuffer.createObject();
//  //^class type||^ Root         ^class method
//  root["type"] = "cmd";
//  root["targetID"] = targetID;
//  root["cmd"] = cmd;
//  //^ "key"   |  ^ "Value"
//  String strang = "";
//  root.printTo(strang);
//  return (strang);
//}

void requestStatus(int recipientCube)
{
  //======Temporarily Generated a Broadcast message =========
  StaticJsonBuffer<128> jsonBuffer; //Space Allocated to store json instance
  JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
  //^class type||^ Root         ^class method                   
  root["type"] = "cmd";
  root["targetID"] = recipientCube;
  root["senderID"] = getCubeIDFromEsp(mesh.getNodeId());
  root["cmd"] = "sendstatus";
  //^ "key"   |  ^ "Value"
  String str; // generate empty string
  root.printTo(str); // print to JSON readable string...
  //======== End Generating of Broadcast message ==========
  
  if (recipientCube == -1){
    mesh.sendBroadcast(str);
  }
  // THIS NEXT LINE WON'T WORK UNTIL RECIPIENTCUBE'S ADDRESS LOOKUP IS BUILT OUT AND USED
  // else mesh.sendSingle(recipientCube, str);
}

void sendStatus(int cubeID)
{
  //======Temporarily Generated a Broadcast message =========
  StaticJsonBuffer<256> jsonBuffer; //Space Allocated to store json instance
  JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
  //^class type||^ Root         ^class method                   
  root["type"] = "status";
  root["targetID"] = cubeID;
  root["senderID"] = getCubeIDFromEsp(mesh.getNodeId());
  root["upface"] = ""; // probably need to write a getUpFace method in Cube class
  root["neighbors"] = ""; // this should be the serialization of 
  root["f1"] = " ";
  root["f2"] = " "; 
  root["f3"] = " ";
  root["f4"] = " ";
  root["f5"] = " ";
  root["f6"] = " "; 
  // the connection table; may want to separate out the angle from the face, 
  // and the face from the cube

  //^ "key"   |  ^ "Value"
  String str; // generate empty string
  root.printTo(str); // print to JSON readable string...
  //======== End Generating of Broadcast message ==========
  
  if (cubeID == -1){
    mesh.sendBroadcast(str);
  }
}
