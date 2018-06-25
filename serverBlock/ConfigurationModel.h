#ifndef CONFIGURATIONMODEL_H
#define CONFIGURATIONMODEL_H

#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal
#include <painlessMesh.h>         // Wifi Mesh Library found on the internet  
#include <ArduinoJson.h>
#include <Arduino.h>              // library with basic arduino commands
#include "Defines.h"
#include "Interaction.h"
#include "Communication.h"

// Functions to use :
// bool areFacesOpposite(int face1, int face2)

/*
 * This Section is intended to figure out how to store and process the state of all
 * of the cubes which are currently attached to the wifi network.
 */

// Giant Data Table
#define timeContacted       0
#define topFace             1
#define forwardFace         2
#define numberOfNeighbors   3
#define face_0              4
#define face_1              5
#define face_2              6
#define face_3              7
#define face_4              8
#define face_5              9
#define lastNumber          10
//int database [NUM_CUBES][lastNumber] = { };    

/*
 * Create database
 */
//int database [NUM_CUBES][lastNumber] = { };    
 
//            Time Last Contacted | Top Face | Forward Face | No. Neighbors | Face0 | Face1 | face2 | face3 | face4 | face5 ||| Part of Line? | Game?
// Cube 0 |  
//    1   |
//    2   |
//    ... | 
//    18  |


int determineAvailableCubes();























#endif
