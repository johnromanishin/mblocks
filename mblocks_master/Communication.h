#ifndef COMMUNICATION_H 
#define COMMUNICATION_H

#include <painlessMesh.h>  // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <Arduino.h>

#include "Cube.h"
#include "Initialization.h"

extern painlessMesh  mesh;

void initializeWifiMesh();
bool sendMessage(String message);
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

//void checkForMessage(Cube* c, String message);

#endif
