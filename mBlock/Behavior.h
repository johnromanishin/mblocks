#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "Cube.h"
#include "Face.h"
#include "CBuff.h"
#include "Defines.h"
#include "Communication.h" // Includes wifi

/*
 * ----------------------------------------------------------------------------------------------------------------
 * ----------------- Related to Basic Upkeeping -------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------------------
 */
Behavior basicUpkeep(Cube* c, Behavior currentBehavior);

Behavior checkForBehaviors(Cube* c, Behavior behavior);

/*
 * Related to Magnetic Tags.
 */
int checkForMagneticTagsStandard(Cube* c);

/*
 * Related to WIFI communciation with the server...
 */
Behavior checkForWifiCommands(Cube* c, Behavior currentBehavior);
String generateUpdateMessage(Cube* c);

/*
 * ----------------------------------------------------------------------------------------------------------------
 * ----------------- State Machine Switching Controllers ----------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------------------
 */
Behavior LightTrackingStateMachine(Cube* c, Behavior inputBehavior, int numberOfNeighborz);

Behavior LineStateMachine(Cube* c, Behavior inputBehavior, int neighbros);

Behavior CubeStateMachine(Cube* c, Behavior inputBehavior, int numberOfNeighborz);

Behavior PathStateMachine(Cube* c, Behavior inputBehavior, int numberOfNeighborz);


/*
 * ----------------------------------------------------------------------------------------------------------------
 * ----------------- I. Behaviors involving non lattice connected Cubes or small mobile assemblies ----------------
 * ----------------------------------------------------------------------------------------------------------------
 */

Behavior soloSeekLight(Cube* c);
/*        --- What it does ---
 * 0. Updated sensors and checks if exit conditions are met
 * 1. Determine which face is the brightest
 * 2. Attempts to Moves towards the brightest face
 *        --- Exit Conditions ---
 * a. Checks if it is attached to a lattice (c->numberOfNeighbors > 0) >>>> Lattice Behavior
 * b. Checks to see if it has tried and failed to move repeaditly      >>>> Lattice alignment Behavior
 */

Behavior Pre_Solo_Light(Cube* c);
/*        --- What it does ---
 * 0. Updated sensors and checks if exit conditions are met
 * 1. Determine which face is the brightest
 * 2. Attempts to Moves towards the brightest face
 *        --- Exit Conditions ---
 * a. Checks if it is attached to a lattice (c->numberOfNeighbors > 0) >>>> Lattice Behavior
 * b. Checks to see if it has tried and failed to move repeaditly      >>>> Lattice alignment Behavior
 */
 
Behavior duoSeekLight(Cube* c);
/*        --- What it does ---
 * 0.
 *        --- Exit Conditions ---
 * a.
 */

/*
 * ----------------------------------------------------------------------------------------------------------------
 * ----------------- II. Behaviors for Cubes connected on a large lattice -----------------------------------------
 * ----------------------------------------------------------------------------------------------------------------
 */
 
Behavior climb(Cube* c);
/*        --- What it does ---
 * 0.
 * 1.
 *        --- Exit Conditions ---
 * a.
 * b.
 */
 
Behavior chilling(Cube* c);
/*        --- What it does ---
 * 0.
 * 1.
 *        --- Exit Conditions ---
 * a.
 * b.
 */

Behavior attractive(Cube* c);
/*        --- What it does ---
 * 0.   Attempts to attract other cubes to it by turning on its lights
 * 1.   Determines which faces are not top/bottom/ or connected
 * 2.   Turns on the 4 Face LEDs on these faces
 *
 *        --- Exit Conditions ---
 * a.   Told to do something else
 * b.   All 4 faces are blocked with something --> Chilling
 */

Behavior followArrows(Cube* c);
/*        --- What it does ---
 * 0.
 * 1.
 *        --- Exit Conditions ---
 * a.
 * b.
 */
 
/*
 * ----------------------------------------------------------------------------------------------------------------
 * ----------------- MISC. Helper functions -----------------------------------------
 * ----------------------------------------------------------------------------------------------------------------
 */
  
String behaviorsToCmd(Behavior inputBehavior);

Behavior cmdToBehaviors(String cmd, Behavior defaultBehavior);

void wifiTargetFace(Cube* c, int faceToSend, int recipientCube = -1);

void goToPlane(Cube* c, int FaceToGoTo);

#endif

