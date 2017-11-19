#ifndef DEFINES
#define DEFINES

#include <Arduino.h>
#include "Communication.h"

// This is a struct for organizing information about motions
typedef struct Motion
{
  String moveName;      // name of move, used when we instantiate the type of movement
  bool brake; // true = use mechanical brake  || false = use the electronic brake only
  int rpm;              // RPM that we 
  int timeout;       // time that we will wait untill for confirmation of the speed
  int current;     // Current we apply to the brake in mA ... Maximum is 6000 ma
  int brakeTime;        // Time (milli Seconds) we apply the brake for Maximum time*current < some value
  int difficulty;     // Estimated difficulty of the move on a scale 1-255; with 1 == easy, 255 == very hard;
  String for_rev;
} Motion;

// List of Possible Motions, defined in Motion.cpp
extern Motion traverse_F;
extern Motion traverse_R;
extern Motion roll_F;
extern Motion roll_R;
extern Motion cornerClimb_F;
extern Motion cornerClimb_R;

/// ESP ID to CUBE MAP ////
typedef struct EspToCubeMapping
{
  int esp;
  int cube;
} EspToCubeMapping;

int getEspIDFromCube(int);
int getCubeIDFromEsp(int);

typedef enum PlaneEnum {PLANE0123, PLANE0425, PLANE1453, PLANENONE, PLANEMOVING, PLANEERROR} PlaneEnum;

/// Global Variables ///
#define DEBUG1 1 // DEBUG was already used somewhere
#define DEBUG_VERBOSE 1
#define DEBUG_BEHAVIOR 1

#define FACES 6  // Number of faces on a cube...
// Hardware Pin Definitions
#define Switch 12 //  Digital Output | Switch which controlls power to the face boards,  
                  //  High = power given to faceboards, Low = ability to charge
#define LED 13    //  Digital Output | Directly Controlls a small white LED on the "Master" circuit board
#define SDA 2     //  Managed by the wire.begin in initializeCube()
#define SCL 14    //  Managed by the wire.begin in initializeCube() 
/////////////////////////////////////////////////////////////////////
extern int faceVersion;
extern int cubeID;

extern int GlobalplaneChangeTime;
extern int GlobalplaneChangeRPM;

extern int TRAVERSE_RPM_F;
extern int TRAVERSE_RPM_R;
extern int TRAVERSE_CURRENT_F;
extern int TRAVERSE_CURRENT_R;

extern int CC_RPM_F;
extern int CC_RPM_R;
extern int CC_CURRENT_F;
extern int CC_CURRENT_R;
extern int CC_BRAKETIME_F;
extern int CC_BRAKETIME_R;

//These tables define connectivity between faces for the cube
const extern PlaneEnum facePlanes[FACES][FACES];
const extern int faceRotations[FACES][4];

PlaneEnum returnPlane(int face1, int face2);

int faceArrowPointsTo(int readingFace, int connectionAngle);

#define ARRAY_SIZEOF(x) ((sizeof(x) / sizeof(x[0])))

// Face board pins
// Least significant byte
#define FB_LED1 7
#define FB_LEDB 5
#define FB_EN2  4
#define FB_EN1  3
#define FB_LEDA 2

// Most significant byte
#define FB_LEDC 7
#define FB_R1 5
#define FB_B1 4
#define FB_G1 3
#define FB_B2 2
#define FB_R2 1
#define FB_G2 0

#endif
