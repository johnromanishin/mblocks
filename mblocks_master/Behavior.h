#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "Cube.h"
#include "Face.h"
#include "SerialDecoder.h"
#include "CBuff.h"
#include "Defines.h"
#include "Communication.h"        // Includes wifi

//typedef enum Behavior // ACTUAL CODE IS in DEFINES.h
//{
//    SOLO_LIGHT_TRACK,
//    DUO_LIGHT_TRACK,
//    FOLLOW_ARROWS,
//    CHILLING,
//    CLIMB,
//    ATTRACTIVE,
//} Behavior;

// Behaviors are enumerated

////////////////////////////////////////////////////////////////////////////////////
// I. Behaviors involving non lattice connected Cubes or small mobile assemblies////
////////////////////////////////////////////////////////////////////////////////////
Behavior sleep(Cube* c);

Behavior soloSeekLight(Cube* c, SerialDecoderBuffer* buf);
/*        --- What it does ---
 * 0. Updated sensors and checks if exit conditions are met
 * 1. Determine which face is the brightest
 * 2. Attempts to Moves towards the brightest face
 *        --- Exit Conditions ---
 * a. Checks if it is attached to a lattice (c->numberOfNeighbors > 0) >>>> Lattice Behavior
 * b. Checks to see if it has tried and failed to move repeaditly      >>>> Lattice alignment Behavior
 */

Behavior Pre_Solo_Light(Cube* c, SerialDecoderBuffer* buf);
/*        --- What it does ---
 * 0. Updated sensors and checks if exit conditions are met
 * 1. Determine which face is the brightest
 * 2. Attempts to Moves towards the brightest face
 *        --- Exit Conditions ---
 * a. Checks if it is attached to a lattice (c->numberOfNeighbors > 0) >>>> Lattice Behavior
 * b. Checks to see if it has tried and failed to move repeaditly      >>>> Lattice alignment Behavior
 */
 
Behavior duoSeekLight(Cube* c, SerialDecoderBuffer* buf);
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
Behavior crystallize();
/*        --- What it does ---
 * 0.
 * 1.
 *        --- Exit Conditions ---
 * a.
 * b.
 */
Behavior chilling(Cube* c, SerialDecoderBuffer* buf);
/*        --- What it does ---
 * 0.
 * 1.
 *        --- Exit Conditions ---
 * a.
 * b.
 */

Behavior attractive(Cube* c, SerialDecoderBuffer* buf);
/*        --- What it does ---
 * 0.   Attempts to attract other cubes to it by turning on its lights
 * 1.   Determines which faces are not top/bottom/ or connected
 * 2.   Turns on the 4 Face LEDs on these faces
 *
 *        --- Exit Conditions ---
 * a.   Told to do something else
 * b.   All 4 faces are blocked with something --> Chilling
 */

Behavior followArrows(Cube* c, SerialDecoderBuffer* buf);
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

Behavior climb(Cube* c, SerialDecoderBuffer* buf);
/*        --- What it does ---
 * 0.
 * 1.
 *        --- Exit Conditions ---
 * a.
 * b.
 */

 //==================Utilities===============================
void wifiDelay(int delayTime);
Behavior checkForMagneticTagsStandard(Cube* c, Behavior currentBehavior, SerialDecoderBuffer* buf);
Behavior checkForBasicWifiCommands(Cube* c, Behavior currentBehavior, SerialDecoderBuffer* buf);
Behavior relayBehavior(Cube* c, Behavior behaviorToRelay, int cubeToRelayTo = -1, int timesToRelay = 4);
Behavior cmdToBehaviors(String cmd, Behavior defaultBehavior);
Behavior checkForBehaviors(Cube* c, SerialDecoderBuffer* buf, Behavior behavior);

Behavior basicUpkeep(Cube* c, Behavior currentBehavior, SerialDecoderBuffer* buf, int lightDigit = 5, bool checkForLightMessages = true, bool blinkLEDs = true);

String behaviorsToCmd(Behavior inputBehavior);
#endif
