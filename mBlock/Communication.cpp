#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoJson.h>

#include <string.h>

#include "Communication.h"
#include "CBuff.h"

#define   BLINK_PERIOD    3000000 // microseconds until cycle repeat
#define   BLINK_DURATION  100000  // microseconds LED is on for
#define   MESH_SSID       "mblocks3d"
#define   MESH_PASSWORD   "mblocks3d"
#define   MESH_PORT       5555

#define   SERVER_NUMBER   99

painlessMesh  mesh; // instantiates the class "mesh" which handles the wireless messages

// CircularBuffer<int> axCoreBuffer(ARRAY_SIZEOF(this->axCoreData), this->axCoreData),
String jsonBufferSpace[40];

CircularBuffer<String, true> jsonCircularBuffer(ARRAY_SIZEOF(jsonBufferSpace), jsonBufferSpace);

uint32_t prevMID = 0;

bool calc_delay = false;
SimpleList<uint32_t> nodes;

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

uint32_t initializeWifiMesh()
{
  if(MAGIC_DEBUG) Serial.println("Beginning initializeWifiMesh");
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  //mesh.onNewConnection(&newConnectionCallback);
  //mesh.onChangedConnections(&changedConnectionCallback);
  //mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  //mesh.onNodeDelayReceived(&delayReceivedCallback);
  randomSeed(ESP.getChipId());
  if(MAGIC_DEBUG) Serial.println("Exiting initializeWifiMesh");
  return(mesh.getNodeId());
}

#define messageDebug 1
void receivedCallback(uint32_t from, String & stringMsg)
{
   Serial.print("Message Received from: ");
   Serial.println(from);
   Serial.print("Message Contents: ");
   Serial.println(stringMsg);


  // Check and see if this message is a dupe by "manually" extracting the message
  // id field.
  
  
  //check to see if it's new, if so, do something with it
  StaticJsonBuffer<256> jsonMsgBuffer;
  JsonObject& jsonMsg = jsonMsgBuffer.parseObject(stringMsg);
  //String mIDstring = jsonMsg["mID"];
  uint32_t mID = jsonMsg["mID"];
  String command = jsonMsg["cmd"];
  if (command == "q")
  {
    sendAck(mID);
    return;
  }
  
  if (mID != prevMID)
  {
    Serial.println("Adding to Buffer...");
    jsonCircularBuffer.push(stringMsg);
    prevMID = mID;
  }
  
  // Send an ack message.
  // The ack consists of
  //     type:      "ack"
  //     targetID:  ID of server that this ack goes to
  //     cubeID:    ID of cube sending this message
  //     neighbors: Number of neighbors surrounding the cube.
  //     topFace:   Face pointing upwards
  
  // this is what we had before:
  // sendAck(Cube* c, mID);
  // and this is what we added 3Apr 1:30AM to make it compilable:
  
  sendAck(mID);
}

// this is what we had before:
// void sendAck(Cube* c, uint32_t messageID)
// and this is what we added 3Apr 1:30AM to make it compilable:
void sendAck(uint32_t messageID)
{
  if(MAGIC_DEBUG) Serial.println("sending ack to server");
  StaticJsonBuffer<256> jsonBuffer; //memory allocated to store json instance
  JsonObject& msg = jsonBuffer.createObject(); // & is "c++ reference"
  msg["mID"] = messageID; // message ID
  msg["type"] = "ack";
  msg["sID"] = thisCubeID; // sender ID
  //msg["neighbors"] = "lots"; // c->numberOfNeighbors();
  msg["bFace"] = bFace; // c->returnTopFace(0);
  
  if(f0 > 0)
  {
     msg["f0"] = f0;
  }
  if(f1 > 0)
  {
     msg["f1"] = f1;
  }
  if(f2 > 0)
  {
     msg["f2"] = f2;
  }
  if(f3 > 0)
  {
     msg["f3"] = f3;
  }
  if(f4 > 0)
  {
     msg["f4"] = f4;
  }
  if(f5 > 0)
  {
     msg["f5"] = f5;
  }
  
  String str; // generate empty string
  msg.printTo(str); // print to JSON readable string...
  sendMessage(SERVER_NUMBER, str);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////NOTES////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////



//void newConnectionCallback(uint32_t nodeId)
//{
//  //Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
//}
//
//void changedConnectionCallback()
//{
////  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
////  nodes = mesh.getNodeList();
////  Serial.printf("Num nodes: %d\n", nodes.size());
////  Serial.printf("Connection list:");
////  SimpleList<uint32_t>::iterator node = nodes.begin();
////  while (node != nodes.end()) {
////    Serial.printf(" %u", *node);
////    node++;
////  }
////  calc_delay = true;
//}
//
//void nodeTimeAdjustedCallback(int32_t offset)
//{
//  //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
//}
//
//void delayReceivedCallback(uint32_t from, int32_t delay)
//{
//  //Serial.printf("Delay to node %u is %d us\n", from, delay);
//}



//
//Behavior relayBehavior(Cube* c, Behavior behaviorToRelay, int cubeToRelayTo, int timesToRelay)
//{
//  //======Temporarily Generated a Broadcast message =========
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
//  for(int i = 0; i < timesToRelay; i++);
//  {
//    mesh.sendBroadcast(str);
//    wifiDelay(400);
//  }
//  return(behaviorToRelay);
//}

//String generateUpdateMessage(Cube* c)
//{
//  StaticJsonBuffer<264> jsonBuffer; //Space Allocated to store json instance
//  JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
//  //^class type||^ Root         ^class method
//  root["type"] = "update";
//  root["cubeID"] = c->cubeID;
//  root["topFace"] = c->returnTopFace(0);
//  //root["plane"]
//  //^ "key"   |  ^ "Value"
//  String str; // generate empty string
//  return(root.printTo(str)) // print to JSON readable string...
//}
