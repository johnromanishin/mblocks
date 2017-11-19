#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <Arduino.h>
#include <painlessMesh.h>  // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoJson.h>

#include "Defines.h"

void initializeCube();
void initializeClasses(int faceVersion);
bool checkFaceVersion();
void shutDownMasterBoard();
void lookUpCalibrationValues();
void initializeHardware();
void disableAutoReset();
void whatToDoIfIamNotConnectedAtBeginning();
void resetI2cBus();
int inputVoltage();
int get_battery_voltage();


void loadMotionData(Motion* motion, int RPM, int Current, int brakeTime);
void actuallyLoadMotionData();
#endif
