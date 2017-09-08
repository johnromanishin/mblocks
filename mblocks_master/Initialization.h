#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <Arduino.h>

void initializeCube();
void initializeClasses(int faceVersion);
void checkFaceVersion();
void shutDownMasterBoard();
void lookUpCalibrationValues();
void initializeHardware();
void disableAutoReset();
int inputVoltage();
int get_battery_voltage();

#endif
