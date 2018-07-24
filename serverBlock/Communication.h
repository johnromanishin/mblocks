#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <painlessMesh.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "espconn.h"
#include "Defines.h"


#define WINDOW_SIZE 1
#define AVERAGE_FIRST_DELAY_MS 100

struct outboxEntry
{
  uint32_t mID;
  int senderID;
  String cmd;
  uint32_t mDeadline;
  unsigned char backoff;
};

struct inboxEntry
{
  uint32_t mID;
  int senderID;
  int bottomFace;
  int faceStates[6];
};

void updateStateModel(int cubeID);

extern painlessMesh mesh;
//extern int cubesState[NUM_CUBES][6];

///
bool sendMessage(int recipientID, String msg);
uint32_t advanceLfsr();

void initializeBoxes();
void updateBoxes();

void initializeWifiMesh();
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

String generateMessageText(String cmd, uint32_t mID);
void pushMessage(int cubeID, String command);

void pushBlinkMessage(int cubeID);
void pushForwardMessage(int cubeID);
void pushReverseMessage(int cubeID);
void pushStatusMessage(int cubeID);
void pushSleepMessage(int cubeID);
void pushColorMessage(int cubeID, char Color);


void advanceOutboxHead(int cubeID);
void advanceOutboxTail(int cubeID);
bool outboxIsFull(int cubeID);

void advanceInboxHead();
void advanceInboxTail();
bool inboxIsFull();
bool inboxIsEmpty();
#endif
