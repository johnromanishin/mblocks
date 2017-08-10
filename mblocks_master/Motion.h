#ifndef MOTION_H
#define MOTION_H
#include <Arduino.h>
//#include <ArduinoHardware.h>

bool roll(bool forwardOrReverse, int flywheelRPM);
bool lattice_transition();
bool setCorePlane(int targetCorePlane);
int currentCorePlane();

#endif
