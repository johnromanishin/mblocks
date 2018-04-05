#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <painlessMesh.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "CBuff.h"
#include "espconn.h"

#define NUM_CUBES 17
#define WINDOW_SIZE 1

//
extern painlessMesh mesh;
extern CircularBuffer<String> jsonCircularBuffer;

//
typedef struct outboxLog
{
  String mContents;
  uint32_t mID;
  uint32_t mDeadline;
  unsigned char backoff;
} outboxLog;

typedef struct inboxLog
{
  String mContents;
  uint32_t mID;
} inboxLog;

typedef struct faceState
{
  char faceID;
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

bool sendMessage(int recipientID, String msg);
void updateBoxes(CircularBuffer<inboxLog>& inbox, CircularBuffer<outboxLog> (&outbox)[NUM_CUBES]);

void initializeWifiMesh();
bool sendBroadcastMessage(String message);
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

void countdown(int numberToSend, int cubeToRelayTo);
void requestStatus(int cubeToRelayTo);

void makeThemBlink(int recipientCube);

#endif

