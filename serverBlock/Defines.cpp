#include "Defines.h"
#include "Communication.h"
#include "espconn.h"
#include <Arduino.h>              // library with basic arduino commands
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh

uint32_t getAddressFromCubeID(int CubeID)
{
  switch (CubeID) { // used to be ESP.getChipID
    case 99:                   //  ESP WIFI  885790061 || Server
      return (885790061);
    case 0:                  //  ESP WIFI 2133796284 ||  Test Mblock
      return (2133796284);
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:                   //  960427 PEI RED  D0:D5:6F:CB:32:4C
      return (2131666859);
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
    case 9:
      break;
    case 10:
      break;
    case 11:
      break;
    case 12:
      break;
    case 13:
      break;
    case 14:
      break;
    case 15:
      break;
    case 16:
      break;
  }
}

void wifiDelay(int delayTime)
{
  long releaseTime = millis() + delayTime;
  while(releaseTime > millis())
  {
    mesh.update();
  }
}
