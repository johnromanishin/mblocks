#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <painlessMesh.h>  // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <Arduino.h>
#include <ArduinoJson.h>
#include "CBuff.h"
#include "espconn.h"

#define NUM_CUBES 17
#define WINDOW_SIZE 1

//

extern painlessMesh mesh;
extern CircularBuffer<String, true> jsonCircularBuffer;

bool sendMessage(int recipientID, String msg);
void updateBoxes(CircularBuffer<inboxLog>& inbox, CircularBuffer<outboxLog>& outbox[])

void initializeWifiMesh();
bool sendBroadcastMessage(String message);
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

void countdown(int numberToSend, int cubeToRelayTo);
void requestStatus(int cubeToRelayTo);
void messageSingle(uint32_t dest);

void makeThemBlink(int recipientCube);

//void checkForMessage(Cube* c, String message);

#endif

