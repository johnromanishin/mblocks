#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <painlessMesh.h>         // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoJson.h>
#include "Communication.h"
#include "Cube.h"
#include "espconn.h"
#include "EspToCubeMapping.h"
#include "CBuff.h"

#define   BLINK_PERIOD    3000000 // microseconds until cycle repeat
#define   BLINK_DURATION  100000  // microseconds LED is on for
#define   MESH_SSID       "mblocks3d"
#define   MESH_PASSWORD   "mblocks3d"
#define   MESH_PORT       5555

painlessMesh  mesh;

extern Cube c;
String jsonBufferSpace[16];
CircularBuffer<String> jsonBuffer(16, jsonBufferSpace);

bool calc_delay = false;
SimpleList<uint32_t> nodes;
void sendMessage() ; // Prototype

void initializeWifiMesh()
{
    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
    mesh.onNodeDelayReceived(&delayReceivedCallback);
    randomSeed(analogRead(A0));
}

void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  bool error = mesh.sendBroadcast(msg + " myFreeMemory: " + String(ESP.getFreeHeap()));

  if (calc_delay) {
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      mesh.startDelayMeas(*node);
      node++;
    }
    calc_delay = false;
  }
}


void receivedCallback(uint32_t from, String & msg) 
{
  // Put the json message into the buffer
  jsonBuffer.push(msg);
}

void newConnectionCallback(uint32_t nodeId) 
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());

  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}

//   Serial.println(c.returnTopFace());
//   Serial.print("IMU_ax: ");Serial.println(c.axFrameBuffer.access(0));
//   Serial.print("IMU_ay: ");Serial.println(c.ayFrameBuffer.access(0));
//   Serial.print("IMU_az: ");Serial.println(c.azFrameBuffer.access(0));
   //Serial.print("IMU_gx: ");Serial.println(c.gxFrameBuffer.access(0));
   //Serial.print("IMU_gy: ");Serial.println(c.gyFrameBuffer.access(0));
   //Serial.print("IMU_gz: ");Serial.println(c.gzFrameBuffer.access(0));
   
   // c.lightFace(c.returnTopFace());
   // delay(100);
   
//
 //c.updateBothIMUs();
   //c.updateCoreMagnetSensor();
   
//   String newmsg = "Angle: " + String(c.coreMagnetAngleBuffer.access(0) - initialMagnetReadingOffset)
//   + " core.ax: " + String(c.axCoreBuffer.access(0))
//   + " core.ay: " + String(c.ayCoreBuffer.access(0))
//   + " core.az: " + String(c.azCoreBuffer.access(0))
//   + " Frame.ax: " + String(c.axFrameBuffer.access(0))
//   + " Frame.ay: " + String(c.ayFrameBuffer.access(0))
//   + " Frame.az: " + String(c.azFrameBuffer.access(0))
//   + " CoreMagAGC: " + String(c.coreMagnetStrengthBuffer.access(0));
   
   //Serial.print("Angle: ");Serial.print(c.coreMagnetAngleBuffer.access(0));Serial.print(" Magnitude: ");Serial.println(c.coreMagnetStrengthBuffer.access(0));
   //String newmsg = "Angle: ";
   //mesh.sendBroadcast(newmsg);
