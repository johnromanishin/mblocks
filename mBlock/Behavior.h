#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "Cube.h"
#include "Face.h"
#include "CBuff.h"
#include "Defines.h"
#include "Communication.h" // Includes wifi

Behavior checkForWifiCommands(Cube* c, Behavior currentBehavior);

Behavior basicUpkeep(Cube* c, Behavior currentBehavior);

Behavior checkForBehaviors(Cube* c, Behavior behavior);

int checkForMagneticTagsStandard(Cube* c);
int processLightDigits(Cube* c);

int checkForMagneticTagsDEMO(Cube* c);

// WIFI RELATED
String generateUpdateMessage(Cube* c);


////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// State Machine Switching Controllers ///////////////////
////////////////////////////////////////////////////////////////////////////////////
Behavior LightTrackingStateMachine(Cube* c, Behavior inputBehavior, int numberOfNeighborz);
Behavior LineStateMachine(Cube* c, Behavior inputBehavior, int neighbros);

////////////////////////////////////////////////////////////////////////////////////
// I. Behaviors involving non lattice connected Cubes or small mobile assemblies////
////////////////////////////////////////////////////////////////////////////////////

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
 * 1.
 *        --- Exit Conditions ---
 * a.
 * b.
 */
///////////////////////////////////////////////////////////
// II. Behaviors for Cubes connected on a large lattice////
///////////////////////////////////////////////////////////
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
//////////////////////////////
// III. Misc. Behaviors   ////
//////////////////////////////
/**
 * Tells all other cubes to go to sleep and then goes to sleep.
 */
//Behavior relayBehavior(Cube* c, Behavior);

Behavior demo(Cube* c);

Behavior climb(Cube* c);
/*        --- What it does ---
 * 0.
 * 1.
 *        --- Exit Conditions ---
 * a.
 * b.
 */

 //==================Utilities===============================
void blinkFaceLeds(Cube* c, int waitTime = 50); // blinks LED's Once for a default time of 50ms

void wifiLightChange(Cube*c, int number, bool turnOff = true);
  // this turns the lights to a specific color based on the received integer 
  
String behaviorsToCmd(Behavior inputBehavior);
Behavior cmdToBehaviors(String cmd, Behavior defaultBehavior);
void wifiTargetFace(Cube* c, int faceToSend, int recipientCube = -1);

void goToPlane(int FaceToGoTo);


#endif

