#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <painlessMesh.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "CBuff.h"
#include "espconn.h"

#define NUM_CUBES 17
#define NUM_MESSAGES_TO_BUFFER_OUTBOX 4 // This is the max number of messages that can simultaneously 
                                        // fit in the outbox for a given cube.
										// They are sent to the cube one-at-a-time

#define AVERAGE_FIRST_DELAY_MS 100

//

extern painlessMesh mesh;
extern CircularBuffer<String> jsonCircularBuffer;

struct outboxEntry;
struct inboxEntry;

bool sendMessage(int recipientID, String msg);
void updateBoxes(CircularBuffer<inboxEntry> &inbox, CircularBuffer<outboxEntry> (&outbox)[NUM_CUBES]);

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

