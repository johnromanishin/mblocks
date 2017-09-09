#ifndef MAGTAG_H
#define MAGTAG_H

#include <Arduino.h>

#include "Cube.h"
#include "Communication.h"
#include "Initialization.h"

typedef enum TagType
{
  TAGTYPE_INVALID,
  TAGTYPE_REGULAR_CUBE,
  TAGTYPE_PASSIVE_CUBE,
  TAGTYPE_COMMAND
} TagType;

typedef enum TagCommand
{
  TAGCOMMAND_NONE,
  TAGCOMMAND_SLEEP
} TagCommand;

typedef struct Tag
{
  TagType type;   
  /* 0 = not a valid tag
   * 1 = Regular Cube Attached
   * 2 = Passive Cube Attached
   * 3 = COMMAND tag
   */ 
  int angle ; // Either 1, 2, 3 or 4 - corresponding to 90 deg angle
  int id; // ID or message code attached to tag
  int face; // face number (0,1,2,3,4,5) associated with a cube
  TagCommand command; // Text of command or behavior to go to... if it exists
} Tag;

void analyzeTag(int angle1, int agc1, int angle2, int agc2, Tag*);

#endif
