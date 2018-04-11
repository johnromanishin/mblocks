#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <painlessMesh.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "CBuff.h"
#include "espconn.h"

#define WINDOW_SIZE 1
#define NUM_CUBES 17
#define NUM_MESSAGES_TO_BUFFER_OUTBOX 4 // This is the max number of messages that can simultaneously 
                                        // fit in the outbox for a given cube.
										// They are sent to the cube one-at-a-time

#define AVERAGE_FIRST_DELAY_MS 100

//

extern painlessMesh mesh;

struct outboxEntry;
struct inboxEntry;

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

String repeatCommand(char cmd, uint32_t mID = 0);
String newBlinkCommand();
String newForwardCommand();
String newReverseCommand();

#endif

