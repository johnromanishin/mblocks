#ifndef DEFINES
#define DEFINES
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include "Communication.h"

uint32_t getAddressFromCubeID(int CubeID);

#define ARRAY_SIZEOF(x) ((sizeof(x) / sizeof(x[0])))
#define NUM_MESSAGES_TO_BUFFER_INBOX 16
#define SERVER_ID 0

/// ESP ID to CUBE MAP ////
typedef struct EspToCubeMapping
{
  int esp;
  int cube;
} EspToCubeMapping;

int getEspIDFromCube(int);
int getCubeIDFromEsp(int);
void wifiDelay(int delayTime);
#endif
