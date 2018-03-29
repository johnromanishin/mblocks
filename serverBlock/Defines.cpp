#include "Defines.h"
#include "Communication.h"
#include "espconn.h"
#include <Arduino.h>              // library with basic arduino commands
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh

EspToCubeMapping espCubeMap[] =
{
  {0, 98},            // This cube...
  {2139793359, 99},   // Other desktop cube...
  {0, 16},            // PEI BLACK        DB:9D:99:1A:BA:23
  {0, 1},             // PEI BROWN        F1:E8:71:B2:99:B5
  {0, 14},            // PEI PURPLE       FA:AA:25:19:C7:DF
  {0, 10},            // PEI BLUE         f7:AE:59:2B:D9:4D
  {0, 15},            // PEI GREEN
  {0, 12},            // PEI YELLOW       CC:F1:4F:AF:64:A8
  {0, 7},             // PEI ORANGE       E6:F6:05:69:08:F2
  {2131666859, 5},    // PEI RED          D0:D5:6F:CB:32:4C
  //
  {0, 9},             // PC BLACK         E3:6B:C6:CE:DA:31
  {0, 8},             // PC YELLOW        FB:0D:8F:2C:3B:B4
  {0, 11},            // PC ORANGE        E6:E5:82:26:C7:8B
  {0, 2},             // PC PURPLE        DF:DF:3C:A0:F1:77
  {0, 6},             // PC Brown         C5:FF:AB:04:3B:9D
  {0, 13},            // PC Blue          D8:9C:4D:EA:27:65
  {0, 4},             // PC Green         ED:A6:6A:8E:1B:58  
  //{15044359, 3}:    // ORANGE PC RED   CD:2B:5E:AB:3E:F3  // jfc
  {0,   3}            // ESP thing "A" with orange board (magID 3)
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

