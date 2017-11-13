#ifndef MAGTAG_H
#define MAGTAG_H

#include <Arduino.h>

#include "Cube.h"
#include "Communication.h"
#include "Initialization.h"
#include "Defines.h"

typedef enum TagType
{
  TAGTYPE_NOTHING,
  TAGTYPE_INVALID,
  TAGTYPE_REGULAR_CUBE,
  TAGTYPE_PASSIVE_CUBE,
  TAGTYPE_COMMAND
} TagType;

typedef enum TagCommand
{
  TAGCOMMAND_NONE,
  TAGCOMMAND_SLEEP,
  TAGCOMMAND_PURPLE,
  TAGCOMMAND_DEBUG_MSG
} TagCommand;

typedef struct Tag
{
  TagType type;   
  /* 0 = not a valid tag
   * 1 = Regular Cube Attached
   * 2 = Passive Cube Attached
   * 3 = COMMAND tag
   */ 
  int angle ; // Either -1, 0, 1, 2 or 3 - corresponding to 90 deg angle
  int id; // ID or message code attached to tag
  int face; // face number (0,1,2,3,4,5) associated with a cube
  int strength; // Validitity of the tag, basically just agc1+agc2
  TagCommand command; // Text of command or behavior to go to... if it exists
} Tag;

void analyzeTag(int angle1, int agc1, int angle2, int agc2, Tag*);
int returnFaceNumber(int magDigit);

#endif
