#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoJson.h>

#include <string.h>

#include "Communication.h"
//#include "Cube.h"
#include "espconn.h"
#include "CBuff.h"
//#include "Face.h"

#define   BLINK_PERIOD    3000000 // microseconds until cycle repeat
#define   BLINK_DURATION  100000  // microseconds LED is on for
#define   MESH_SSID       "mblocks3d"
#define   MESH_PASSWORD   "mblocks3d"
#define   MESH_PORT       5555

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

long initializeWifiMesh()
{
  if(MAGIC_DEBUG) Serial.println("Beginning initializeWifiMesh");
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  //mesh.onNewConnection(&newConnectionCallback);
  //mesh.onChangedConnections(&changedConnectionCallback);
  //mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  //mesh.onNodeDelayReceived(&delayReceivedCallback);
  randomSeed(analogRead(A0));
  return(mesh.getNodeId());
  if(MAGIC_DEBUG) Serial.println("Exiting initializeWifiMesh");
}

void receivedCallback(uint32_t from, String & msg)
{
  if(MAGIC_DEBUG)
  {
    Serial.print("Message Received from: ");
    Serial.println(from);
  }

  // Check and see if this message is a dupe by "manually" extracting the message
  // id field.
  char *s = msg.c_str();
  int len = msg.length();
  uint32_t mid = 0;
  for(int i = 0; i < len; i++)
  {
    if((s[i] == '\"') && (!strncmp(&s[i] + 1, "mID\"", 4)))
    {
      mid = strtol(&s[i], NULL, 10);
    }
  }

  Serial.printf("Extracted mid %i\r\n", mid);

  if(mid != prevMID)
  {
    jsonCircularBuffer.push(msg);
  }
  prevMID = mid;

  // Send an ack message.
  // The ack consists of
  //     type:      "ack"
  //     targetID:  ID of server that this ack goes to
  //     cubeID:    ID of cube sending this message
  //     neighbors: Number of neighbors surrounding the cube.
  //     topFace:   Face pointing upwards
  sendAck(Cube* c, )
}

void sendAck(Cube* c, uint32_t messageID, int serverNumber)
{
  Serial.println("sending ack");
  StaticJsonBuffer<256> jsonBuffer; //Space Allocated to store json instance
  JsonObject& msg = jsonBuffer.createObject(); // & is "c++ reference"
  msg["mID"] = messageID;
  msg["type"] = "ack";
  msg["targetID"] = serverNumber;
  msg["cubeID"] = c->cubeID;
  msg["neighbors"] = c->numberOfNeighbors();
  msg["topFace"] = c->returnTopFace(0);
  //^ "key"   |  ^ "Value"

  String str; // generate empty string
  msg.printTo(str); // print to JSON readable string...
  sendMessage(serverNumber, str);
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
