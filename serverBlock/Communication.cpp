#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoJson.h>
#include "Communication.h"
#include "CBuff.h"
#include "Defines.h"

#define   BLINK_PERIOD    3000000 // microseconds until cycle repeat
#define   BLINK_DURATION  100000  // microseconds LED is on for
#define   MESH_SSID       "mblocks3d"
#define   MESH_PASSWORD   "mblocks3d"
#define   MESH_PORT       5555

painlessMesh mesh;

struct outboxEntry{
  uint32_t mID;
  int senderID;
  char cmd;
  uint32_t mDeadline;
  unsigned char backoff;
};

struct inboxEntry{
  uint32_t mID;
  char bottomFace;
};

inboxEntry inbox;
outboxEntry outbox;

/**
 * In the outbox, we need to keep track of each message that we are transmitting
 */

/**
 * These variables hold messages that need to be sent, and recieved messages that need to be
 * processed
 */

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


void initializeBoxes(){

}

void updateBoxes()
/*
 * This function checks the inbox to see if it is an ack for a message currently in the outbox.
 * If so, it clears both the inbox and the outbox.
 * If not, or if no message is in the inbox, it checks to see if the outbox message needs to be resent (and does so if needed).
 */
{
  // Clear out the inbox
  if(inbox.mID != 0)
  {
      if(outbox.mID == inbox.mID) {
        // if inbox is ack for outbox
        outbox.mID = 0;
        outbox.backoff = 0;
        outbox.mDeadline = 0;
        inbox.mID = 0;
        inbox.bottomFace = 0;
      }
    else{
      Serial.println("Spurious ACK for message ID: " + inboxItem.mID);
    }
  }

    if (outbox.mID != 0){ // for the outbox queues with messages in them...
      if (millis() > outbox.mDeadline) // if the time has come to resend the message...
      {
        //generate message
        sendMessage(TESTCUBE_ID, repeatCommand(outbox.cmd, outbox.mID)); // send it...
        outbox.mDeadline = millis() + random((1UL << outbox.backoff) * AVERAGE_FIRST_DELAY_MS); 
        // set the next deadline using exponential backoff...
        outbox.backoff++; // and increment the counter to reflect the number of tries.
        }
    }
  }

void receivedCallback(uint32_t from, String & msg)
{
  if (inboxEntry.mID == 0){
    StaticJsonBuffer<256> jsonMsgBuffer;
    JsonObject& jsonMsg = jsonMsgBuffer.parseObject(stringMsg);
    
    // ******
    // update data object for lattice
    InboxEntry.bottomFace = jsonMsg["bFace"];
    InboxEntry.mID = jsonMsg["mID"];
  }
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
  Serial.printf("Connection Event\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {}

void delayReceivedCallback(uint32_t from, int32_t delay) {}

String repeatCommand(char cmd, uint32_t mID = 0)
{
  if (mID == 0){
    mID = advanceLfsr();
  }
  //====== Generate a blink message =========
  StaticJsonBuffer<256> jsonBuffer; //Space Allocated to construct json instance
  JsonObject& jsonMsg = jsonBuffer.createObject(); // & is "c++ reference"
  //jsonMsg is our output, but in JSON form
  jsonMsg["mID"] =  mID;
  jsonMsg["type"] = 'c';
  jsonMsg["sID"] =  SERVER_ID;
  jsonMsg["cmd"] =  cmd;
  String strMsg; // generate empty string
  //strMsg is our output in String form
  jsonMsg.printTo(strMsg); // print to JSON readable string...
  return strMsg;
}

String newBlinkCommand()
{
  //====== Generate a blink message =========
  StaticJsonBuffer<256> jsonBuffer; //Space Allocated to construct json instance
  JsonObject& jsonMsg = jsonBuffer.createObject(); // & is "c++ reference"
  //jsonMsg is our output, but in JSON form
  jsonMsg["mID"] =  advanceLfsr();
  jsonMsg["type"] = 'c';
  jsonMsg["sID"] =  SERVER_ID;
  jsonMsg["cmd"] =  'b';
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
  jsonMsg["mID"] =  advanceLfsr();
  jsonMsg["type"] = 'c';
  jsonMsg["sID"] =  SERVER_ID;
  jsonMsg["cmd"] =  'f';
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
  jsonMsg["mID"] =  advanceLfsr();
  jsonMsg["type"] = 'c';
  jsonMsg["sID"] =  SERVER_ID;
  jsonMsg["cmd"] =  'r';
  String strMsg; // generate empty string
  //strMsg is our output in String form
  jsonMsg.printTo(strMsg); // print to JSON readable string...
  return strMsg;
}
