#include "Database.h"

/*
#define timeContacted       0
#define 
Face             1
#define forwardFace         2
// Neighbor Related
#define numberOfNeighbors   3
#define face_0              4
#define face_1              5
#define face_2              6
#define face_3              7
#define face_4              8
#define face_5              9
// Other Flags?
#define inLine              10
#define attribute           11

extern int database[NUM_CUBES][attribute];  
*/

void testThingNow()
{
  Serial.println("-----------------------");
  Serial.println(database[5][4]);
  Serial.println("-----------------------");
}

int determineAvailableCubes()
{
  Serial.println("beginning determineAvailableCubes())");
  
  // Clear out the current contents of the inboxes
  for(int i = 0; i < NUM_CUBES; i++)
  {
    pushBlinkMessage(i);
  }
}

