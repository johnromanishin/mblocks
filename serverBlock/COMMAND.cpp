#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal
#include <painlessMesh.h>         // Wifi Mesh Library found on the internet  
#include <ArduinoJson.h>
#include <Arduino.h>              // library with basic arduino commands
#include "Interaction.h"
#include "Communication.h"
#include "Defines.h"

#define Parameters 10
#define numberOfCubes 18
//  cubes[Parameters][NumberofCubes]
//
// Cube Number          0     1     2     ...NumberofCubes
// [0] cubeID           [    ][    ][    ]
// [1] wirelessID       [...
// [2] lastContacted    [
// [3] UP FACE          [
// [4] PLANE            [
// [5] NeighborID f0    [
// [6] NeighborID f1    [
// [7] NeighborID f2    [
// [8] NeighborID f3    [
// [9] NeighborID f4    [
// [10]NeighborID f5    [
// [11]                 [
// [12]                 [
// [13] batteryvoltage  [

int cubeDataArray[Parameters][numberOfCubes] = {};

void updateCubeInformation(int cubeID, int parameter, int valueToAdd)
{
  cubeDataArray[cubeID][parameter] = valueToAdd;
 
}

int returnCubeInformation(int cubeID, int parameter)
{
  return(cubeDataArray[cubeID][parameter]);
}
