#include "Defines.h"
//#include "espconn.h"
//#include <Arduino.h>              // library with basic arduino commands
//#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh

// Database
int database[NUM_CUBES][tableWidth] = {};
//

uint32_t getAddressFromCubeID(int CubeID)
{
  /*
   * This functions returns the Wifi Address (long) from the short digit ID number (1-16)
   * The values are from a lookup table in defines.h
   */
  switch (CubeID) { // used to be ESP.getChipID
    default:
      return (0);
    case 99:                      //  ESP WIFI  885790061 || Server
      return (wifiAddress_SERVER);
    case 0:                       //  ESP WIFI 2133796284 ||  Test Mblock
      return (wifiAddress_cube00);
    case 1:
      return (wifiAddress_cube01);
    case 2:
      return (wifiAddress_cube02);
    case 3:
      return (wifiAddress_cube03);
    case 4:
      return (wifiAddress_cube04);
    case 5:                       //  960427 PEI RED  D0:D5:6F:CB:32:4C
      return (wifiAddress_cube05);
    case 6:
      return (wifiAddress_cube06);
    case 7:
      return (wifiAddress_cube07);
    case 8:
      return (wifiAddress_cube08);
    case 9:
      return (wifiAddress_cube09);
    case 10:
      return (wifiAddress_cube10);
    case 11:
      return (wifiAddress_cube11);
    case 12:
      return (wifiAddress_cube12);
    case 13:
      return (wifiAddress_cube13);
    case 14:
      return (wifiAddress_cube14);
    case 15:
      return (wifiAddress_cube15);
    case 16:
      return (wifiAddress_cube16);
  }
}

bool areFacesOpposite(int face1, int face2)

/*
 * This function takes two integers, and returns true if they are
 * any of the following pairs, 
 * (0,2), or (2,0)
 * (1,3), or (3,1)
 * (4,5), or (5,4)
 */
{
  if(face1 == oppositeFace(face2))
    return(true);
  else
    return(false);
}

int oppositeFace(int face)
{
  if(face == 0){return(2);}
  else if(face == 1){return(3);}
  else if(face == 2){return(0);}
  else if(face == 3){return(1);}
  else if(face == 4){return(5);}
  else if(face == 5){return(4);}
  else{return(-1);}
}

void wifiDelay(int delayTime)
{
  long releaseTime = millis() + delayTime;
  while(releaseTime > millis())
  {
    mesh.update();
  }
}
