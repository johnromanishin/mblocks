/*
   This Contains Three Main Sections
   0. Database Matching cubeID's to WIFI ID's
   1. Misc. Global Variables
   2. Motion Definitions
   3. Color Definitions
   4. PlaneEnums
*/

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
Motion traverse_F     = {"traverse"   , true    , 6000            , 6000          , 3000              , 12            , 9         , "f"};
Motion traverse_R     = {"traverse"   , true    , 6000            , 6000          , 3000              , 12            , 9         , "r"};

Motion roll_F         = {"roll"       , false   , 6500            ,    0          ,    0              , 0             , 1         , "f"};
Motion roll_R         = {"roll"       , false   , 6500            ,    0          ,    0              , 0             , 1         , "r"};

Motion cornerClimb_F  = {"cornerClimb", true    , 15500           , 7000          , 3300              , 12            , 250       , "f"};
Motion cornerClimb_R  = {"cornerClimb", true    , 15500           , 7000          , 3300              , 12            , 250       , "r"};

Motion shake_F        = {"shake"      , false   , 4500            , 4000          , 3500              , 20            , 250       , "f"};
Motion softShake_F    = {"softShake"  , false   , 3500            , 3000          , 3000              , 20            , 250       , "f"};

Motion explode_F      = {"explode"    , false   , 15500           , 6000          , 5000              , 30            , 250       , "f"};
Motion explode_R      = {"explode"    , false   , 15500           , 6000          , 5000              , 30            , 250       , "r"};
//
Motion rollDouble_F   = {"double"     , false   , 15500           , 3000          , 3000              , 5             , 250       , "f"};
Motion rollDouble_R   = {"double"     , false   , 15500           , 3000          , 3000              , 5             , 250       , "r"};

//  List of possible colors;
Color red =     {1, 0, 0};
Color green =   {0, 1, 0};
Color blue =    {0, 0, 1};
Color yellow =  {1, 1, 0};
Color purple =  {1, 0, 1};
Color teal  =   {0, 1, 1};
Color white =   {1, 1, 1};
Color off   =   {0, 0, 0};

int thisCubeID = 1;
extern String Game = "nothing";
extern int GlobalplaneChangeTime;
extern int GlobalplaneChangeRPM;
extern int GlobalPlaneAccel = 2100;
extern bool magicTheLight = false;
extern int magicVariable = 0;
extern int MAGIC_DEBUG = 0;
extern int magicFace = 0;
extern int magicVariable_0 = 0;
extern int magicVariable_1 = 0;
extern int GlobalMaxAccel = 6000;

// Defaults for motion values
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

uint32_t getAddressFromCubeID(int CubeID)
{
  switch (CubeID) { // used to be ESP.getChipID
    case 0:                   //  ESP 13374829 || WIFI  885790061 || Server
      return (885790061);
    case 99:                  //  ESP  9086927 || WIFI 2139793359 ||  Test Mblock
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

/**
   For the ith face, faceRotations[i][0] will tell which face the intrisic arrow points towards.
   faceRotations[i][1] tells which face the arrow rotated 90 degrees counter-clockwise from the
   intrinsic arrow points towards.  face[rotations[i][2] give the 180-degree face.

   For cubes
*/

PlaneEnum returnPlane(int face1, int face2)
{
  return (facePlanes[face1][face2]);
}

int faceArrowPointsTo(int readingFace, int connectionAngle)
{
  return (faceRotations[readingFace][connectionAngle]);
}

//    Face you want to move towards | other face in plane

/*
   This lookup table tells the module to either move Clockwise (+1)
   or Counter-clockwise (-1) or invalid selection (0).

   Useage: First arguement is the face we want to move TOWARDS
           Second face is the face we are moving FROM
*/
const int faceClockinessMatrix[FACES][FACES] =
{
  //0,   1,   2,   3,   4,  5 // This is the face we want to move towards
  { 0,   1,   0,  -1,  -1,   1},   // 0
  { -1,   0,   1,   0,   1,  -1},  // 1
  { 0,  -1,   0,   1,   1,  -1},   // 2
  { 1,   0,  -1,   0,  -1,   1},   // 3
  { 1,  -1,  -1,   1,   0,   0},   // 4
  { -1,   1,   1,  -1,   0,   0}   // 5
};

int faceClockiness(int faceTowards, int faceReference)
{
  int result = 0;
  if ((faceTowards > -1) && (faceTowards < 6))
  {
    if ((faceReference > -1) && (faceReference < 6))
    {
      result = faceClockinessMatrix[faceReference][faceTowards];
    }
  }
  return (result);
}
// PLANE0123, PLANE0425, PLANE1453, PLANENONE
// PLANE0123, PLANE0425, PLANE1453, PLANENONE
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

String planeEnumToString(PlaneEnum thePlaneNow)
{
  String resultString = "";
  if (       thePlaneNow == PLANE0123)   {
    resultString = "PLANE0123";
  }
  else if ( thePlaneNow == PLANE0425)   {
    resultString = "PLANE0425";
  }
  else if ( thePlaneNow == PLANE1453)   {
    resultString = "PLANE1453";
  }
  else if ( thePlaneNow == PLANENONE)   {
    resultString = "PLANENONE";
  }
  else if ( thePlaneNow == PLANEMOVING) {
    resultString = "PLANEMOVING";
  }
  else if ( thePlaneNow == PLANEERROR)  {
    resultString = "PLANEERROR";
  }
  return (resultString);
}

PlaneEnum stringToPlaneEnum(String inputString)
{
  PlaneEnum resultPlaneEnum = PLANEERROR;
  if (       inputString == "PLANE0123")   {
    resultPlaneEnum = PLANE0123;
  }
  else if ( inputString == "PLANE0425")   {
    resultPlaneEnum = PLANE0425;
  }
  else if ( inputString == "PLANE1453")   {
    resultPlaneEnum = PLANE1453;
  }
  else if ( inputString == "PLANENONE")   {
    resultPlaneEnum = PLANENONE;
  }
  else if ( inputString == "PLANEMOVING") {
    resultPlaneEnum = PLANEMOVING;
  }
  else if ( inputString == "PLANEERROR")  {
    resultPlaneEnum = PLANEERROR;
  }
  return (resultPlaneEnum);
}

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
