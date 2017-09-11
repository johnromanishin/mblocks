#include "Defines.h"
#include "Communication.h"

int faceVersion = 1;
int cubeID = 0;
// typedef enum PlaneEnum {PLANE0123, PLANE0425, PLANE1453, PLANENONE, PLANEMOVING, PLANEERROR} PlaneEnum;
int GlobalplaneChangeTime = 60;
int GlobalplaneChangeRPM = 5000;

int traverseBrakeCurrent_F = 2800;
int traverseBrakeCurrent_R = 2800;
int cornerClimbBrakeCurrent_F = 3000;
int cornerClimbBrakeCurrent_R = 3000;

EspToCubeMapping espCubeMap[] =
{
  {960242,    07},  // 
  //{959839,    10},  // 
  {9      ,   99},  // PEI ||  ORANGE  || E6:F6:05:69:08:F2 01  || 
  {959694,    99},  // PEI ||  BLUE    || FA:AA:25:19:C7:DF 01  ||  WIFI doesn't work great...
  {959839,    14},  // PEI ||  PURPLE  || DB:9D:99:1A:BA:23 01  ||
  {15044426,  16},  // PEI ||  BLACK   ||                       ||  Face5 doesn't work great
  {960348  ,  15},  // PEI ||  GREEN   || EC:47:A9:35:1F:02 01  || WORKS great!
  {10229112,  05},  // PEI ||  RED     || DF:DF:3C:A0:F1:77 01  || Works well
  {1       ,  11},  // PEI ||  BROWN   ||
  {99      ,  22},  // PEI ||  YELLOW  || 
  {13374829,  99}   // ESP THING 

};

int getEspIDFromCube(int cubeID)
{
  int idex;
  int foundit = 0;
  for(idex = 0; idex < (sizeof(espCubeMap) / sizeof(espCubeMap[0])); idex++)
  {
    if(cubeID == espCubeMap[idex].cube)
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

/**
 * For the ith face, faceRotations[i][0] will tell which face the intrisic arrow points towards.
 * faceRotations[i][1] tells which face the arrow rotated 90 degrees counter-clockwise from the
 * intrinsic arrow points towards.  face[rotations[i][2] give the 180-degree face.
 *
 * For cubes
 */

PlaneEnum returnPlane(int face1, int face2)
{
  return(facePlanes[face1][face2]);
}

int faceArrowPointsTo(int readingFace, int connectionAngle)
{
  return(faceRotations[readingFace][connectionAngle]);
}

// PLANE0123, PLANE0425, PLANE1453, PLANENONE
//PLANE0123, PLANE0425, PLANE1453, PLANENONE
// 0--------,1---------,2---------,3---------,4---------,5---------
const PlaneEnum facePlanes[FACES][FACES] =
{
  { PLANENONE, PLANE0123, PLANENONE, PLANE0123, PLANE0425, PLANE0425},   // 0
  { PLANE0123, PLANENONE, PLANE0123, PLANENONE, PLANE1453, PLANE1453},   // 1
  { PLANENONE, PLANE0123, PLANENONE, PLANE0123, PLANE0425, PLANE0425},   // 2
  { PLANE0123, PLANENONE, PLANE0123, PLANENONE, PLANE1453, PLANE1453},   // 3
  { PLANE0425, PLANE1453, PLANE0425, PLANE1453, PLANENONE, PLANENONE},   // 4
  { PLANE0425, PLANE1453, PLANE0425, PLANE1453, PLANENONE, PLANENONE}    // 5
};

 
const int faceRotations[FACES][4] =
{
// 0  1  2  3 ---------ANGLES---
  {5, 1, 4, 3},  // FACE 0
  {5, 2, 4, 0},  // FACE 1 
  {1, 5, 3, 4},  // FACE 2
  {5, 0, 4, 2},  // FACE 3
  {2, 3, 0, 1},  // FACE 4
  {1, 0, 3, 2}   // FACE 5
};
