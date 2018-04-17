#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <painlessMesh.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "CBuff.h"
#include "espconn.h"

#define WINDOW_SIZE 1
#define NUM_CUBES 17
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
  int bottomFace;
  int faceStates[6];
};


extern painlessMesh mesh;
extern int cubesState[6];

///
bool sendMessage(int recipientID, String msg);
uint32_t advanceLfsr();

void initializeBoxes();
void initializeOutBoxes();
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

void advanceOutboxHead();
void advanceOutboxTail();
bool outboxIsFull();
void advanceInboxHead();
void advanceInboxTail();
bool inboxIsFull();
void initializeOutboxes();

void updateStateModel();

#endif

