#ifndef DATABASE_H
#define DATABASE_H

#include "Defines.h"
#include "Interaction.h"
#include "Communication.h"

/* ------------------------------------------------------------------------------------
 * This Section is intended to figure out how to store and process the state of all
 * of the cubes which are currently attached to the wifi network.
 * ------------------------------------------------------------------------------------
 */

int initializeDatabase();
/*
 * This function sets all of the values in the database to be -1
 */
 
void testDatabase();
/*
 *  This function actually processes the database 
 */
 
void processConnections();
/*
 * This function processes the connection database
 */

 
bool checkIfInLine(int CubeToCheck);
/*
 * This function checks if a specific cube is part of a line
 */

int countNeighborsFromDatabase(int cubeNumber);
/*
 *  This function goes cube by cube and populates a number for how many neighbros
 *  the cube has
 */

bool checkIfBottomIsConnected(int cubeNumber);
/*
 * This fucntion checks to see if the particular cube has the bottom connected
 */

void addTimeToNoContactTimer(int cubeNumber, int timeToAdd);
/*
 * This function adds the specified time in ms to the no-contact-time attributed
 * in the database for that specific cube.
 */

#endif
