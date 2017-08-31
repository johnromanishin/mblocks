#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal - used to communicate with all of the sensors on the Mblocks
#include <painlessMesh.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "Defines.h"
#include "CommandParser.h"
#include "Commands.h"

painlessMesh mesh;

void receivedCallback(uint32_t from, String & msg) 
{
  // Decode recieved JSON
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) 
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());

  SimpleList<uint32_t> nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}

void setup()
{
  Serial.begin(115200);
  
  // Initialize callbacks for the mesh.
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);
  randomSeed(analogRead(A0));

  Serial.printf("Initializing\r\n");
  
  // Parse commands from the user
  char serial_decode_space[1024];
  serial_decoder_buffer_t buf = {serial_decode_space, 1024, 0};
  state_t s = {&mesh};
  while(1)
  {
    checkSerialAndCall(&buf, commands, NUMCMDS, &s);
    delay(1);
  }
}

void loop(){}
