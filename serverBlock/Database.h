#ifndef DATABASE_H
#define DATABASE_H

#include "Defines.h"
#include "Interaction.h"
#include "Communication.h"

// Functions to use :
// bool areFacesOpposite(int face1, int face2)

/*
 * This Section is intended to figure out how to store and process the state of all
 * of the cubes which are currently attached to the wifi network.
 */
 

void testDatabase();
void processConnections();
//int determineAvailableCubes();
int initializeDatabase();

int countNeighborsFromDatabase(int cubeNumber);
bool checkIfBottomIsConnected(int cubeNumber);
void addTimeToNoContactTimer(int cubeNumber, int timeToAdd);


#endif
