#ifndef DEFINES
#define DEFINES

#include "Communication.h"

#define FACES 6
#define NUM_CUBES   17

// Giant Data Table
// Basic Data
// [index] = ID number of the cube...

#define timeContacted       0
#define bottom_Face         1
#define forwardFace         2
// Neighbor Related
#define numberOfNeighbors   3
#define face_0              4
#define face_1              5
#define face_2              6
#define face_3              7
#define face_4              8
#define face_5              9
// Other Flags?
#define inLine              10
#define changedFlag         11
#define justMoved           12
#define leaveAloneFor       13
#define prevNeighbors       14
#define tableWidth          15

extern int database[NUM_CUBES][tableWidth];  

/*
 *
 */
#include <painlessMesh.h> // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh

#define ARRAY_SIZEOF(x) ((sizeof(x) / sizeof(x[0])))
#define SERVER_ID 99
#define TESTCUBE_ID 0
#define OUTBOX_SIZE 10
#define INBOX_SIZE 20

#define wifiAddress_SERVER  885790061   //  Sparkfun THing SERVER
#define wifiAddress_cube00  2133796284  //  Sparkfun Thing LArge Breadboard

//  16 Actual Cubes...
#define wifiAddress_cube01  2131667094  //  {960662,    1},     // PEI BROWN  - F1:E8:71:B2:99:B5
#define wifiAddress_cube02  882644344   //  {10229112,  2},     // PC PURPLE  - DF:DF:3C:A0:F1:77
#define wifiAddress_cube03  887459591   //  {15044359,  3}      // ORANGE PC RED  CD:2B:5E:AB:3E:F3
#define wifiAddress_cube04  2139281740  //  {8575308,   4},     // PC Green : ED:A6:6A:8E:1B:58
#define wifiAddress_cube05  2131666859  //  {960427,    5},     // PEI RED  D0:D5:6F:CB:32:4C
#define wifiAddress_cube06  2139284147  //  {8577715,   6},     // PC Brown  C5:FF:AB:04:3B:9D
#define wifiAddress_cube07  2131666674  //  {960242,    7},     // PEI ORANGE E6:F6:05:69:08:F2
#define wifiAddress_cube08  2131666141  //  {959709,    8},     // PC YELLOW  - FB:0D:8F:2C:3B:B4           
#define wifiAddress_cube09  2139282946  //  {8576514,   9},     // PC BLACK  E3:6B:C6:CE:DA:31
#define wifiAddress_cube10  2131667020  //  {960558,    10},    // PEI BLUE  f7:AE:59:2B:D9:4D
#define wifiAddress_cube11  2139283535  //  {8577103,   11},    // PC ORANGE  - E6:E5:82:26:C7:8B
#define wifiAddress_cube12  2131666475  //  {960043,    12},    // PEI YELLOW  CC:F1:4F:AF:64:A8  
#define wifiAddress_cube13  887459658   //  {15044426,  13},    // PC Blue  D8:9C:4D:EA:27:65
#define wifiAddress_cube14  2121666126  //  {959694,    14},    // PEI PURPLE | FA:AA:25:19:C7:DF
#define wifiAddress_cube15  2131666780  //  {960348,    15},    // PEI GREEN
#define wifiAddress_cube16  2131666271  //  {959839,    16},    // PEI BLACK DB:9D:99:1A:BA:23

bool areFacesOpposite(int face1, int face2);
int oppositeFace(int face);
void wifiDelay(int delayTime);
uint32_t getAddressFromCubeID(int CubeID);

#endif
