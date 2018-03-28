#ifndef DEFINES
#define DEFINES
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include "Communication.h"

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
