#ifndef COMMUNICATION_H 
#define COMMUNICATION_H

#include <painlessMesh.h>         // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoHardware.h>

extern painlessMesh  mesh;

void initializeWifiMesh();
void sendMessage() ;
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

#endif
