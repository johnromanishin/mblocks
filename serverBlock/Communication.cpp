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

/**
 * These variables hold messages that need to be sent, and recieved messages that need to be
 * processed
 */
#define NUM_MESSAGES_TO_BUFFER_OUTBOX 8 	// This is the max number of messages that can simultaneously fit in the outbox for a given cube.
																					// They are sent one-at-a-time
std::pair<String, uint32_t> outboxMem[16][NUM_MESSAGES_TO_BUFFER_OUTBOX];
CircularBuffer<std::pair<String, uint32_t>> outbox[16] =
{
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[0]), &outboxMem[0]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[1]), &outboxMem[1]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[2]), &outboxMem[2]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[3]), &outboxMem[3]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[4]), &outboxMem[4]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[5]), &outboxMem[5]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[6]), &outboxMem[6]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[7]), &outboxMem[7]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[8]), &outboxMem[8]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[9]), &outboxMem[9]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[10]), &outboxMem[10]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[11]), &outboxMem[11]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[12]), &outboxMem[12]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[13]), &outboxMem[13]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[14]), &outboxMem[14]),
  CircularBuffer<std::pair<String, uint32_t>>(ARRAY_SIZEOF(outboxMem[15]), &outboxMem[15])
};

#define NUM_MESSAGES_TO_BUFFER_INBOX 16
std::pair<String, uint32_t> inboxMem[NUM_MESSAGE_TO_BUFFER_INBOX];
CircularBuffer<std::pair<String, uint32_t>> inbox;

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
  Serial.println("rx message");

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

uint32_t advanceLfsr() // this call returns a message ID. these are not sequential.
{
  static uint32_t lfsr = 0xfefefe;

  uint32_t lsb = lfsr & 0x01u;
  lfsr >>= 1;
  if(lsb)
  {
    lfsr ^= (1u << 31) | (1u << 21) | (1u << 1) | (1u << 0);
  }
  return lfsr;
}

/**
 * In the outbox, we need to keep track of each message that we are going to transmit
 */
typedef struct outboxLog
{
  String contents;
  uint32_t mID;
  uint32_t mDeadline;
  uint32_t backoff;
}

String newBlinkCommand()
{
  //====== Generate a Broadcast message =========
  StaticJsonBuffer<256> jsonBuffer; //Space Allocated to construct json instance
  JsonObject& jsonMsg = jsonBuffer.createObject(); // & is "c++ reference"
  //jsonMsg is our output, but in JSON form
  jsonMsg["mID"] = 	advanceLfsr;
  jsonMsg["type"] = "cmd";
  jsonMsg["sID"] = 	SERVER_ID;
  jsonMsg["cmd"] = 	"blink";
  String strMsg; // generate empty string
  //strMsg is our output in String form
  msg.printTo(strMsg); // print to JSON readable string...
  return strMsg;
}

String newStatusCommand()
{
  //======Temporarily Generated a Broadcast message =========
  StaticJsonBuffer<256> jsonBuffer; //Space Allocated to construct json instance
  JsonObject& jsonMsg = jsonBuffer.createObject(); // & is "c++ reference"
  //jsonMsg is our output, but in JSON form
  jsonMsg["mID"] = 	advanceLfsr;
  jsonMsg["type"] = "cmd";
  jsonMsg["sID"] = 	SERVER_ID
  jsonMsg["cmd"] = 	"statReq";
  String strMsg; // generate empty string
  //strMsg is our output in String form
  jsonMsg.printTo(strMsg); // print to JSON readable string...
  return strMsg;
}

String newMoveCommand()
{
  //======Temporarily Generated a Broadcast message =========
  StaticJsonBuffer<256> jsonBuffer; //Space Allocated to construct json instance
  JsonObject& jsonMsg = jsonBuffer.createObject(); // & is "c++ reference"
  //jsonMsg is our output, but in JSON form
  jsonMsg["mID"] = 	advanceLfsr;
  jsonMsg["type"] = "cmd";
  jsonMsg["sID"] = 	SERVER_ID
  jsonMsg["cmd"] = 	"statReq";
  String strMsg; // generate empty string
  //strMsg is our output in String form
  jsonMsg.printTo(strMsg); // print to JSON readable string...
  return strMsg;
}
