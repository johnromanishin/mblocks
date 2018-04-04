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
 * In the outbox, we need to keep track of each message that we are transmitting
 */


#define NUM_MESSAGES_TO_BUFFER_OUTBOX 4 	// This is the max number of messages that can simultaneously fit in the outbox for a given cube.
																					// They are sent to the cube one-at-a-time
outboxLog outboxMem[NUM_CUBES][NUM_MESSAGES_TO_BUFFER_OUTBOX];

CircularBuffer<outboxLog> outbox[NUM_CUBES] =
{
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[0]), &outboxMem[0]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[1]), &outboxMem[1]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[2]), &outboxMem[2]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[3]), &outboxMem[3]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[4]), &outboxMem[4]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[5]), &outboxMem[5]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[6]), &outboxMem[6]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[7]), &outboxMem[7]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[8]), &outboxMem[8]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[9]), &outboxMem[9]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[10]), &outboxMem[10]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[11]), &outboxMem[11]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[12]), &outboxMem[12]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[13]), &outboxMem[13]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[14]), &outboxMem[14]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[15]), &outboxMem[15]),
 CircularBuffer<outboxLog>(ARRAY_SIZEOF(outboxMem[16]), &outboxMem[16]),
};

inboxLog inboxMem[NUM_CUBES * WINDOW_SIZE];
CircularBuffer<inboxLog> inbox;

// this is where the cube data object will be built in the future
//
//
//

bool calc_delay = false;
SimpleList<uint32_t> nodes;

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
 * It also updates the state of the cube data model
 */

#define AVERAGE_FIRST_DELAY_MS 100

void updateBoxes(CircularBuffer<inboxLog>& inbox, CircularBuffer<outboxLog>& outbox[])
{
  // Clear out the inbox
  while(!inbox.empty())
  {
    // Retrieve the most recent thing in the inbox
    inboxLog inboxItem = inbox.pop();

    // See if we need to mark any outbox messages as "acked"
    bool foundflag = false;
    for(int cub = 0; cub < (NUM_CUBES); cub++)
    {
      if(outbox[cub].access(0).mID == inboxItem.mID) {
        foundflag = true;
        outbox[cub].pop();
      }
    }
    if(!foundflag) {
      Serial.println("Spurious ACK for message ID: " + inboxItem.mID);
    }

    // updateCubeModelWithAck() TODO incorporate new inboxItem into the state of the cubes
  }

  // Decide which messages to (re)send.
  for (int cub = 0; cub < (NUM_CUBES); cub++){
    if (!outbox[cub].empty()){ // for the outbox queues with messages in them...
      if (millis()>outbox[cub].access(0).mDeadline) // if the time has come to resend the message...
      {
        sendMessage(cub, outbox[cub].access(0).mContents); // send it...
        outbox.cub.access(0).mDeadline = millis() + random((1ul << outbox.cub.access(0).backoff) * AVERAGE_FIRST_DELAY_MS); // set the next deadline using exponential backoff...
        outbox.cub.access(0).backoff++; // and increment the counter to reflect the number of tries.
        }
    }
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
