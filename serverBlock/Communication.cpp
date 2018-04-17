#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoJson.h>
#include "Communication.h"
#include "CBuff.h"
#include "Defines.h"

// Parameters for the the WIFI mesh network
#define   BLINK_PERIOD    3000000 // microseconds until cycle repeat
#define   BLINK_DURATION  100000  // microseconds LED is on for
#define   MESH_SSID       "mblocks3d"
#define   MESH_PASSWORD   "mblocks3d"
#define   MESH_PORT       5555

// Class instance for the WIFI mesh
painlessMesh mesh;

/** INBOX
   These variables hold messages that need to be sent, and recieved messages that need to be
   processed
*/
inboxEntry inbox;

/** OUTBOX
   In the outbox, we need to keep track of each message that we are transmitting
*/
outboxEntry outbox[OUTBOX_SIZE] ; // outboxEntry outbox[cubeID][OUTBOX_SIZE];
int outboxHead = 0; //
int outboxTail = 0; //

/**
   This function looks through newly-recieved messages, and prunes waiting
   messages in the outbox, sending them if appropriate.

   It also updates the state of the cube data model
*/

//typedef struct outboxEntry
//{
//  uint32_t mID;
//  int senderID;
//  String cmd;
//  uint32_t mDeadline;
//  unsigned char backoff;
//} outboxEntry ;

void initializeOutboxes() 
{
  outbox[head].mID=0;
}

void updateBoxes()
/*
   This function checks the inbox to see if it is an ack for a message currently in the outbox.
   If so, it clears both the inbox and the outbox.
   If not, or if no message is in the inbox, it checks to see if the outbox message needs to be resent 
   (and does so if needed).
*/
{
  // Clear out the inbox
  if (inbox.mID != 0)
  {
    if (outbox[outboxTail].mID == inbox.mID) // This means that we successfully acknowledged a message
    {
       //if inbox is ack for outbox
       outbox[outboxTail].mID = 0;
       advanceOutboxTail();
    }
    else 
    {       
      Serial.println("Spurious ACK");
    }
    inbox.mID = 0;
    inbox.bottomFace = 0;
  }
  
  if (outbox[outboxTail].mID != 0) { // for the outbox queues with messages in them...
    if (millis() > outbox[outboxTail].mDeadline) // if the time has come to resend the message...
    {
      //generate message
      sendMessage(TESTCUBE_ID, generateMessageText(outbox[outboxTail].cmd, outbox[outboxTail].mID)); // send it...
      outbox[outboxTail].mDeadline = millis() + random((1UL << outbox[outboxTail].backoff) * AVERAGE_FIRST_DELAY_MS * 2);
      // set the next deadline using exponential backoff,
      outbox[outboxTail].backoff++;
      // and increment the counter to reflect the number of tries.
      
      Serial.println("mID ");
      Serial.println(String(outbox[outboxTail].mID));
      Serial.println("cmd ");
      Serial.println(String(outbox[outboxTail].cmd));
      Serial.println("backoff ");
      Serial.println(String(outbox[outboxTail].backoff));
      Serial.println("mDeadline ");
      Serial.println(String(outbox[outboxTail].mDeadline));
      Serial.println("senderID ");
      Serial.println(String(outbox[outboxTail].senderID));
    }
  }
}

void receivedCallback(uint32_t from, String & stringMsg)
/*
 * This function gets called by the mesh library whenever we receive a wifi Message
 * 
 * The goal is to check to see if it is a new message, and if so, we add it to a 
 * list of messages to be processed by inboxbuffer
 */
{
  Serial.print("Received message from: ");
  Serial.println(from);
  Serial.print("Message Contents: ");
  Serial.println(stringMsg);

  if (inbox.mID == 0) //if no current item in inbox
  {
    StaticJsonBuffer<512> jsonMsgBuffer; // allocate memory for json
    JsonObject& jsonMsg = jsonMsgBuffer.parseObject(stringMsg); // parse message
    inbox.bottomFace = jsonMsg["bFace"];
    uint32_t mID = jsonMsg["mID"];
    inbox.mID = mID;
  }
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
  Serial.printf("Connection Event\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {}

void delayReceivedCallback(uint32_t from, int32_t delay) {}

String generateMessageText(String cmd, uint32_t mID)
{
  Serial.println("RESENDING MESSAGE");
  if (mID == 0) {
    mID = advanceLfsr();
  }
  //====== Generate a blink message =========
  StaticJsonBuffer<512> jsonBuffer; //Space Allocated to construct json instance
  JsonObject& jsonMsg = jsonBuffer.createObject(); // & is "c++ reference"
  //jsonMsg is our output, but in JSON form

  jsonMsg["mID"] =  mID;
  jsonMsg["type"] = "c";
  jsonMsg["sID"] =  SERVER_ID;
  jsonMsg["cmd"] =  cmd;
  String strMsg; // generate empty string
  //strMsg is our output in String form
  jsonMsg.printTo(strMsg); // print to JSON readable string...
  return strMsg;
}

void pushBlinkMessage(int cubeID)
{
  pushMessage(cubeID, "b");
}

void pushForwardMessage(int cubeID)
{
  pushMessage(cubeID, "f");
}

void pushReverseMessage(int cubeID)
{
  pushMessage(cubeID, "r");
}

void pushMessage(int cubeID, String command)
{
  if (outboxisFull())
    return;
  outbox[head].mID = advanceLfsr();
  outbox[head].senderID = SERVER_ID;
  outbox[head].backoff = 0;
  outbox[head].mDeadline = 0;
  outbox[head].cmd = command;
  advanceOutboxHead();
}

// Outbox circular buffer functions
void advanceOutboxHead(){
  outboxHead++;
  if (outboxHead == OUTBOX_SIZE) outboxHead = 0;
}

void advanceOutboxTail(){
  outboxTail++;
  if (outboxTail == OUTBOX_SIZE) outboxTail = 0;
}

bool outboxIsFull(){
  if (outboxHead == outboxTail){
    if (outbox[outboxHead].mID == 0) 
      return false;
    else 
      return true;
  }
  return false;
}

// Inbox circular buffer functions
void advanceInboxHead(){
  inboxHead++;
  if (inboxHead == NUM_CUBES) inboxHead = 0;
}

void advanceInboxTail(){
  inboxTail++;
  if (inboxTail == NUM_CUBES) inboxTail = 0;
}

bool inboxIsFull(){
  if (inboxHead == inboxTail){
    if (inbox[inboxHead].mID == 0) 
      return false;
    else 
      return true;
  }
  return false;
}


// Cube Data Object

// Misc Helper Functions

uint32_t advanceLfsr() // this call returns a message ID. these are not sequential.
{
  static uint32_t lfsr = 0xfefefe;

  uint32_t lsb = lfsr & 0x01u;
  lfsr >>= 1;
  if (lsb)
  {
    lfsr ^= (1u << 31) | (1u << 21) | (1u << 1) | (1u << 0);
  }
  return lfsr;
}

bool calc_delay = false;
SimpleList<uint32_t> nodes;

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
  Serial.println("sendMessage sending: ");
  Serial.println(msg);
  if (recipientID == -1)
  {
    return (mesh.sendBroadcast(msg));
  }
  else
  {
    uint32_t address = getAddressFromCubeID(recipientID);
    return (mesh.sendSingle(address, msg));
  }
}