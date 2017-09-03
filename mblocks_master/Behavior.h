#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "Cube.h"
#include "Face.h"
#include "SerialDecoder.h"
#include "CBuff.h" 
#include "Defines.h"
#include "Communication.h"        // Includes wifi 
#include "MagTag.h"

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

/////////////////////////////////////////////////////////
// II. Behaviors Cubes connected on a large lattice////
/////////////////////////////////////////////////////////
Behavior duoSeekLight();

// Lattice Related 

Behavior crystolLattice();

Behavior chilling(Cube* c, bool r, bool g, bool b);

Behavior followArrows();

//////////////////////////////
// III. Misc. Behaviors   ////
//////////////////////////////
Behavior testTestingThangs(Cube* c, SerialDecoderBuffer* buf);

#endif
