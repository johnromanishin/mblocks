#ifndef DEFINES
#define DEFINES

#include <Arduino.h>

// Global Variables:
#define FACES 6
#define PLANE_0321 ((int)'A')
#define PLANE_0425 ((int)'B') 
#define PLANE_1534 ((int)'C') 

extern int faceVersion;
extern int cubeID;
extern int planeChangeTime;
extern int planeChangeRPM;
extern int traverseBrakeCurrent_F;
extern int traverseBrakeCurrent_R;
extern int cornerClimbBrakeCurrent_F;
extern int cornerClimbBrakeCurrent_R;
extern int plane0321Magnet;
extern int plane0425Magnet;
extern int plane1435Magnet;
//

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