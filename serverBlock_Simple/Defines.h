#ifndef DEFINES
#define DEFINES
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include "Communication.h"

#define MAGIC_DEBUG 1

uint32_t getAddressFromCubeID(int CubeID);

#define ARRAY_SIZEOF(x) ((sizeof(x) / sizeof(x[0])))
#define SERVER_ID 0

void wifiDelay(int delayTime);
#endif
