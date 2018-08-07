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
  //Serial.println("Running the database...");
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
  processConnections();
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
       if(checkIfInLine(cubeEntry) == true)
       {
          Serial.println("I FOUND A CUBE WITH A LINE!!!!!");
          if(FOUND_LINE == false)
            {
               pushMessage(cubeEntry, "THE_ONE");
               pushMessage(cubeEntry, "THE_ONE");
               FOUND_LINE = true;
            }
       }
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
          //Serial.println("WOOO! Added time and told it to go forward");
          //pushMessage(cubeEntry, "f");
          //addTimeToNoContactTimer(cubeEntry, 5000);
        }
      }
    
      if(database[cubeEntry][numberOfNeighbors] > 1)
      {
        //pushMessage(cubeEntry, "R");
        //addTimeToNoContactTimer(cubeEntry, 5000);
        //wifiDelay(300);
      }
    }
    else // this means we should leave the cube alone, but decrement the timer
    {
      //Serial.print("Time remaining until we talk to again...: "); Serial.println(database[cubeEntry][leaveAloneFor]);
      database[cubeEntry][leaveAloneFor]-= (millis() - previousTime);
      //Serial.print("Adjusted time Remaining: "); Serial.println(database[cubeEntry][leaveAloneFor]);
    }
  }
  previousTime = millis();
}

void processConnections()
{
  /*
   * Iterate over all cubes in the database
   */
  Serial.println("-------------------------------------------------");
  for(int cubeEntry = 0; cubeEntry < NUM_CUBES; cubeEntry++)
  {
    /*
     * Iterate over all of the faces...
     */
    for(int face = face_0; face < (FACES+face_0); face++)
    {
      if(database[cubeEntry][face] > -1)
      {
        int numm = database[cubeEntry][face];
        Serial.print("Cube: ");Serial.print(cubeEntry);Serial.print(" Face: ");Serial.print(face-face_0);
        Serial.print(" -- ");
        Serial.print("Cube: ");Serial.print(numm/100);Serial.print(" Face: ");Serial.println((numm%100)/10);
      }
    }
  }
  Serial.println("-------------------------------------------------");
}

void addTimeToNoContactTimer(int cubeNumber, int timeToAdd)
{
   database[cubeNumber][leaveAloneFor] = timeToAdd;
}

bool checkIfBottomIsConnected(int cubeNumber)
{
  //Serial.println("CHECKING BOTTOM");
  bool tempResult = false;
  if(database[cubeNumber][numberOfNeighbors] == 1)
  {
    for(int face = face_0; face < (face_0 + FACES); face++)
    {
      int neighborFace = -1;
      if(database[cubeNumber][face] > -1)
      {
        neighborFace = face-face_0;
      }
      if((neighborFace == database[cubeNumber][bottom_Face]) && (database[cubeNumber][bottom_Face] > -1))
      {
        tempResult = true;
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

/*
 * This is intended to check the first two neighbors that it finds and tests
 * to see if they are in a line - Eveuntually it should be able to check all of
 * the pairs of faces
 */
bool checkIfInLine(int CubeToCheck)
{
   bool inAnLine = false;
   int face1 = -1;
   int face2 = -1;
   int face3 = -1;
   int face4 = -1;
   int counter = 1;
   for(int face = face_0; face < (face_0 + FACES); face++) // 
   {
    if(database[CubeToCheck][face] != -1)
      {
      if(counter == 1)
        {
          face1 = face-face_0;
          counter++;
        }
        else if(counter == 2)
        {
          face2 = face-face_0;
          counter++;

        }
        else if(counter == 3)
        {
          face3 = face-face_0;
          counter++;
        }
        else if(counter == 4)
        {
          face3 = face-face_0;
          counter++;
        }
      }
    }
    /*
     * Now we have up to 4 cube faces 
     * with counter equal to the number of faces...
     */
     if(counter == 2)
     {
      if(areFacesOpposite(face1, face2))
        inAnLine = true;
     }
     else if(counter == 3)
     {
        if(areFacesOpposite(face1, face2) || areFacesOpposite(face1, face3) ||
           areFacesOpposite(face2, face3))
          inAnLine = true;
     }
     /*
      * If we have 4 neighbors, at least one pair must be on opposite faces
      */
     else if(counter == 4)
     {
        inAnLine = true;
     }
   return(inAnLine);
}
