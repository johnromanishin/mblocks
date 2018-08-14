#include "Defines.h"
/*
 * Flags - 
 */
extern bool FOUND_LINE = false;

/*
 * State Machine Variable
 */
extern String GAME = "CUBE";


/*
 * Cube Data Base - Check in Defines.h fore more details
 */
int database[NUM_CUBES][tableWidth] = {};

/*
 * Connection Database - check for more information in Defines.h
 */
int connectionDatabase[Connections][Connection_Parameters] = {};



/* ------------------------------------------------------------------------------------
 * ----------------------------------  Functions --------------------------------------
 * ------------------------------------------------------------------------------------
 */
 
int getCubeIDFromAddress(uint32_t wifiAddress)
/*
 * This functions returns the Wifi Address (long) from the short digit ID number (1-16)
 * The values are from a lookup table in defines.h
 */
{
  switch (wifiAddress) 
  {
    default:
      return (-1);
    case wifiAddress_SERVER:                      
      return (99);
    case wifiAddress_cube00:                       
      return (0);
    case wifiAddress_cube01:
      return (1);
    case wifiAddress_cube02:
      return (2);
    case wifiAddress_cube03:
      return (3);
    case wifiAddress_cube04:
      return (4);
    case wifiAddress_cube05:                       
      return (5);
    case wifiAddress_cube06:
      return (6);
    case wifiAddress_cube07:
      return (7);
    case wifiAddress_cube08:
      return (8);
    case wifiAddress_cube09:
      return (9);
    case wifiAddress_cube10:
      return (10);
    case wifiAddress_cube11:
      return (11);
    case wifiAddress_cube12:
      return (12);
    case wifiAddress_cube13:
      return (13);
    case wifiAddress_cube14:
      return (14);
    case wifiAddress_cube15:
      return (15);
    case wifiAddress_cube16:
      return (16);
  }
}

uint32_t getAddressFromCubeID(int CubeID)
/*
 * This functions returns the Wifi Address (long) from the short digit ID number (1-16)
 * The values are from a lookup table in defines.h
 */
{
  switch (CubeID) 
  { 
    default:
      return (0);
    case 99:                      
      return (wifiAddress_SERVER);
    case 0:                       
      return (wifiAddress_cube00);
    case 1:
      return (wifiAddress_cube01);
    case 2:
      return (wifiAddress_cube02);
    case 3:
      return (wifiAddress_cube03);
    case 4:
      return (wifiAddress_cube04);
    case 5:                       
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
/*
 * This simple function just maps the sets of faces which are 
 * opposites, and returns the one which is opposite to the one we are 
 * interested in.
 */
{
  if(face == 0)
    return(2);
  else if(face == 1)
    return(3);
  else if(face == 2)
    return(0);
  else if(face == 3)
    return(1);
  else if(face == 4)
    return(5);
  else if(face == 5)
    return(4);
  else
    return(-1);
}

void wifiDelay(int delayTime)
/*
 * This function delays for a specific ammount of time while still updating the WIFI
 */
{
  long releaseTime = millis() + delayTime;
  while(releaseTime > millis())
  {
    mesh.update();
  }
}
