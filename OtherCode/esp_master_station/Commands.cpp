#include "Commands.h"
#include "Defines.h"
#include "CommandParser.h"
#include "EspToCubeMapping.h"
#include <stdint.h>
#include <painlessMesh.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>

////////////////////////////////////////////////
//        prototypes                          //
////////////////////////////////////////////////
void stub(int argc, char** argv, void* user);
void help(int argc, char** argv, void* user);
void printConnectedCubeIDs(int argc, char** argv, void* user);
void printConnectedIDs(int argc, char** argv, void* user);
void blinkCubeID(int argc, char** argv, void* user);

const command_t commands[] = 
{
  {"help",
   "No arguments",
   help},
   
  {"print-connected-cube-ids",
   "No arguments",
   printConnectedCubeIDs},
   
  {"print-connected-ids",
   "No arguments",
   printConnectedIDs},
   
  {"blink-cube-id",
   "<cube ID>\r\n"
   "example: blink-cube-id 1",
   blinkCubeID},

  {"clear-all-arrows",
   "No arguments",
   
  
  {"set-virtual-arrow",
   "<cube ID> <face number> <direction [0, 3]>\r\n"
   "example: set-virtual-arrow 13 2 1",
   stub}
};

static uint32_t my_atoi(const char* c)
{
  uint32_t result = 0;
  while((*c != '\0') && (*c >= '0') && (*c <= '9'))
  {
    result *= 10;
    result += *c - '0';
    c++;
  }
  return result;
}

void stub(int argc, char** argv, void* user)
{
  Serial.print("Uh-oh!  It looks like command ");
  Serial.print(argv[0]);
  Serial.print(" hasn't been implemented yet!\r\n");
}

void help(int argc, char** argv, void* user)
{
  Serial.println("The following commands are available: ");
  for(int i = 0; i < (sizeof(commands) / sizeof(commands[0])); i++)
  //for(int i = 0; i < 5; i++);
  {
    Serial.println(commands[i].name);
    Serial.print("args: ");
    Serial.println(commands[i].arghelp);
    Serial.println();
  }
}

void printConnectedCubeIDs(int argc, char** argv, void* user)
{
  state_t* s = (state_t*)user;
  
  SimpleList<uint32_t> nodes = s->mesh->getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:\r\n");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf("Cube %u\r\n", getCubeIDFromEsp(*node & 0xffffffu));
    node++;
  }
  Serial.println();
}

void printConnectedIDs(int argc, char** argv, void* user)
{
  state_t* s = (state_t*)user;
  
  SimpleList<uint32_t> nodes = s->mesh->getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}

void blinkCubeID(int argc, char** argv, void* user)
{
  if(argc != 2)
  {
    Serial.println("Wrong number of arguments.  Expected 2.  Type \'help\' for help.");
    return;  
  }

  state_t* s = (state_t*)user;
  uint32_t targetID = my_atoi(argv[1]);
  Serial.printf("Sending message to ESP ID %u\r\n", targetID);

  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["type"] = "cmd";
  root["cubeID"] = targetID;
  root["cmd"] = "blinkOnce";
  
  String str;
  root.printTo(str);
  s->mesh->sendBroadcast(str);

  //check and see if that cube is present on the network and print a warning if it isn't
  int foundit = 0;
  SimpleList<uint32_t> nodes = s->mesh->getNodeList();
  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    if(getCubeIDFromEsp(*node & 0xffffffu) == targetID)
      foundit = 1;
    node++;
  }
  if(foundit == 0)
    Serial.printf("Warning: it looks like cube ID %i isn't in the mesh network.\r\n", targetID);
}

void clearAllArrows(int argc, char** argv, void* user)
{
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["type"] = "cmd";
  root["cubeID"] = -1;
  root["cmd"] = "blinkOnce";
  
  String str;
  root.printTo(str);
  s->mesh->sendBroadcast(str);

}

