#include "Defines.h"
#include "Communication.h"

//typedef struct Motion
//{
//  String moveName;      // name of move, used when we instantiate the type of movement
//  bool brake; // true = use mechanical brake  || false = use the electronic brake only
//  int rpm;              // RPM that we 
//  int timeout;       // time that we will wait untill for confirmation of the speed
//  int current;     // Current we apply to the brake in mA ... Maximum is 6000 ma
//  int brakeTime;        // Time (milli Seconds) we apply the brake for Maximum time*current < some value
//  int difficulty;     // Estimated difficulty of the move on a scale 1-255; with 1 == easy, 255 == very hard;
//  String for_rev;
//} Motion;

//                      moveName      , brake   , rpm             , timout        , current           , brakeTime     , difficult , for_rev
Motion traverse_F     = {"traverse"   , true    , 6000            , 6000          , 3000              , 12            , 9         ,"f"};
Motion traverse_R     = {"traverse"   , true    , 6000            , 6000          , 3000              , 12            , 9         ,"r"};

Motion roll_F         = {"roll"       , false   , 6500            ,    0          ,    0              , 0             , 1         ,"f"};
Motion roll_R         = {"roll"       , false   , 6500            ,    0          ,    0              , 0             , 1         ,"r"};

Motion cornerClimb_F  = {"cornerClimb", true    , 15500           , 7000          , 3300              , 12            , 250       ,"f"};
Motion cornerClimb_R  = {"cornerClimb", true    , 15500           , 7000          , 3300              , 12            , 250       ,"r"};

Motion shake_F        = {"shake"      , false   , 4500            , 4000          , 3500              , 20            , 250       ,"f"};
Motion softShake_F    = {"softShake"  , false   , 3500            , 3000          , 3000              , 20            , 250       ,"f"};


Motion explode_F      = {"explode"    , false   , 15500            , 6000         , 5000              , 30            , 250       ,"f"};
Motion explode_R      = {"explode"    , false   , 15500            , 6000         , 5000              , 30            , 250       ,"r"};
//

Motion rollDouble_F      = {"double"    , false   , 15500            , 3000         , 3000              , 5            , 250       ,"f"};
Motion rollDouble_R      = {"double"    , false   , 15500            , 3000         , 3000              , 5            , 250       ,"r"};

//  List of possible colors;
Color red =     {1,0,0};
Color green =   {0,1,0};
Color blue =    {0,0,1};
Color yellow =  {1,1,0};
Color purple =  {1,0,1};
Color teal  =   {0,1,1};
Color white =   {1,1,1};
Color off   =   {0,0,0};

int faceVersion = 1;
int GlobalCubeID = 0;
// typedef enum PlaneEnum {PLANE0123, PLANE0425, PLANE1453, PLANENONE, PLANEMOVING, PLANEERROR} PlaneEnum;
//int GlobalplaneChangeTime = 60;
//int GlobalplaneChangeRPM = 5000;
extern int GlobalplaneChangeTime;
extern int GlobalplaneChangeRPM;
extern int GlobalPlaneAccel = 2100;
extern bool magicTheLight = false;
extern int magicVariable = 0;
extern int MAGIC_DEBUG = 0;
extern int magicFace = 0;
extern int magicVariable_0 = 0;
extern int magicVariable_1 = 0;
extern int magicVariable_2 = 0;
extern int magicVariable_3 = 0;
extern int magicVariable_4 = 0;
extern int magicVariable_5 = 0;
extern int GlobalMaxAccel = 6000;


extern int TRAVERSE_RPM_F = 6500;
extern int TRAVERSE_RPM_R = 6500;
extern int TRAVERSE_CURRENT_F = 2500;
extern int TRAVERSE_CURRENT_R = 2500;

extern int CC_RPM_F = 15500;
extern int CC_RPM_R = 15500;
extern int CC_CURRENT_F = 3000;
extern int CC_CURRENT_R = 3000;
extern int CC_BRAKETIME_F = 12;
extern int CC_BRAKETIME_R = 12;

EspToCubeMapping espCubeMap[] =
{
  {960662, 1}, // PEI BROWN  - F1:E8:71:B2:99:B5
  
  //
  {960348,    15},  // PEI ||  GREEN   || EC:47:A9:35:1F:02 01  || WORKS great!
  {959694,    14},  // PEI ||  PURPLE  || FA:AA:25:19:C7:DF 01  || SPINS VERY FREELY
  {960242,    07},  // 
  {9      ,   99},  // PEI ||  ORANGE  || E6:F6:05:69:08:F2 01  || 
  {15044426,  16},  // PEI ||  BLACK   ||                       ||  Face5 doesn't work great
  {10229112,  05},  // PEI ||  RED     || DF:DF:3C:A0:F1:77 01  || Works well
  {1       ,  11},  // PEI ||  BROWN   || CC:F1:4F:AF:64:A8 01  || OLD FRAME
  {99      ,  22},  // PEI ||  YELLOW  || 
  ////==============================
  {13374829,  99},   // ESP THING 
  {9086927,   3}     // ESP thing "A" with orange board (magID 3)
};
//{959839,    10},  // 
//{959694,    99},  // PEI ||  BLUE    || FA:AA:25:19:C7:DF 01  ||  WIFI doesn't work great..


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

//                      Face you want to move towards | other face in plane

/*
 * This lookup table tells the module to either move Clockwise (+1) 
 * or Counter-clockwise (-1) or invalid selection (0).
 * 
 * Useage: First arguement is the face we want to move TOWARDS
 *         Second face is the face we are moving FROM
 */
const int faceClockinessMatrix[FACES][FACES] =
{
  //0,   1,   2,   3,   4,  5 // This is the face we want to move towards
  { 0,   1,   0,  -1,  -1,   1},   // 0
  {-1,   0,   1,   0,   1,  -1},   // 1
  { 0,  -1,   0,   1,   1,  -1},   // 2
  { 1,   0,  -1,   0,  -1,   1},   // 3
  { 1,  -1,  -1,   1,   0,   0},   // 4
  {-1,   1,   1,  -1,   0,   0}    // 5
};

int faceClockiness(int faceTowards, int faceReference)
{
  int result = 0;
  if((faceTowards > -1) && (faceTowards < 6))
  {
    if((faceReference > -1) && (faceReference < 6))
    {
      result = faceClockinessMatrix[faceReference][faceTowards];
    }
  }
  return(result);
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
