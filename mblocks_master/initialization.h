#ifndef INITIALIZATION_H
#define INITIALIZATION_H
#include <ArduinoHardware.h>

void initializeCube();

void initializeClasses(int faceVersion);
void shutDownMasterBoard();
void lookUpCalibrationValues();
void initializeHardware();
void disableAutoReset();
int inputVoltage();

#endif
