#include "Database.h"

/*
#define timeContacted       0
#define bottom_Face         1
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
uint32_t previousTime = 0;

void testDatabase()
{
  Serial.println("Running the database...");
  /*
   * This will iterate over all of the entries in the database
   * and do different things...
   * 
   * 1. We first do a pre-processing loop, applying certian rules to populate different
   * parts of the database, e.g. how many neighbors each cube has. These Rules are applied 
   * every time we run the database
   */
  for(int cubeEntry = 0; cubeEntry < NUM_CUBES; cubeEntry++)
  {
    database[cubeEntry][numberOfNeighbors] = countNeighborsFromDatabase(cubeEntry);
  }
  /*
   * THen we do a second loop over the database to actually 
   * send commands and apply rules to the cubes.
   * 
   * First thing we do is check to see if we should actually process the cube,
   * reasons not to includ that they have recently just been sent a command, so we
   * don't want to repead it until a timer has been brough down to zero.
   * To prevent the cube from doing something, we 
   */
  for(int cubeEntry = 0; cubeEntry < NUM_CUBES; cubeEntry++)
  {
    if(database[cubeEntry][leaveAloneFor] < 0)
    {
       //This Loop repeates for every cube ENTRY
//      if(database[cubeEntry][bottom_Face] != -1)
//      {
//        Serial.println("-----------------------");
//        Serial.print("WE HEARD FROM THIS CUBE!!; Bottom face is: ");
//        Serial.println(database[cubeEntry][bottom_Face]);
//        Serial.print("NEIGHBORS ON: ");
//        for(int face = face_0; face < (face_0 + FACES); face++)
//        {
//          if(database[cubeEntry][face] != -1)
//          {
//            Serial.print(" ");Serial.print(face-face_0);Serial.print(" ");
//          }
//        }
//        Serial.println(" ");
//        Serial.println(database[cubeEntry][bottom_Face]);
//        Serial.println("-----------------------");
//      }
      if(database[cubeEntry][numberOfNeighbors] == 1)
      {
        if(checkIfBottomIsConnected(cubeEntry))
        {
          Serial.println("WOOO! Added time and told it to go forward");
          pushMessage(cubeEntry, "f");
          addTimeToNoContactTimer(cubeEntry, 5000);
        }
      }
    
      if(database[cubeEntry][numberOfNeighbors] > 1)
      {
        pushMessage(cubeEntry, "RED");
        addTimeToNoContactTimer(cubeEntry, 5000);
        wifiDelay(300);
      }
    }
    else // this means we should leave the cube alone, but decrement the timer
    {
      Serial.print("Time remaining until we talk to again...: "); Serial.println(database[cubeEntry][leaveAloneFor]);
      database[cubeEntry][leaveAloneFor]-= (millis() - previousTime);
      Serial.print("Adjusted time Remaining: "); Serial.println(database[cubeEntry][leaveAloneFor]);
    }
  }
  previousTime = millis();
}

void addTimeToNoContactTimer(int cubeNumber, int timeToAdd)
{
   database[cubeNumber][leaveAloneFor] = timeToAdd;
}

bool checkIfBottomIsConnected(int cubeNumber)
{
  Serial.println("CHECKING BOTTOM");
  bool tempResult = false;
  if(database[cubeNumber][numberOfNeighbors] == 1)
  {
    for(int face = face_0; face < (face_0 + FACES); face++)
    {
      int neighborFace = -1;
      if(database[cubeNumber][face] > -1)
      {
        neighborFace = face-face_0;
        Serial.print("NEIGhBOR FACE is: ");
        Serial.println(neighborFace);
      }
      if((neighborFace == database[cubeNumber][bottom_Face]) && (database[cubeNumber][bottom_Face] > -1))
      {
        tempResult = true;
        Serial.println("WOOO FOUND ONE THAT SHOULD MOVE...");
      }
    }
  }
  return(tempResult);
}


int countNeighborsFromDatabase(int cubeNumber)
{
  int runningNeighborCount = 0;
  for(int face = face_0; face < (face_0 + FACES); face++)
    {
      if(database[cubeNumber][face] != -1)
      {
        runningNeighborCount++;
      }
    }
    if(runningNeighborCount > 0)
    {
     // Serial.print(" WE HAVE THIS MANY NEIGHBORS!!: ");
     // Serial.println(runningNeighborCount);
    }
  return(runningNeighborCount);
}

//int determineAvailableCubes()
//{
//  Serial.println("beginning determineAvailableCubes())");
//  
//  // Clear out the current contents of the inboxes
//  for(int i = 0; i < NUM_CUBES; i++)
//  {
//    pushBlinkMessage(i);
//  }
//}

int initializeDatabase()
{
  for(int i = 0; i < NUM_CUBES; i++)
  {
    for(int j = 0; j < tableWidth; j++)
    {
      database[i][j] = -1;
    }
  }
}

