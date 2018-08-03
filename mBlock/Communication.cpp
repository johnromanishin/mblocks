//#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
//#include <ArduinoJson.h>

#include <string.h>

#include "Communication.h"

#define   BLINK_PERIOD    3000000 // microseconds until cycle repeat
#define   BLINK_DURATION  100000  // microseconds LED is on for
#define   MESH_SSID       "mblocks3d"
#define   MESH_PASSWORD   "mblocks3d"
#define   MESH_PORT       5555

#define   SERVER_NUMBER   99
//#define   SPECIAL_MID     42

painlessMesh  mesh; // instantiates the class "mesh" which handles the wireless messages

// CircularBuffer<int> axCoreBuffer(ARRAY_SIZEOF(this->axCoreData), this->axCoreData),
String jsonBufferSpace[40];

CircularBuffer<String, true> jsonCircularBuffer(ARRAY_SIZEOF(jsonBufferSpace), jsonBufferSpace);

/*
 * These variables are used in keeping track of which message ID's we have already seen
 */
#define   seenMsgCount 10
int      prevMIDindex = 0;
uint32_t prevMID[seenMsgCount] = {};


bool calc_delay = false;
SimpleList<uint32_t> nodes;

bool sendMessage(int cubeID, String msg)
/*
 * This command attempts to send an already formatted STRING message
 * to the address cubeID - If cubeID is -1, it sends it as a broadcast
 * The message already needs to be formatted as a json before getting 
 * passed into this function
 */
{
  if(cubeID == -1)
  {
    return(mesh.sendBroadcast(msg));
  }
  else
  {
    uint32_t address = getAddressFromCubeID(cubeID); // this convers to MAC address space
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

#define messageDebug 0

void receivedCallback(uint32_t from, String & stringMsg)
{
  if(messageDebug)
  {
    Serial.print("Message Received from: ");
    Serial.println(from);
    Serial.print(" Message Contents: ");
    Serial.println(stringMsg);
  }
  
  /*
   * If the commands is "q" then we quickly return an ack, and then go back to the 
   * regularly scheduled programming...
   */
  if(stringMsg == "q")
  {
    Serial.println("Responded to simple ACK");
    sendAck(SPECIAL_MID); // if this is a "q" message, we know that the cube is trying to figure out who is there...
    return;
  }
  /*
   * Now we extract the Command "cmd"
   * and the message ID         "mID"
   * from the message and store them in local variables
   */
  StaticJsonBuffer<256> jsonMsgBuffer;
  JsonObject& jsonMsg = jsonMsgBuffer.parseObject(stringMsg);
  uint32_t mID = jsonMsg["mID"];
  String command = jsonMsg["cmd"];
  /*
   * Now we are trying to see if we have seen this specific mID before...
   * We do this with the seenMessage flag... Everytime we see a new message,
   * we add it to an array of the latest 10 mID's we have seen | prevMID[index]
   */
  bool newMessage = true;

  /*
   * Cycle through the 10 most recent mID's
   * and see if the current mID matches to any of them...
   */
  for(int i = 0; i < seenMsgCount; i++)
  {
    if(mID == prevMID[i])
    {
      newMessage = false;
    }
  }
  /*
   * If the index gets above seenMsgCount, we roll it back over to 0...
   */
  if(prevMIDindex >= seenMsgCount)
  {
    prevMIDindex = 0;
  }

  /*
   * At this point we bool newMessage is true if we haven't seen the messageID recently
   * So if this is true, we add the command to be processed... If it is a repeat, we return
   */
  /*
   * push the new mID
   */
  if(newMessage == true)
  {
    prevMID[prevMIDindex] = mID;
    prevMIDindex++;
    jsonCircularBuffer.push(stringMsg);
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
  msg["sID"] = thisCubeID; // sender ID
  
  //if(messageID != SPECIAL_MID)
  
  msg["bFace"] = bFace; // c->returnTopFace(0);
  msg["fFace"] = fFace;
  
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
