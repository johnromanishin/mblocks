/*
 * This Section is intended to figure out how to store and process the state of all
 * of the cubes which are currently attached to the wifi network.
 */

int determineAvailableCubes()
{
  Serial.println("beginning determineAvailableCubes())");
  
  // Clear out the current contents of the inboxes
  for(int i = 0; i < 16; i++)
  {
    pushBlinkMessage(i);
  }
}
