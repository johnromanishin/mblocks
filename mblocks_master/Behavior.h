#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "Cube.h"
#include "Face.h"
#include "SerialDecoder.h"
#include "CBuff.h"
#include "Defines.h"
#include "Communication.h"        // Includes wifi
#include "MagTag.h"

typedef enum Behavior
{
    SOLO_LIGHT_TRACK,
    DUO_LIGHT_TRACK,
    FOLLOW_ARROWS,
    CHILLING,
    TEST_TESTING_THANGS,
    ATTRACTIVE,
    TESTING, 
    SHUT_DOWN,
    RELAY_SLEEP
} Behavior;

// Behaviors are enumerated

////////////////////////////////////////////////////////////////////////////////////
// I. Behaviors involving non lattice connected Cubes or small mobile assemblies////
////////////////////////////////////////////////////////////////////////////////////
Behavior soloSeekLight(Cube* c, SerialDecoderBuffer* buf);
/*        --- What it does ---
 * 0. Updated sensors and checks if exit conditions are met
 * 1. Determine which face is the brightest
 * 2. Attempts to Moves towards the brightest face
 *        --- Exit Conditions ---
 * a. Checks if it is attached to a lattice (c->numberOfNeighbors > 0) >>>> Lattice Behavior
 * b. Checks to see if it has tried and failed to move repeaditly      >>>> Lattice alignment Behavior
 */

Behavior latticeAlign(Cube* c, SerialDecoderBuffer* buf);
/*        --- What it does ---
 * 0. Updated sensors and checks if exit conditions are met
 * 1. Attempts to roll forward/ backwards... Checks gyro sensor to see if we have rolled
 * 2. Attemps to Aligns flywheel to be parallal to the ground
 * 3. Performs several actuation events to attempt to join the lattice.
 *        --- Exit Conditions ---
 * a. Checks if it is attached to a lattice (c->numberOfNeighbors > 0) >>>> Lattice Behavior
 * b. Checks to see if it has tried and failed to move repeaditly      >>>> go to sleep
 */

Behavior duoSeekLight();
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

Behavior followArrows();
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
Behavior relaySleepMessage(Cube* c);

Behavior testTestingThangs(Cube* c, SerialDecoderBuffer* buf);
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
#endif
