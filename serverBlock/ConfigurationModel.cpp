#include "ConfigurationModel.h"
/*
 * This Section is intended to figure out how to store and process the state of all
 * of the cubes which are currently attached to the wifi network.
 */

//int database [NUM_CUBES][lastNumber] = { };    


int determineAvailableCubes()
{
  Serial.println("beginning determineAvailableCubes())");
  
  // Clear out the current contents of the inboxes
  for(int i = 0; i < NUM_CUBES; i++)
  {
    pushBlinkMessage(i);
  }
}
