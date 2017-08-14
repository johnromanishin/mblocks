#ifndef MOTION_H
#define MOTION_H

#include "defines.h"
//#include <arduino.h>

bool roll(char forwardOrReverse, int flywheelRPM = 6500);
bool lattice_transition();
bool setCorePlane(int targetCorePlane);
int currentCorePlane();

#endif
