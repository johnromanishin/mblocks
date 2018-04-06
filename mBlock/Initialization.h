#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <Arduino.h>
#include <painlessMesh.h>  // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoJson.h>
#include "Defines.h"  

void initializeCube();      // This function runs lots of the calibration and initialization functions 
bool checkIfConnected();    // checks to see if it can find any of the face boards on the I2C bus...
void shutDownMasterBoard(); // basically this shuts the ENTIRE BOARD DOWN...
void whatToDoIfIamNotConnectedAtBeginning();  // this runs when the board can't find any of the face boards connected to it
void initializeHardware();  // this starts up all of the busses like I2C, Serial, etc... And sets pin values
void disableAutoReset();    // this disables a feature by communicating with the master board
int inputVoltage();         // Asks the Motor Control Board through serial what the input voltage is... Used to turn off the board while charging
int get_battery_voltage();  // Asks the Motor control board through serial what the battery voltage is...
void lookUpCalibrationValues(long wifiID); // uses a switch/case to populate individually calibrated motion number
void loadMotionData(Motion* motion, int RPM, int Current, int brakeTime); // this loads the individually calibrated motorion 
void setMotionDataGlobalVariables();
void resetI2cBus();         // Reets power to the I2C bus by cycling its power off and then back on
                                                                          // data for each cube based on the ID number
#endif
