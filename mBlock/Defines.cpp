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

extern int bFace = -1;
extern int fFace = -1;
extern int f0 = -1;
extern int f1 = -1;
extern int f2 = -1;
extern int f3 = -1;
extern int f4 = -1;
extern int f5 = -1;

extern int ARROW_OFFSETS[CUBES][FACES] = 
{
  {0,0,0,0,0,0},  //Cube0
  {0,0,0,0,0,0},  //Cube1
  {0,0,0,0,0,0},  //Cube2
  {0,0,0,0,0,0},  //Cube3
  {0,0,0,0,0,0},  //Cube4
  {0,0,0,0,0,0},  //Cube5
  {0,0,0,0,0,0},  //Cube6 
  {1,2,3,4,1,2},  //Cube7 Orange PEI
  {0,0,0,0,0,0},  //Cube8
  {0,0,0,0,0,0},  //Cube9
  {0,0,0,0,0,0},  //Cube10
  {0,0,0,0,0,0},  //Cube11
  {0,0,0,0,0,0},  //Cube12
  {0,0,0,0,0,0},  //Cube13
  {0,0,0,0,0,0},  //Cube14
  {0,0,0,0,0,0},   //Cube15
  {0,0,0,0,0,0}   //Cube16
};

int thisCubeID = 0;
String Game = "NOTHING";
extern bool HALF_LIGHT = false;

/*
 * 
 */
extern int LIGHT_TRANSITIONS = 0;
extern int TOP_FACE_LIGHT[TOP_LIGHT_SAMPLES] = {};
extern int TOTAL_LIGHT = 0;
extern int DIRECTION = 1;
extern long timeLightTurnedOn = 0;


/*
 * Global variables related to GRID
 */
extern bool LIGHT_TOGGLE_STATE = false;
extern int  LIGHT_TOGGLE_TIME = 0;


/*
 * GLobal Variables that are Flags...
 */
bool PART_OF_LINE = false;
bool MAGIC_THE_LIGHT = false;
bool DOUBLE_CHECK = true;
extern bool THE_CHOSEN_ONE = false;

/*
 * 
 */
extern int FACES_LIGHTS[FACES] = {0,0,0,0,0,0};

//typedef struct Motion
//{
//  String moveName;      // name of move, used when we instantiate the type of movement
//  bool brake;           // true = use mechanical brake  || false = use the electronic brake only
//  int rpm;              // RPM that we
//  int timeout;          // time that we will wait untill for confirmation of the speed
//  int current;          // Current we apply to the brake in mA ... Maximum is 6000 ma
//  int brakeTime;        // Time (milli Seconds) we apply the brake for Maximum time*current < some value
//  int difficulty;       // Estimated difficulty of the move on a scale 1-255; with 1 == easy, 255 == very hard;
//  String for_rev;
//} Motion;

//                          moveName      , brake   , rpm     , timout  , current , brakeTime , neighbors   , for_rev
Motion traverse_F         = {"traverse"   , true    , 6000    , 6000    , 3000    , 12        , 1           , "f"};
Motion traverse_R         = {"traverse"   , true    , 6000    , 6000    , 3000    , 12        , 1           , "r"};

Motion horizontal_F       = {"h_traverse" , true    , 5000    , 4000    , 3000    , 20        , 1           , "f"};
Motion horizontal_R       = {"h_traverse" , true    , 5000    , 4000    , 3000    , 20        , 1           , "r"};

Motion horizontal_Stair_F = {"h_traverse" , true    , 12000   , 6000    , 3000    , 20        , 2           , "f"};
Motion horizontal_Stair_R = {"h_traverse" , true    , 12000   , 6000    , 3000    , 20        , 2           , "r"};

Motion roll_F             = {"roll"       , false   , 6500    ,    0    ,    0    , 0         , 0           , "f"};
Motion roll_R             = {"roll"       , false   , 6500    ,    0    ,    0    , 0         , 0           , "r"};

Motion cornerClimb_F      = {"cornerClimb", true    , 15500   , 7000    , 3300    , 12        , 1         , "f"};
Motion cornerClimb_R      = {"cornerClimb", true    , 15500   , 7000    , 3300    , 12        , 1         , "r"};

Motion stepDownStair_F    = {"stairDown"  , true    , 12000   , 6000    , 4500    , 20        , 2          , "f"};
Motion stepDownStair_R    = {"stairDown"  , true    , 12000   , 6000    , 4500    , 20        , 2          , "r"};

Motion shake_F            = {"shake"      , false   , 4500    , 4000    , 3500    , 20        , 0         , "f"};
Motion softShake_F        = {"softShake"  , false   , 3500    , 3000    , 3000    , 20        , 0         , "f"};

Motion explode_F          = {"explode"    , false   , 15500   , 6000    , 5000    , 30        , 3         , "f"};
Motion explode_R          = {"explode"    , false   , 15500   , 6000    , 5000    , 30        , 3         , "r"};
//
Motion rollDouble_F       = {"double"     , false   , 15500   , 3000    , 3000    , 5         , 0         , "f"};
Motion rollDouble_R       = {"double"     , false   , 15500   , 3000    , 3000    , 5         , 0         , "r"};

//  List of possible colors;
Color red =     {1, 0, 0};
Color green =   {0, 1, 0};
Color blue =    {0, 0, 1};
Color yellow =  {1, 1, 0};
Color purple =  {1, 0, 1};
Color teal  =   {0, 1, 1};
Color white =   {1, 1, 1};
Color off   =   {0, 0, 0};

int GlobalplaneChangeTime;
int GlobalplaneChangeRPM;
int GlobalPlaneAccel = 2100;

extern int magicVariable = 0; // please don't ask what this is or what it does... its bad. =(
extern int MAGIC_DEBUG = 0;
extern int magicFace = 0;

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

extern int STEP_DOWN_STAIR_CURRENT_F = 4500;
extern int STEP_DOWN_STAIR_CURRENT_R = 4500;

extern int horizontal_CURRENT_F = 3000;
extern int horizontal_CURRENT_R = 3000;

extern int horizontal_Stair_RPM_F = 15000;
extern int horizontal_Stair_RPM_R = 15000;

extern int horizontal_Stair_CURRENT_F = 4500;
extern int horizontal_Stair_CURRENT_R = 4500;

uint32_t getAddressFromCubeID(int CubeID)
{
switch (CubeID) { // used to be ESP.getChipID
    default:
      return (0);
    case 99:                 //  ESP WIFI  885790061 || Server
      return (wifiAddress_SERVER);
    case 0:                  //  ESP WIFI 2133796284 ||  Test Mblock
      return (wifiAddress_cube00);
    case 1:
      return (wifiAddress_cube01);
    case 2:
      return (wifiAddress_cube02);
    case 3:
      return (wifiAddress_cube03);
    case 4:
      return (wifiAddress_cube04);
    case 5:                   //  960427 PEI RED  D0:D5:6F:CB:32:4C
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

