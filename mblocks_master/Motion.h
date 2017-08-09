#ifndef MOTION_H
#define MOTION_H

#include <ArduinoHardware.h>
#include "defines.h"

bool roll(bool forwardOrReverse, int flywheelRPM);
bool lattice_transition();
bool setCorePlane(int targetCorePlane);
int currentCorePlane();

#endif
