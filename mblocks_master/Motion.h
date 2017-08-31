#ifndef MOTION_H
#define MOTION_H

#include "Defines.h"
#include "SerialDecoder.h"
#include <Arduino.h>

typedef struct Motion
{
  String moveName;      // name of move, used when we instantiate the type of movement
  bool brake; // true = use mechanical brake  || false = use the electronic brake only
  int rpm;              // RPM that we 
  int timeout;       // time that we will wait untill for confirmation of the speed
  int current;     // Current we apply to the brake in mA ... Maximum is 6000 ma
  int brakeTime;        // Time (milli Seconds) we apply the brake for Maximum time*current < some value
  int difficulty;     // Estimated difficulty of the move on a scale 1-255; with 1 == easy, 255 == very hard;
} Motion;

extern Motion traverse;
extern Motion roll;
extern Motion cornerClimb;

bool Move(Motion*, String Direction, int numberOfAttempts, SerialDecoderBuffer*);
bool setCorePlane(int targetCorePlane);
int currentCorePlane();

#endif
