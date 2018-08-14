#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <painlessMesh.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "espconn.h"
#include "Defines.h"
#include <ESP8266WiFi.h>


/*
 * The Window size is the number of messages that send at a time
 */
#define WINDOW_SIZE 1

/*
 * This is the average delay between when we send out a message a second time
 */
#define AVERAGE_FIRST_DELAY_MS 300

/*
 * This creates the class for the WIFI mesh network called mesh,
 * this actually manages the network at a low level, and sends and recieves messages
 */
extern painlessMesh mesh;

/*
 * An "outboxEntry" is basically a message, and some timing related information
 * This also manages the mID - a randomly determined unique number to identify the message
 * as well as a count of the number of times we have sent the message
 */
struct outboxEntry
{
  uint32_t mID;
  int senderID;
  String cmd;
  uint32_t mDeadline;
  unsigned char backoff;
};

/*
 * This struct stores incomming messages
 */
struct inboxEntry
{
  uint32_t mID;
  int senderID;
};

/* --------------------------------------------------------------------
 * ----------------------- WIFI FUNCTIONS -----------------------------
 * --------------------------------------------------------------------
 */
 
void initializeWifiMesh();
/*
 * This function starts up the Wifi Mesh
 */

void receivedCallback(uint32_t from, String & msg);
/*
 * This function is called when the server recieves a message. It is currently
 * set up to update the database from this callback, which might be a bad idea, but it is what we
 * do...
 */

void newConnectionCallback(uint32_t nodeId);
/*
 * This is called every time the mesh network adds a node...
 */

void changedConnectionCallback();
/*
 * This callback is activated when a cube either leaves or joins the mesh network
 */

void nodeTimeAdjustedCallback(int32_t offset);
/*
 * One more call back
 */

void delayReceivedCallback(uint32_t from, int32_t delay);
/*
 * Another call back
 */

bool sendMessage(int recipientID, String msg);
/*
 * This function actually sends a message on the wifi Mesh
 * It is intended to work with "generateMessageText" command
 * which formats the message into json.
 */

/* --------------------------------------------------------------------
 * ----------------------- INBOX/OUTBOX FUNCTIONS ---------------------
 * --------------------------------------------------------------------
 */

void initializeBoxes();
/*
 * This function sets the initial values for the inbox and outbox
 */

uint32_t advanceLfsr();
/*
 * This is a linear shift register, and is essentially a fancy way to generate
 * unique random numbers which are then assigned as the new mID's which are added to the 
 * cubes.
 */

void updateBoxes();
/*
 * This is the main function which processes the inboxes and outboxes. It should be called
 * regularily to make sure everything is being manages correctly.
 */

String generateMessageText(String cmd, uint32_t mID);
/*
 * This function creates a JSON formatted string which is ready to be actually sent to the mesh network
 */
 
/*
 * These functions push a specific type of message, and exist so that the intent of the message
 * is clear to the programmer
 */
void pushMessage(int cubeID, String command);

void pushBlinkMessage(int cubeID);
void pushForwardMessage(int cubeID);
void pushReverseMessage(int cubeID);
void pushStatusMessage(int cubeID);
void pushSleepMessage(int cubeID);

/*
 * These functions are designed to manage the indexing in the Outboxes and Inboxes
 * which are implemented as circular buffers.
 */
void advanceOutboxHead(int cubeID);

void advanceOutboxTail(int cubeID);

bool outboxIsFull(int cubeID);

void advanceInboxHead();

void advanceInboxTail();

bool inboxIsFull();

bool inboxIsEmpty();

#endif
