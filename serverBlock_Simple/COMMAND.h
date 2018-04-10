#ifndef COMMAND_H
#define COMMAND_H
#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal
#include <painlessMesh.h>         // Wifi Mesh Library found on the internet  
#include <ArduinoJson.h>
#include <Arduino.h>              // library with basic arduino commands
#include "Interaction.h"
#include "Communication.h"
#include "Defines.h"

void updateCubeInformation(int cubeID, int parameter, int valueToAdd);

int returnCubeInformation(int cubeID, int parameter);

#endif
