#include "Defines.h"
#include "Communication.h"
#include "espconn.h"
#include <Arduino.h>              // library with basic arduino commands
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh

EspToCubeMapping espCubeMap[] =
{
  {13374829, 98}, // This cube...
  {9086927, 99},  // Other desktop cube...
  {959839, 16},   // PEI BLACK DB:9D:99:1A:BA:23
  {960662, 1},    // PEI BROWN  - F1:E8:71:B2:99:B5
  {959694, 14},   // PEI PURPLE | FA:AA:25:19:C7:DF
  {960558, 10},   // PEI BLUE  f7:AE:59:2B:D9:4D
  {960348, 15},   // PEI GREEN
  {960043, 12},   // PEI YELLOW  CC:F1:4F:AF:64:A8
  {960242, 7},    // PEI ORANGE E6:F6:05:69:08:F2
  {960427, 5},    // PEI RED  D0:D5:6F:CB:32:4C
  //
  {8576514, 9},   // PC BLACK  E3:6B:C6:CE:DA:31
  {959709, 8},    // PC YELLOW  - FB:0D:8F:2C:3B:B4
  {8577103, 11},  // PC ORANGE  - E6:E5:82:26:C7:8B
  {10229112, 2},  // PC PURPLE  - DF:DF:3C:A0:F1:77
  {8577715, 6},   // PC Brown  C5:FF:AB:04:3B:9D
  {15044426, 13}, // PC Blue  D8:9C:4D:EA:27:65
  {8575308, 4},   // PC Green : ED:A6:6A:8E:1B:58  
  //{15044359, 3}: // ORANGE PC RED  CD:2B:5E:AB:3E:F3
  {9086927,   3}     // ESP thing "A" with orange board (magID 3)
};

int getEspIDFromCube(int GlobalCubeID)
{
  int idex;
  int foundit = 0;
  for(idex = 0; idex < (sizeof(espCubeMap) / sizeof(espCubeMap[0])); idex++)
  {
    if(GlobalCubeID == espCubeMap[idex].cube)
    {
      foundit = 1;
      break;
    }
  }

  if(!foundit)
    return -1;
  else
    return espCubeMap[idex].esp;
}

int getCubeIDFromEsp(int espID)
{
  int idex;
  int foundit = 0;
  for(idex = 0; idex < (sizeof(espCubeMap) / sizeof(espCubeMap[0])); idex++)
  {
    if(espID == espCubeMap[idex].esp)
    {
      foundit = 1;
      break;
    }
  }

  if(!foundit)
    return -1;
  else
    return espCubeMap[idex].cube;
}

void wifiDelay(int delayTime)
{
  int millisNow = millis();
  while((millis() - millisNow) < delayTime)
  {
    delay(5);
    mesh.update();
  }
}

