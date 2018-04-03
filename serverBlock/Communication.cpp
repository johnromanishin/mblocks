#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoJson.h>
#include "Communication.h"
#include "espconn.h"
#include "CBuff.h"
#include "Defines.h"

#define   BLINK_PERIOD    3000000 // microseconds until cycle repeat
#define   BLINK_DURATION  100000  // microseconds LED is on for
#define   MESH_SSID       "mblocks3d"
#define   MESH_PASSWORD   "mblocks3d"
#define   MESH_PORT       5555

painlessMesh  mesh;

/**
 * In the outbox, we need to keep track of each message that we are going to transmit
 */
typedef struct outboxLog
{
  String contents;
  uint32_t mID;
  uint32_t mDeadline;
  char backoff;
} outboxLog;

typedef struct inboxLog
{
  String contents;
  uint32_t mID;
} inboxLog;

typedef struct faceState
{
  char ID;
  char connectedCube;
  char connectedFace;
  char connectedAngle;
} faceState;

typedef struct cubeState
{
  char bottomFace;
  char plane;
  faceState faceA;
  faceState faceB;
  faceState faceC;
  faceState faceD;
  faceState faceE;
  faceState faceF;

} cubeState;

/**
 * These variables hold messages that need to be sent, and recieved messages that need to be
 * processed
 */
#define NUM_MESSAGES_TO_BUFFER_OUTBOX 5 	// This is the max number of messages that can simultaneously fit in the outbox for a given cube.
																					// They are sent one-at-a-time
outboxLog outboxMem[16][NUM_MESSAGES_TO_BUFFER_OUTBOX];

//CircularBuffer<outboxLog> outbox[16] =
//{
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[0]), &outboxMem[0]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[1]), &outboxMem[1]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[2]), &outboxMem[2]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[3]), &outboxMem[3]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[4]), &outboxMem[4]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[5]), &outboxMem[5]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[6]), &outboxMem[6]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[7]), &outboxMem[7]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[8]), &outboxMem[8]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[9]), &outboxMem[9]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[10]), &outboxMem[10]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[11]), &outboxMem[11]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[12]), &outboxMem[12]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[13]), &outboxMem[13]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[14]), &outboxMem[14]),
//  CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[15]), &outboxMem[15])
//};

#define NUM_MESSAGES_TO_BUFFER_INBOX 16
inboxLog inboxMem[NUM_MESSAGE_TO_BUFFER_INBOX];
//CircularBuffer<inboxLog> inbox;

#define NUM_CUBES 16
cubeState topologyMem[NUM_CUBES];
//CircularBuffer<cubeState> topology[16] =
//{
//  delay(1);
//  //% TODO
//};

bool calc_delay = false;
SimpleList<uint32_t> nodes;
void sendMessage() ; // Prototype

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

bool sendMessage(int recipientID, String msg)
{
  if(recipientID == -1)
  {
    return(mesh.sendBroadcast(msg));
  }
  else
  {
    uint32_t address = getAddressFromCubeID(recipientID);
    return(mesh.sendSingle(address, msg));
  }
}

/**
 * This function looks through newly-recieved messages, and prunes waiting
 * messages in the outbox, sending them if appropriate.
 *
 * It also updates the state of the cubes
 *
 * XXX TODO:
 */
void updateBoxes(CircularBuffer<inboxLog>& inbox, CircularBuffer<outboxLog>& outbox[])
{
  // Clear out the inbox
  while(!inbox.empty())
  {
    // Retrieve the most recent thing in the inbox
    inboxLog inboxItem = inbox.pop();

    // See if we need to mark any outbox messages as "acked"
    bool foundflag = false;
    for(int i = 0; i < ARRAY_SIZEOF(outbox); i++)
    {
      if(outbox[i].access(0).mID == inboxItem.mID) {
        foundflag = true;
        outbox[i].pop();
        //sendnextmessagetocubei()
      }
    }
    if(!foundflag) {
      printf("No sent message found in outbox for message ID %");
    }

    // XXXTODO incorporate new inboxItem into the state of the cubes
  }

  // Decide which messages to send next. To do this, we search through all of the messages
  // at the front of the circular buffers and find the empty ones
  uint32_t mintime = 0xffffffffu;
  int minidx = -1;
  for(int i = 0; i < ARRAY_SIZE(outbox); i++) {
    if(!outbox[i].empty() && (outbox[i].access(0).mDeadline < mintime)) {
      mintime = outbox[i].access(0).mDeadline;
      minidx = i;
    }
  }

  // Check and make sure that the minimum time that we just found is past the current mtime
  if(mintime < millis()) {
    sendMessage()
  }
}

void receivedCallback(uint32_t from, String & msg)
{
  Serial.println("Received ACK:\n" + msg + "\n");

	char *s = msg.c_str();
  int len = msg.length();
  uint32_t mID = 0;
  for(int i = 0; i < len; i++)
  {
    if((s[i] == '\"') && (!strncmp(&s[i] + 1, "mID\"", 4)))
    {
      mID = strtol(&s[i], NULL, 10);
    }
  }

  Serial.printf("Extracted mID %i\r\n", mID);

  inbox.push({msg, mID, 0, 0})
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
  Serial.printf("Connection Event\n");
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

String newBlinkCommand()
{
  //====== Generate a blink message =========
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
  //====== Generate a status request =========
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

String newForwardCommand()
{
  //====== Generate a Move command =========
  StaticJsonBuffer<256> jsonBuffer; //Space Allocated to construct json instance
  JsonObject& jsonMsg = jsonBuffer.createObject(); // & is "c++ reference"
  //jsonMsg is our output, but in JSON form
  jsonMsg["mID"] = 	advanceLfsr;
  jsonMsg["type"] = "cmd";
  jsonMsg["sID"] = 	SERVER_ID
  jsonMsg["cmd"] = 	"F";
  String strMsg; // generate empty string
  //strMsg is our output in String form
  jsonMsg.printTo(strMsg); // print to JSON readable string...
  return strMsg;
}

String newReverseCommand()
{
  //====== Generate a Move command =========
  StaticJsonBuffer<256> jsonBuffer; //Space Allocated to construct json instance
  JsonObject& jsonMsg = jsonBuffer.createObject(); // & is "c++ reference"
  //jsonMsg is our output, but in JSON form
  jsonMsg["mID"] =  advanceLfsr;
  jsonMsg["type"] = "cmd";
  jsonMsg["sID"] =  SERVER_ID
  jsonMsg["cmd"] =  "F";
  String strMsg; // generate empty string
  //strMsg is our output in String form
  jsonMsg.printTo(strMsg); // print to JSON readable string...
  return strMsg;
}
