#ifndef DEFINES
#define DEFINES

#include <Arduino.h>
#include "Communication.h"

/*
 * Plane Enums are the possible states that the internal robot can be in
 * in regards to the frame. Functions involving PlaneEnums involve 
 * 
 * PlaneEnum Cube::findPlaneStatus(bool reset) - returns (And adds to buffer) the current plane 
 * bool Cube::setCorePlaneSimple(PlaneEnum targetCorePlane) - actually attempts to go to a certian plane
 * 
 * Also See:
 * bool Cube::isValidPlane() - returns TRUE if 
 * PlaneEnum returnPlane(int face1, int face2)
 */
typedef enum PlaneEnum 
{
  PLANE0123,  // Valid Plane, with face "5" On Top
  PLANE0425,  // Valid Plane, with face "1" On Top
  PLANE1453,  // Valid Plane, with face "2" On Top
  PLANENONE,  // Means we are inbetween two valid planes
  PLANEMOVING,// gyro values show we are moving, measurements are therefor unreliable
  PLANEERROR  // Error - maybe I2C bus is disconnected?
} PlaneEnum;

String planeEnumToString(PlaneEnum thePlaneNow);

PlaneEnum stringToPlaneEnum(String inputString);

/*
 * TagCommands are different types of "commands" that can be contained within 
 * the special magnetic tags. Commands are designated by having the two magnetic Values
 * to be about the same value, i.e. Both digits are 27, or one is 26 and the other is 27
 */
typedef enum TagCommand
{
  TAGCOMMAND_NONE,
  TAGCOMMAND_SLEEP,
  TAGCOMMAND_PURPLE,
  TAGCOMMAND_27,
  TAGCOMMAND_25,
  TAGCOMMAND_23,
  TAGCOMMAND_21,
  TAGCOMMAND_19,
  TAGCOMMAND_13_ESPOFF,
} TagCommand;

typedef enum TagType
{
  TAGTYPE_NOTHING,
  TAGTYPE_INVALID,
  TAGTYPE_REGULAR_CUBE,
  TAGTYPE_PASSIVE_CUBE,
  TAGTYPE_COMMAND
} TagType;

typedef enum Behavior
{
    PRE_SOLO_LIGHT,
    SOLO_LIGHT_TRACK,
    DUO_LIGHT_TRACK,
    MULTI_LIGHT_TRACK,
    FOLLOW_ARROWS,
    CHILLING,
    CLIMB,
    ATTRACTIVE,
    TESTING, 
    SHUT_DOWN,
    SLEEP,
    RELAY_SLEEP,
    DEMO
} Behavior;

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

typedef struct Color
{
  bool r;
  bool g;
  bool b;
} Color;

//  List of possible colors;
extern Color red;
extern Color green;
extern Color blue;
extern Color yellow;
extern Color purple;
extern Color teal;
extern Color white;
extern Color off;

// List of Possible Motions, defined in Defines.cpp

extern Motion traverse_F;
extern Motion traverse_R;
extern Motion roll_F;
extern Motion roll_R;
extern Motion cornerClimb_F;
extern Motion cornerClimb_R;

extern Motion shake_F;
extern Motion softShake_F;

extern Motion explode_F;
extern Motion explode_R;

extern Motion rollDouble_F;
extern Motion rollDouble_R;

/// ESP ID to CUBE MAP ////
typedef struct EspToCubeMapping
{
  int esp;
  int cube;
} EspToCubeMapping;

int getEspIDFromCube(int);
int getCubeIDFromEsp(int);

//typedef enum PlaneEnum {PLANE0123, PLANE0425, PLANE1453, PLANENONE, PLANEMOVING, PLANEERROR} PlaneEnum;

/// Global Variables ///
#define DEBUG1 0 // DEBUG was already used somewhere
//#define DEBUG_VERBOSE 0
//#define DEBUG_BEHAVIOR 0

#define FACES 6  // Number of faces on a cube...
// Hardware Pin Definitions
#define Switch 12 //  Digital Output | Switch which controlls power to the face boards,  
                  //  High = power given to faceboards, Low = ability to charge
#define LED 13    //  Digital Output | Directly Controlls a small white LED on the "Master" circuit board
#define SDA 2     //  Managed by the wire.begin in initializeCube()
#define SCL 14    //  Managed by the wire.begin in initializeCube() 
/////////////////////////////////////////////////////////////////////
//extern int faceVersion; // This 
extern int GlobalCubeID;
extern int GlobalPlaneAccel;
extern bool magicTheLight;
extern int GlobalMaxAccel;

extern int GlobalplaneChangeTime;
extern int GlobalplaneChangeRPM;
extern int magicVariable;
extern int magicVariable_0;
extern int magicVariable_1;
extern int magicVariable_2;
extern int magicVariable_3;
extern int magicVariable_4;
extern int magicVariable_5;

extern int MAGIC_DEBUG;
extern int magicFace;

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
int faceClockiness(int faceTowards, int faceReference);

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
