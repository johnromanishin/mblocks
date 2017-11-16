#ifndef DEFINES
#define DEFINES

#include <Arduino.h>
#include "Communication.h"

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
#define DEBUG1 0 // DEBUG was already used somewhere
#define DEBUG_VERBOSE

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
extern int traverseBrakeCurrent_F;
extern int traverseBrakeCurrent_R;
extern int cornerClimbBrakeCurrent_F;
extern int cornerClimbBrakeCurrent_R;


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
