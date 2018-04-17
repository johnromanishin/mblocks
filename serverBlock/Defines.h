#ifndef DEFINES
#define DEFINES
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include "Communication.h"

uint32_t getAddressFromCubeID(int CubeID);

#define ARRAY_SIZEOF(x) ((sizeof(x) / sizeof(x[0])))
#define SERVER_ID 99
#define TESTCUBE_ID 0
#define OUTBOX_SIZE 10
#define INBOX_SIZE 20

bool areFacesOpposite(int face1, int face2);
int oppositeFace(int face);


void wifiDelay(int delayTime);
#endif
