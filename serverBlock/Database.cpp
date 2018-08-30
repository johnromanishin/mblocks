#include "Database.h"

/*
 * This is just a quick global variable which we can modify 
 */
uint32_t previousTime = 0;

void testDatabase()
/*
 * Every time this function is called, the database is iterated over... and various things
 * are calculated and messages are sent... The basic Structure is as follows...
 * 1. Pre-Processing - going through and calculating neighbors, etc...
 * 2. Cube by cube Processing
 *      A. Check up on things
 *      B. Add appropriate messages to the inbox for each cube
 */
{
  /*
   * 1. We first do a pre-processing loop, applying certian rules to populate different
   * parts of the database, e.g. how many neighbors each cube has. These Rules are applied 
   * every time we run the database
   */
  for(int cubeEntry = 0; cubeEntry < NUM_CUBES; cubeEntry++)
  {
    database[cubeEntry][numberOfNeighbors] = countNeighborsFromDatabase(cubeEntry);
  }

  /*
   * This part is eveuntually supposed to update the connection database, but right now
   * it basically just prints out the contents of the database for cubes which have at least
   * one connection
   */
  if(DATABASE_DEBUG)
  {
    processConnections();
  }
  
  /*
   * Then we do a second loop over the database to actually 
   * send commands and apply rules to the cubes.
   * 
   * First thing we do is check to see if we should actually process the cube,
   * reasons not to include that they have recently just been sent a command, so we
   * don't want to repeat it until a timer has been brough down to zero.
   * To prevent the cube from doing something, we add time to the no-contact timer
   */
  for(int cubeEntry = 0; cubeEntry < NUM_CUBES; cubeEntry++)
  {
    /*
     * This section runs if we are supposed to try to contact the cube
     * If not, we decrease the timer "LeaveAloneFor" and then move on
     */
    if(database[cubeEntry][leaveAloneFor] < 0)
    {
      
      /*
       * This is intended to pick only one cube and tell it that it is part of a line
       */
      if((GAME == "LINE") && (checkIfInLine(cubeEntry) == true))
      {
        Serial.println("CHECK IF IN LINE is TRUE!");
        if(FOUND_LINE == false)
        {
          Serial.println("Sending message to pick a cube...");
          pushMessage(cubeEntry, "THE_ONE");
          pushMessage(cubeEntry, "THE_ONE");
          FOUND_LINE = true;
        }
      }

      /*
       * This function will run if this particular cube has one neighbor
       */
      if(database[cubeEntry][numberOfNeighbors] == 1)
      {
        if(checkIfBottomIsConnected(cubeEntry))
        {
          //Serial.println("WOOO! Added time and told it to go forward");
          //pushMessage(cubeEntry, "f");
          //addTimeToNoContactTimer(cubeEntry, 5000);
        }
      }

      /*
       * This function runs if there are more than one neighbor
       */
      else if(database[cubeEntry][numberOfNeighbors] > 1)
      {
        wifiDelay(1);
      }
    }

    /*
     * This is called for each cube where the "leaveAloneFor" timer is larger than zero
     * We decrement the time by how much time has elapsed since we just called this function
     */
    else
    {
      database[cubeEntry][leaveAloneFor] -= (millis() - previousTime);
    }
  }
  /*
   * Record the time between we recently called this function, in order to 
   * know how much time to remove from the leaveAloneFor timer
   */
  previousTime = millis();
}

void processConnections()
{
  /*
   * Iterate over all cubes in the database and take a look at the connections
   */
  if(DATABASE_DEBUG){
    Serial.println("-------------------------------------------------");
  }
  
  for(int cubeEntry = 0; cubeEntry < NUM_CUBES; cubeEntry++)
  {
    /*
     * Iterate over all of the faces...
     */
    for(int face = face_0; face < (FACES+face_0); face++)
    {
      if(DATABASE_DEBUG && (database[cubeEntry][face] > -1))
      {
        int numm = database[cubeEntry][face];
        Serial.print("Cube: ");Serial.print(cubeEntry);Serial.print(" Face: ");Serial.print(face-face_0);
        Serial.print(" -- ");
        Serial.print("Cube: ");Serial.print(numm/100);Serial.print(" Face: ");Serial.println((numm%100)/10);
      }
    }
  }
  if(DATABASE_DEBUG){
    Serial.println("-------------------------------------------------");
  }
}

void addTimeToNoContactTimer(int cubeNumber, int timeToAdd)
/*
 * This function takes to values, the cube number, and an ammount of time to add
 * And if the counter is negative, it sets the time to timeToAdd,
 * if it is larger than zero, then it adds it to the previous value.
 */
{
  if(database[cubeNumber][leaveAloneFor] < 0)
  {
    database[cubeNumber][leaveAloneFor] += timeToAdd;
  }
  else
  {
    database[cubeNumber][leaveAloneFor] += timeToAdd;
  }
}

bool checkIfBottomIsConnected(int cubeNumber)
/*
 * This function checks if the face on the bottom of the cube is connected
 * to another cube, and Only one other cube, and it returns true if this is so...
 */
{
  bool tempResult = false;
  if(database[cubeNumber][numberOfNeighbors] == 1)
  {
    /*
     * Loop over all of the faces to find the one which has a neighbor
     */
    for(int face = face_0; face < (face_0 + FACES); face++)
    {
      int neighborFace = -1;

      /*
       * we temporarily store the value of the the face into neihgborFace
       * if it exists
       */
      if(database[cubeNumber][face] > -1)
      {
        neighborFace = face-face_0;
        /*
         * If this face is equal to the bottom face, then we return true!
         */
        if(neighborFace == database[cubeNumber][bottom_Face])
        {
          tempResult = true;
        }
      }
    }
  }
  return(tempResult);
}


int countNeighborsFromDatabase(int cubeNumber)
/*
 * This function counts the number of neighbors from the information stored
 * in the database
 */
{
  /*
   * Start a count of the neighbors
   */
  int runningNeighborCount = 0;
  /*
   * Loop over the faces, and if the cube has a neighbor, increment the running
   * count of the neighbors
   */
  for(int face = face_0; face < (face_0 + FACES); face++)
    {
      if(database[cubeNumber][face] != -1)
      {
        runningNeighborCount++;
      }
    }
  return(runningNeighborCount);
}

int initializeDatabase()
/*
 * This function just sets all of the values of the database to be -1
 */
{
  for(int i = 0; i < NUM_CUBES; i++)
  {
    for(int j = 0; j < tableWidth; j++)
    {
      database[i][j] = -1;
    }
  }
}

bool checkIfInLine(int CubeToCheck)
/*
 * This is intended to check the first two neighbors that it finds and tests
 * to see if they are in a line - Eveuntually it should be able to check all of
 * the pairs of faces. This is NOT the best way to do this, but it appears to work
 */
{
  bool inAnLine = false;
  /*
   * Create temporary variables which are empty which will store
   * the faces which are neighbors are connected to
   */
  int face1 = -1;
  int face2 = -1;
  int face3 = -1;
  int face4 = -1;
  
  /*
   * Create a counter to count how many faces we have
   */
  int counter = 0;

  /*
   * Loops over the six faces in the database...
   * There is an offset the value face_0 in the database is where the index starts...
   */
  for(int faceIndex = face_0; faceIndex < (face_0 + FACES); faceIndex++)
  {
    /*
     * If the database of the index value shows that there is a face...
     * then we check to see
     */
    if(database[CubeToCheck][faceIndex] > -1)
    {
      /*
       * If this is the first face we have notices, then we assign the temp variable
       * face1 to be equal to the translated value for the face from the database...
       */
      if(counter == 0)
      {
        face1 = faceIndex - face_0;
        counter++;
      }
      /*
       * Do the similar thiing for the second face
       */
      else if(counter == 1)
      {
        face2 = faceIndex - face_0;
        counter++;
      }
      /*
       * etc...
       */
      else if(counter == 2)
      {
        face3 = faceIndex - face_0;
        counter++;
      }
      /*
       * Also for the fourth...
       */
      else if(counter == 3)
      {
        face4 = faceIndex - face_0;
        counter++;
      }
    }
  }
  /*
   * Now we a list of up to 4 cube faces by numbers, hopefully of the form like this
   * face1 = 0
   * face2 = 2
   * face3 = -1
   * with counter equal to the number of faces...
   * counter = 2
   */

  /*
   * If there are only 2 connections, 
   * we just need to check and see if they are opposites
   */
  if(counter == 2)
  {
    if(areFacesOpposite(face1, face2))
    {
      inAnLine = true;
    }
  }

  /*
   * If there are three connections, then we need to check all possible combinations of the 
   * connections
   */
  else if(counter == 3)
  {
    if(areFacesOpposite(face1, face2) || 
       areFacesOpposite(face1, face3) ||
       areFacesOpposite(face2, face3))
    {
       inAnLine = true;
    }
  }
  
  /*
   * If we have 4 neighbors, at least one pair must be on opposite faces, so
   * we just set inAnLine to be true.
   */
  else if(counter == 4)
  {
    inAnLine = true;
  }

  /*
   * Return the result
   */
  return(inAnLine);
}
