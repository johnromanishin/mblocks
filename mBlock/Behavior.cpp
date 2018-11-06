#include "Behavior.h"

/*
 * Index
 * *BASIC - Basic Upkeep Function
 * *WIFI  - Wifi Management function
 * *TAGS  - Code Dealing with magnetic tags...
 * 
 * *LINE  - LINE FORMING State Machine switching
 * *LIGHT - Light Tracking State Machine switching
 * *CUBE - code to form a large cube
 * *GRID - Grid Aggregate
 */

/*
 * ===============================================================================================================
 * ===============================================================================================================
 * ===============================================================================================================
 * ============================= *BASIC STATE MACHINE UPKEEP======================================================
 * ===============================================================================================================
 * ===============================================================================================================
 * ===============================================================================================================
 */

Behavior basicUpkeep(Cube* c, Behavior inputBehavior)
{
  /*
   * c->update(); reads all of the sensor values, includinglight sensors, the magnetic sensors
   * and IMU's and adds the values to the circular buffers corresponding to each sensor
   */
  c->update();
  wifiDelay(20);
  
  /*
   * checkForWifiCommands looks to see if it should take any actions based on received WIFI commands
   * these fit into the following categories
   * 1. Movements 
   * 2. LED's
   * 3. Status update
   * 4. Change "game"
   */
  

  Behavior newBehavior = checkForWifiCommands(c, inputBehavior);
  if (MAGIC_DEBUG) 
  {
    Serial.print("nextBehavior is... ");
    Serial.println(behaviorsToCmd(newBehavior));
  }
  
  /*
   * If we have failed to properly move three times in a row, we reset...
   */
  if( c->moveSuccessBuffer.access(0) == false && 
      c->moveSuccessBuffer.access(1) == false &&
      c->moveSuccessBuffer.access(2) == false )
      {
        c->superSpecialBlink(&purple, 100);
        c->superSpecialBlink(&purple, 90);
        c->superSpecialBlink(&red, 80);
        c->superSpecialBlink(&purple, 70);
        c->superSpecialBlink(&yellow, 60);
        Serial.println("esprst");
        delay(100);
        Serial.println("esprst");
      }
  /*
   * checkForMagneticTagsStandard - this function "processes" the magnetic tags
   * some tags will prompt immediate actions, including making it go to sleep, 
   * or changing to specific colors.
   */
  int numberOfNeighborz = checkForMagneticTagsStandard(c);

  if (Game == "LIGHT") // *LIGHT
  {
    wifiDelay(200);
    return(LightTrackingStateMachine(c, newBehavior, numberOfNeighborz));
  }
  
  else if(Game == "LINE") // *LINE
  {
    wifiDelay(200);
    return(LineStateMachine(c, newBehavior, numberOfNeighborz));
  }
  
  else if(Game == "CUBE") // *CUBE
  {
    wifiDelay(200);
    return(CubeStateMachine(c, newBehavior, numberOfNeighborz));
  }
  
  else if(Game == "PATH") // *CUBE
  {
    wifiDelay(200);
    return(PathStateMachine(c, newBehavior, numberOfNeighborz));
  }
  
  else if(Game == "GRID") // *GRID
  {
    return(GridAggregateStateMachine(c, newBehavior, numberOfNeighborz));
  }
  
  return (newBehavior);
}

/*
 * ===============================================================================================================
 * ===============================================================================================================
 * ===============================================================================================================
 * ============================= *WIFI Checking CHECKING==========================================================
 * ===============================================================================================================
 * ===============================================================================================================
 * ===============================================================================================================
 */
 
Behavior checkForWifiCommands(Cube* c, Behavior currentBehavior)
{
  /*
   * This is part of Basic Upkeep - the goal of this function is to process queued
   * wifi message, and then act on them. 
   */
  Behavior resultBehavior = currentBehavior;
  if (!jsonCircularBuffer.empty()) 
  {
    /* At this point, we have determined that the message is for us... so now we try to decode the contents
     * this extracts the contents of "cmd" and puts it into a local variable
     * Create a buffer to store our Jason Objects...
     */
    StaticJsonBuffer<512> jb;
    JsonObject& jsonMsg = jb.parseObject(jsonCircularBuffer.pop());
    
    String receivedCMD = jsonMsg["cmd"];

    /*  checks to see if the recieved message matches a behavior...
        If it doesn't we default to the currentBehavior
    */
    
    /*  If the command is "blink" then we will really quickly blink the face LED's
     *  This can be used to visually verify which cubes are actually connectd to the wifi mesh
     *  and are working properly
     */
    
    if (receivedCMD == "b")
    {
      c->lightCube(&green);
      wifiDelay(300);
      c->lightCube(&off);
    }
    
    /*
     * Motion Commands
     * "f" - attempts to move "forward" - either ontop, or on the side/ground
     * "r" - attempts to move "reverse" - either ontop, or on side/ground
     * "c" - attempts to climb, if this is possible
     */
    else if((receivedCMD == "f") || (receivedCMD == "r"))
    {
      /*  First we check to see if we have (1) neighbor
       *  and that neighbor is on the bottom,
       *  then we want to do a regular traverse
       */
      int tempNeighbors = c->numberOfNeighbors(0,0);
      if(tempNeighbors < 3)
      {
        /*
         * Ok so we know we only have one or two neighbor,s now we check to see 
         * if the neighbor is on the bottom, or on the side...
         */
        if((c->whichFaceHasNeighbor(0) == c->returnBottomFace()) &&
           (tempNeighbors == 1))
        {
          /*
           * Now we need to check to make sure that we have a forward face...
           * Assuming this is true, we will finally actually move...
           */
          if(c->returnForwardFace() != -1)
          {
            if(receivedCMD == "f")
            {
              c->moveOnLattice(&traverse_F);
              if(MAGIC_DEBUG) {
                Serial.println("Preparring to move forward");
              }
            }
            else if(receivedCMD == "r")
            {
              c->moveOnLattice(&traverse_R);
              if(MAGIC_DEBUG) {
              Serial.println("Preparring to move IN REVERSE");
              }
            }
          }
        }
        /*
         * Horizontal moves, when the face with a neighbor is neither top nor bottom...
         * and the plane is correct...
         */
        else if((c->whichFaceHasNeighbor(0) != c->returnBottomFace()) && 
                (c->whichFaceHasNeighbor(0) != c->returnTopFace()) &&
                c->returnForwardFace() == -1)
        {
          if(receivedCMD == "f")
          {
            if(tempNeighbors == 1)
            {
              c->moveOnLattice(&horizontal_F);
            }
            else if(tempNeighbors == 2)
            {
              c->moveOnLattice(&horizontal_Stair_F);
            }
          }
            
          else if(receivedCMD == "r")
          {
            if(tempNeighbors == 1)
            {
              c->moveOnLattice(&horizontal_R);
            }
            else if(tempNeighbors == 2)
            {
              c->moveOnLattice(&horizontal_Stair_R);
            }
          }
        }
        else if(c->whichFaceHasNeighbor(0) != c->returnBottomFace())
        {
          //WIP
          c->superSpecialBlink(&yellow, 50);
          c->superSpecialBlink(&yellow, 70);
          c->superSpecialBlink(&yellow, 50);
        }
      }
        
      /* If we happen to be connected by more than one face... then we need to think this through
       *  right now all we do is blink, WIP
       */
      else if(c->numberOfNeighbors() > 2)
      {
        c->superSpecialBlink(&red, 50);
        c->superSpecialBlink(&red, 70);
        c->superSpecialBlink(&red, 50);
      }
    }

    /*
     * OTHER Related Commands
     */
    else if(receivedCMD == "s")
    {
      c->shutDown();
    }
    
    else if(receivedCMD == "Y")
    {
      c->lightCube(&yellow);
      mesh.update();
    }
      
    else if(receivedCMD == "B")
    {
      c->lightCube(&blue);
      mesh.update();
    }
    
    else if(receivedCMD == "R")
    {
      c->lightCube(&red);
      mesh.update();
    }
      
    else if(receivedCMD == "P")
    {
      c->lightCube(&purple);
      mesh.update();
    }
    
    else if(receivedCMD == "G")
    {
      c->lightCube(&green);
      mesh.update();
    }
    else if(receivedCMD == "W")
    {
      c->lightCube(&white);
      mesh.update();
    }
      
    else if(receivedCMD == "O")
    {
      c->lightCube(&off);
      mesh.update();
    }

    else if(receivedCMD == "P")
    {
      c->lightCube(&off);
      mesh.update();
    }

    else if(receivedCMD == "CPPG")
    {
      c->goToPlaneParallel(c->returnTopFace());
      mesh.update();
    }
      
    else if(receivedCMD == "off")
    {
      c->lightCube(&off);
      mesh.update();
    }
    
    else if (receivedCMD == "lightSeek")
    {
      Game = "LIGHT";
    }

    else if (receivedCMD == "line")
    {
      Game = "LINE";
    }

    else if (receivedCMD == "THE_ONE")
    {
      THE_CHOSEN_ONE = true;
      sendAck(CONFIRM_MESSAGE_ID);
      wifiDelay(1000);
      sendAck(CONFIRM_MESSAGE_ID);
      if(MAGIC_DEBUG)
        Serial.println("RECEIVED WIFI COMMAND TO BE CHOSEN ONE");
    }
    
    else if (receivedCMD == "chill")
    {
      Game = "Nothing";
    }
      
    /* cubeID's == 0 means it is attached by a cable... not a real cube // so we print
    */

    if (thisCubeID == 0)
    {
      String receivedCMD = jsonMsg["cmd"];
      String senderID = jsonMsg["sID"];
      String stringMsg = "Message from: " + senderID + " to: " + 
      thisCubeID + " Command is: " + receivedCMD;
      Serial.println(stringMsg);
    }
  }
  return (resultBehavior);
}

/*
 * ===============================================================================================================
 * ===============================================================================================================
 * ===============================================================================================================
 * =============================*TAGS MAGNETIC TAGS CHECKING======================================================
 * ===============================================================================================================
 * ===============================================================================================================
 * ===============================================================================================================
 */

int checkForMagneticTagsStandard(Cube* c)
/*
 * This function processes the 6 magnetic tags on all 6 faces...
 * It returns an integer corresponding to how many actual cube neighbors the cube has at that instant.
 *  Additional actions need to be performed immedediatly from the 
 *  These are the parameters we have access to:
 *    a. bool returnNeighborPresence(int index);
 *    b. int returnNeighborID(int index);
 *    c. int returnNeighborAngle(int index);
 *    d. int returnNeighborFace(int index);
 *    e. TagType returnNeighborType(int index);
 *    f. TagCommand returnNeighborCommand(int index);
 *  
 *  For reasons which are very difficult to explain this function also changes planes...
 */
{
  /*
   * variable to count of the number of neighbors that we have
   */
  int neighbors = 0; 

  /*
   * Now we loop over all six faces and evaluate the circular buffers filled with tag parameters
   * for each of the magnetic sensors - this function doesn't actually update the sensor values
   * that happens in the cube->update();
   */
  for (int face = 0; face < FACES; face++)
  {
    /* This gets activated if we are attached to an actual cube or passive cube
       for at least two time steps...
    */
    if ((c->faces[face].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE) ||
        (c->faces[face].returnNeighborType(0) == TAGTYPE_REGULAR_CUBE))
    {
      neighbors++;
      if ((c->faces[face].returnNeighborAngle(0) > -1) && (MAGIC_THE_LIGHT == false))
      {
        c->lightFace(faceArrowPointsTo(face, c->faces[face].returnNeighborAngle(0)), &purple);
        wifiDelay(200);
        c->lightCube(&off);
      }
    }
    
    /*
     * This section changes the planes of the module, either if the global variable 
     * "magicvariable" is set to one... This is done this way becuase it crashes if it is 
     * done any other way. I have no
     * idea why this is the behavior
     */
    if ((c->faces[face].returnNeighborCommand(0) == TAGCOMMAND_05_PURPLE) ||
        (magicVariable == 1))
    {
      int z = face;
      if (magicVariable)
      {
        z = magicFace;
        magicFace = 0;
      }
      magicVariable = 0;
      c->goToPlaneParallel(z);
    }

    /*
     * This section checks for specific commaands, and does an action if that is appropriate
     */
    if (c->faces[face].returnNeighborCommand(0) == TAGCOMMAND_09_ORANGE)
    {
      wifiDelay(100);
      PART_OF_LINE = true;
      THE_CHOSEN_ONE = true;
      if(MAGIC_DEBUG)
      {
        Serial.println("BECAME THE CHOSEN ONE THROUGH MAGNETIC TAG");
      }
    }
    
    else if (c->faces[face].returnNeighborCommand(0) == TAGCOMMAND_27_GREEN)
    {
      c->lightCube(&green);
    }
    
    else if (c->faces[face].returnNeighborCommand(0) == TAGCOMMAND_23_BLUE)
    {
      c->shutDown();
    }

    else if (c->faces[face].returnNeighborCommand(0) == TAGCOMMAND_13_RED)
    {
      MAGIC_DEBUG = 1;
    }
    /*
     *  End Else/if tree for specific commands
     */
  }
  return (neighbors);
}


/*
 * ===============================================================================================================
 * ==================*LINE STATE MACHINE==========================================================================
 * ===============================================================================================================
 */

Behavior LineStateMachine(Cube* c, Behavior inputBehavior, int neighbros)
/*
 * This code determines what to do if the goal is to get into a line...
 * Issues...
 * 
 * Variables...
 * bool PART_OF_LINE
 * int FACES_LIGHTS[FACES];
 * State Discussion...
 * First... if we think we are part of a line | SEEN THE LIGHT! | then we have two special faces...
 */
{
  Behavior newBehavior = inputBehavior; 
  int numberOfNeighborz = c->numberOfNeighbors(0);
  int first_neighborFace = c->whichFaceHasNeighbor(0);
  int second_neighborFace = c->whichFaceHasNeighbor(1);
  if(THE_CHOSEN_ONE)
  {
    PART_OF_LINE = true;
  }
  
  if (MAGIC_DEBUG) 
  {
    Serial.print("MAGIC_THE_LIGHT: ");Serial.print(MAGIC_THE_LIGHT);
    Serial.print(" | PART_OF_LINE: ");Serial.print(PART_OF_LINE);
    Serial.print(" | THE CHOSEN ONE: ");Serial.print(THE_CHOSEN_ONE);
    Serial.print("FIRST_NEIGHBOR FACE: ");Serial.println(first_neighborFace);
    Serial.print("SECOND_NEIGHBOR FACE: ");Serial.println(second_neighborFace);
    Serial.print("FACES_LIGHTS: ");
    for(int face = 0; face < FACES; face++)
    {
      Serial.print(" | Face: ");
      Serial.print(face);
      Serial.print(" = ");
      Serial.print(FACES_LIGHTS[face]);
    }
    Serial.println(" ");
  }
  /*
   * First we check to see if we are part of a line or not...
   * bool PART_OF_LINE is a global variable keeping track of this...
   * First we check to see if any of the faces has seen part of a line...
   */
  if(PART_OF_LINE == false)
  {
    for(int face = 0; face < FACES; face++)
    {
      if(FACES_LIGHTS[face] > 0)
      {
        PART_OF_LINE = true;
        MAGIC_THE_LIGHT = true;
      }
    }
  }

   /*
   * If something else told us we are part of a line (e.g. WIFI, or special tag)
   * Then we need to check our neighbors, etc...
   * We just arbitrarily pick the first face with a neighbor, and make that face, and its opposite
   * to be active faces...
   */
  
  if((PART_OF_LINE == true) && (MAGIC_THE_LIGHT == false))
  {
    if((first_neighborFace > -1) && (first_neighborFace < 6))
    {
       FACES_LIGHTS[first_neighborFace] = 1;
       FACES_LIGHTS[oppositeFace(first_neighborFace)] = 1;
       MAGIC_THE_LIGHT = true;
    }
  }
  
  if(PART_OF_LINE == true)
  {
    c->lightCube(&green);
    wifiDelay(500+random(500));
  }

  /*
   * If we lose all of our neighbors... reset the light things...
   */
  if((c->numberOfNeighbors(0) == 0) && (c->numberOfNeighbors(2) == 0))
  {
    if(MAGIC_DEBUG)
    {
       Serial.println("Erassing magic the light and all faces...");
    }
    
    MAGIC_THE_LIGHT = false;
    PART_OF_LINE = false;
    
    for(int face = 0; face < FACES; face++)
    {
      FACES_LIGHTS[face] = 0;
    }
    
    if((TOP_FACE_LIGHT[0] > TOP_LIGHT_THRESHOLD) && (TOP_FACE_LIGHT[8] > TOP_LIGHT_THRESHOLD))
    {    
      c->blockingBlink(&yellow);
      c->moveOnLattice(&horizontal_F);
    }
  }
  
  /* ----------------------------------------------------------------------------
   * ----------------------------------------------------------------------------
   * ----------------------------------------------------------------------------
   * Now we base our behavior on whether or not the light is turned on...
   * If it is turned on... we prepare to actually move. However we wait 10 seconds from when the light turned on
   
   */
    int timeToWait = 10000;
    if(TOP_FACE_LIGHT[0] > TOP_LIGHT_THRESHOLD && (millis() - timeLightTurnedOn) > timeToWait && 
      (PART_OF_LINE == false ))
    {
      newBehavior = CHILLING;
      /*
      * IF WE HAVE 0 NEIGHBOR...
      */
      if ((numberOfNeighborz == 0) && (c->numberOfNeighbors(2) == 0))
      {
        newBehavior = SOLO_LIGHT_TRACK;
        c->blockingBlink(&teal);
      }
      /*
      * IF WE HAVE 1 NEIGHBOR...
      */
      else if ((numberOfNeighborz == 1) && (c->numberOfNeighbors(2) == 1))
      {
        /*
        * If we have ONE neighbor, and it is on the BOTTOM, then our strategy is to just move until get down
        * to the lower leve.
        */
        if (first_neighborFace == c->returnBottomFace())
        {
          if(c->returnForwardFace() != -1)
          {
            if(DIRECTION)
            {
              c->moveOnLattice(&traverse_F);
            }
            else
            {
              c->moveOnLattice(&traverse_R);
            }
          }
          /*
          * If our flywheel is parallel to the ground, then we attempt to change planes
          */
          else
          {
            goToPlane(c, faceArrowPointsTo(first_neighborFace, c->faces[first_neighborFace].returnNeighborAngle(0)));
          }
        }
        /*
        * This means that we are ready to move... Since the face is neither top/botom
        * and our plane is parallal to the ground
        */
        else if((first_neighborFace != c->returnBottomFace()) && 
              (first_neighborFace != c->returnTopFace()) &&
               (c->returnForwardFace() == -1))
        {
          if(DIRECTION)
          {
            c->moveOnLattice(&horizontal_F);
          }
          else
          {
            c->moveOnLattice(&horizontal_R);
          }
        }        
      
        else
        {
          goToPlane(c, c->returnTopFace());
        }
      }
  
    /*
      * IF WE HAVE 2 NEIGHBOR...
      */
      else if ((numberOfNeighborz == 2) && (c->numberOfNeighbors(2) == 2) && 
            ((millis() - timeLightTurnedOn) > (30000+random(20000))))
      {
        if(c->areFacesOpposite(first_neighborFace, second_neighborFace) == true)
        {
          c->blockingBlink(&purple);
          wifiDelay(1000);
          c->blockingBlink(&purple);
        }
        /*
        * If both of the faces are in the plane parallel to the ground... we pick a direction (FORWARD)
        * and move that way
        */
        else if((first_neighborFace != c->returnBottomFace()) && (first_neighborFace != c->returnTopFace()) &&
             (second_neighborFace != c->returnBottomFace()) && (second_neighborFace != c->returnTopFace()))
        {
          c->blockingBlink(&white);
          if(c->isPlaneInPlaneOfFaces(first_neighborFace, second_neighborFace) && millis() > random(60000))
          {
            c->moveOnLattice(&horizontal_Stair_F);
          }
          else
          {
            goToPlane(c, c->returnTopFace());
          }
        }
        /*
         * if Either of the faces that we are connect to is the bottom face...
         * Then we need to set the plane to be in the plane of both neighbors, and then 
         * try to move DOWN away... Not to climb.
         */
        else if((first_neighborFace == c->returnBottomFace()) ||
                (second_neighborFace == c->returnBottomFace()))
        {
          if(first_neighborFace == c->returnBottomFace())
          {
            DIRECTION = faceClockiness(c->returnBottomFace(), second_neighborFace);
          }
        
          else if(second_neighborFace == c->returnBottomFace())
          {
            DIRECTION = faceClockiness(c->returnBottomFace(), first_neighborFace);
          }
        
          if(DIRECTION == 1)
          {
            c->moveOnLattice(&stepDownStair_F);
          }
        
          else if(DIRECTION == -1)
          {
            c->moveOnLattice(&stepDownStair_R);
          }
        }

        /*
         * This means one of the faces is the top/bottom face, and the flywheel is NOT
         * in a valid plane, so we pick a face that isn't the bottom/top face
         * and move the plane to be parallal to that
         */
        else if(c->returnForwardFace() == -1)
        {
          goToPlane(c, faceArrowPointsTo(c->returnBottomFace(), 
          c->faces[c->returnBottomFace()].returnNeighborAngle(0)));
        }
        else
        {
          c->superSpecialBlink(&yellow, 100);
        }
      }

      /*
      * IF WE HAVE 2+ NEIGHBOR...
      */

      else if (numberOfNeighborz > 2)
      {
        if (MAGIC_DEBUG) 
        {
          Serial.println("***NEIGHBORS == __ 2+ __");
        }
        c->blockingBlink(&red);
      }
  }
  /*
   * THis means that the light is off what we are supposed to do...
   * 1. if we have no neighbors, spin a little bit...
   * 2. Try to get to the correct plane
   * 3. Turn on if we are part of line...
   * 4. Wait
   */
  else
  {
    wifiDelay(500);
    c->blockingBlink(&yellow);
  }
  wifiDelay(400);
  return (newBehavior);
}


//=============================================================================================
//=============================================================================================
//=============================================================================================
//=============================*LIGHT STATE MACHINECHECKING====================================
//=============================================================================================
//=============================================================================================
//=============================================================================================

Behavior LightTrackingStateMachine(Cube* c, Behavior inputBehavior, int numberOfNeighborz)

/*
   This function does basic state machine switching
   It (1) Updates the sensors, including magnetic sensors
   light sensors, and IMU's

   Then it checks the wifi BUFFER and checks the magnetic tags
   for actionable configurations
*/
{
  if (MAGIC_DEBUG) Serial.println("Starting LightTrackingStateMachine");
  Behavior newBehavior = inputBehavior; //basicUpkeep(Cube* c, Behavior inputBehavior)

  // update sensors, numberOfNeighbors, and check wifi commands...
  //int numberOfNeighborz = checkForMagneticTagsStandard(c);
  if(MAGIC_THE_LIGHT)
  {
    for(int face = 0; face < FACES; face++)
    {
      if(face == c->returnTopFace() || face == c->returnBottomFace())
      {
        wifiDelay(1);
      }
      else
      {
        FACES_LIGHTS[face] = 1;
      }
    }
  }
  if((c->numberOfNeighbors(0) == 0) && (c->numberOfNeighbors(2) == 0))
  {
    if(MAGIC_DEBUG)
    {
       Serial.println("Erassing magic the light and all faces...");
    }
    
    MAGIC_THE_LIGHT = false;
    PART_OF_LINE = false;
    for(int face = 0; face < FACES; face++)
    {
      FACES_LIGHTS[face] = 0;
    }
  }
  /*
   * First check to see if we should try to move at all - if not, just chill for a bit
   */
  if(((TOP_FACE_LIGHT[0] < TOP_LIGHT_THRESHOLD) && (TOP_FACE_LIGHT[1] < TOP_LIGHT_THRESHOLD)) || 
      millis() < 10000)
  {
    newBehavior = CHILLING;
    c->blockingBlink(&blue);
    wifiDelay(500);
  }
  /*
   * Now we know we are supposed to start moving... so 
   */
  else
  {
    //************************************
    //NEIGHBORS == __ 0 __
    //************************************
    if (numberOfNeighborz == 0)
    {
      newBehavior = SOLO_LIGHT_TRACK;
    }
    //************************************
    //NEIGHBORS == __ 1 __
    //************************************
    else if (numberOfNeighborz == 1)
    {
      if (MAGIC_DEBUG) 
      {
        Serial.println("***NEIGHBORS == 1");
      }
      
      int neighborFace = c->whichFaceHasNeighbor();
      if (c->faces[neighborFace].returnNeighborType(0) == TAGTYPE_REGULAR_CUBE)
      {
        if (MAGIC_THE_LIGHT == true) // MAGIC_THE_LIGHT
          newBehavior = CLIMB;
        else if (c->numberOfNeighbors(3, 0) == 1) // if the last one also shows that there is a neighbor...
          newBehavior = DUO_LIGHT_TRACK;
      }
      
      else if (c->faces[neighborFace].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE)
      {
        MAGIC_THE_LIGHT = true;
        if (neighborFace == c->returnBottomFace())
        {
          newBehavior = ATTRACTIVE;
        }
        //else if ((c->returnForwardFace() == neighborFace) ||
        //         (c->returnReverseFace() == neighborFace))
        //  newBehavior = CLIMB;
        else
          newBehavior = CLIMB;
      }
    }
    //************************************
    //NEIGHBORS == __ 2 __
    //************************************
    else if (numberOfNeighborz == 2)
    {
      int first_neighborFace = c->whichFaceHasNeighbor(0);
      int second_neighborFace = c->whichFaceHasNeighbor(1);
      if (MAGIC_DEBUG) {
        Serial.println("***NEIGHBORS == 2");
      }
  
      if (MAGIC_THE_LIGHT == true) 
      {
        if (MAGIC_DEBUG) {
          Serial.println(" I SEE THE LIGHT - Don't do the next part...");
        }
        newBehavior = ATTRACTIVE;
      }
      else if (c->areFacesOpposite(first_neighborFace, second_neighborFace) == true)
      {
        c->blockingBlink(&purple, 50, 20);
        newBehavior = MULTI_LIGHT_TRACK;
      }
      else if ((c->numberOfNeighbors(5, 0) == 2) &&
               (c->numberOfNeighbors(0, 0) == 2)) // if we have been connected for a while to two cubes, 
               //and haven't seen the light
               //... we BOUNCE... disconnect
      {
        if (MAGIC_DEBUG) {
          Serial.println(" SO IT HAS COME TO THIS.... PEACE OUT BROTHERS...");
        }
        c->blockingBlink(&red, 70, 50);
        c->blockingBlink(&red, 70, 50);
        c->blockingBlink(&red, 70, 50);
        c->update();
        delay(2000);
        checkForMagneticTagsStandard(c); // give me a chance to turn it off before it explodes...
        c->moveOnLattice(&explode_F);
        newBehavior = SOLO_LIGHT_TRACK;
      }
    }
  
    //************************************
    //NEIGHBORS == __ 2+ __
    //************************************
  
    else if (numberOfNeighborz > 2)
    {
      if (MAGIC_DEBUG) {
        Serial.println("***NEIGHBORS == __ 2+ __");
      }
      newBehavior = ATTRACTIVE;
    }
  }
  return (newBehavior);
}

//=============================================================================================
//======================== *CUBE ==============================================================
//=============================================================================================

Behavior CubeStateMachine(Cube* c, Behavior inputBehavior, int neighbros)
/*
 * This code determines what to do if the goal is to TURN a line of 8 cubes into
 * one Giant Cube
 * 
*/
{
  Behavior newBehavior = inputBehavior; 
  int numberOfNeighborz = c->numberOfNeighbors(0);
  int first_neighborFace = c->whichFaceHasNeighbor(0);
  int second_neighborFace = c->whichFaceHasNeighbor(1);
  /*
   * First we check to see if we are part of a line or not...
   * bool PART_OF_LINE is a global variable keeping track of this...
   * First we check to see if any of the faces has seen part of a line...
   */
   
  /*
   * If we lose all of our neighbors... reset the light things...
   */
  if((c->numberOfNeighbors(0) == 0) && (c->numberOfNeighbors(2) == 0))
  {
    if(TOP_FACE_LIGHT[1] < TOP_LIGHT_THRESHOLD && TOP_FACE_LIGHT[0] > TOP_LIGHT_THRESHOLD)
      c->blockingBlink(&red);
  }
  /*
   * If we get triggered by the changing light...
   */
  else if((TOP_FACE_LIGHT[1] < TOP_LIGHT_THRESHOLD) && (TOP_FACE_LIGHT[0] > TOP_LIGHT_THRESHOLD))
  {
   newBehavior = CHILLING;
   /*
   * IF WE HAVE 0 NEIGHBOR...
   */
    if ((numberOfNeighborz == 0) && (c->numberOfNeighbors(2) == 0))
    {
      c->blockingBlink(&teal);
    }
    /*
    * IF WE HAVE 1 NEIGHBOR...
    */
    else if ((numberOfNeighborz == 1) && (c->numberOfNeighbors(2) == 1))
    {
      /*
       * If we have ONE neighbor, and it is on the BOTTOM, then our strategy is to just move forward
       * until we have two neighbors, or we have succeeded
       */
      if(c->returnForwardFace() != -1)
      {
        newBehavior = CLIMB;
        if(first_neighborFace == c->returnBottomFace())
        {
          if(c->returnForwardFace() != -1)
          {
            if(DIRECTION == 1)
              c->moveOnLattice(&traverse_F);
            else if(DIRECTION == -1)
              c->moveOnLattice(&traverse_R);
          }
        }
      }
      /*
       * This means we are triggered, we have 1 neighbor, and 
       * our flywheel is parallel to the ground
       */
      else if(c->returnForwardFace() == -1)
      {
        c->moveOnLattice(&horizontal_F);
      }
    }
   /*
    * IF WE HAVE 2 NEIGHBOR...
    */
    else if (numberOfNeighborz == 2)
    {
      c->blockingBlink(&teal);
    }

    /*
    * IF WE HAVE 2+ NEIGHBOR...
    */

    else if (numberOfNeighborz > 2)
    {
        wifiDelay(100);
    }
  }
  else
  {
    c->blockingBlink(&yellow);
  }
  wifiDelay(400);
  return (newBehavior);
}


/*
 * ===============================================================================================================
 * ==================*PATH STATE MACHINE==========================================================================
 * ===============================================================================================================
 */
Behavior PathStateMachine(Cube* c, Behavior inputBehavior, int neighbros)
/*
 * This code determines what to do if the goal is to get into a line...
 * Issues...
 * 
 * Variables...
 * bool PART_OF_LINE
 * int FACES_LIGHTS[FACES];
 * State Discussion...
 * First... if we think we are part of a line | SEEN THE LIGHT! | then we have two special faces...
 */
{
  Behavior newBehavior = inputBehavior; 
  int numberOfNeighborz = c->numberOfNeighbors(0);
  int first_neighborFace = c->whichFaceHasNeighbor(0);
  
  /* ----------------------------------------------------------------------------
   * ----------------------------------------------------------------------------
   * ----------------------------------------------------------------------------
   * Now we base our behavior on whether or not the light is turned on...
   * If it is turned on... we prepare to actually move. However we wait 10 seconds from when the light turned on
  */ 
    /*
  * IF WE HAVE 0 NEIGHBOR...
  */
  if ((numberOfNeighborz == 0) && (c->numberOfNeighbors(2) == 0))
  {
    newBehavior = CHILLING;
    c->blockingBlink(&teal);
  }
  if(MAGIC_THE_LIGHT)
  {
    c->lightCube(&green);
    wifiDelay(5000);
  }
  /*
  * IF WE HAVE 1 NEIGHBOR...
  */
  else if ((numberOfNeighborz == 1) && (c->numberOfNeighbors(2) == 1))
  {
    if(first_neighborFace == c->returnBottomFace())
    {
      newBehavior = FOLLOW_ARROWS;
    }
    else
    {
      newBehavior = CLIMB;
    }
  }
  
/*
  * IF WE HAVE 2 NEIGHBOR...
  */
  if ((numberOfNeighborz == 2) && (c->numberOfNeighbors(2) == 2))
  {
    //c->faces[c->returnTopFace()].turnOnFaceLEDs(1, 1, 1, 1);
    newBehavior = CHILLING;
    wifiDelay(1000);
    FACES_LIGHTS[c->returnTopFace()] = 1;
  }

  /*
  * IF WE HAVE 2+ NEIGHBOR...
  */
  else if (numberOfNeighborz > 2)
  {
    //c->faces[c->returnTopFace()].turnOnFaceLEDs(1, 1, 1, 1);
    newBehavior = CHILLING;
    FACES_LIGHTS[c->returnTopFace()] = 1;
    wifiDelay(1000);
  }

  wifiDelay(400);
  return (newBehavior);
}

/*
 * ===============================================================================================================
 * ==================*GRID STATE MACHINE==========================================================================
 * ===============================================================================================================
 */
 /*
 * Global variables related to GRID
 */
//extern bool LIGHT_TOGGLE_STATE = false;
//extern int  LIGHT_TOGGLE_TIME = 0;

Behavior GridAggregateStateMachine(Cube* c, Behavior inputBehavior, int neighbros, bool simpleNotComplicated)
/*

 */
{
  Behavior newBehavior = inputBehavior; 
  int numberOfNeighborz = c->numberOfNeighbors(0);
  int first_neighborFace = c->whichFaceHasNeighbor(0);
  
  int NSEW_lightValues[4] = {0,0,0,0}; // e.g. 
  int NSEW_faceNumbers[4] = {0,0,0,0};
  bool NSEW_neighbors[4]   = {false,false,false,false};
  int NSEWindex = 0;
  int lightThreshold = 40;
  
  if(simpleNotComplicated)
  {
    int CW_or_CCW;
    for (int face = 0; face < 6; face++)
    {
      if ((face == c->returnTopFace()) || (face == c->returnBottomFace())) // This ensures we only
      {
        c->faces[face].turnOnFaceLEDs(0,0,0,0);
        FACES_LIGHTS[face] = 0;
      }
      else
      {
        FACES_LIGHTS[face] = 1;
        c->faces[face].turnOnFaceLEDs(1,1,1,1); 
        /*
         * Here we store the face value into an array
         */
        if(NSEWindex > 3) 
        {
          NSEWindex = 3;
        }
        NSEW_faceNumbers[NSEWindex] = face;
        NSEW_lightValues[NSEWindex] = c->faces[face].returnAmbientValue(0);
        NSEW_neighbors[NSEWindex] = c->faces[face].returnNeighborPresence(0);
        //Serial.print("Face: "); Serial.print(face); Serial.print(" || "); Serial.println(c->faces[face].returnAmbientValue(0));
        NSEWindex++;
      }
    }
    bool waitingForSample = true;
    int sample;
    
    while(waitingForSample)
    {
      sample = random(5);
      if(sample == 4)
      {
        waitingForSample = false;
        c->superSpecialBlink(&yellow, 160);
        Serial.println("Just doing nothing");
      }
      else
      {
        if((NSEW_lightValues[sample] > lightThreshold) && (NSEW_neighbors[sample] == false))
        {
          waitingForSample = false;
          int faceToLight = NSEW_faceNumbers[sample];
          /*
           * ACTION - Move or turn on the light
           */
          c->lightFace(faceToLight, &purple);
          if (c->goToPlaneIncludingFaces(c->returnBottomFace(), faceToLight) == true) // then go to plane parallel to these two faces.
          {
            int CW_or_CCW = faceClockiness(faceToLight, c->returnBottomFace());
            if (CW_or_CCW == 1)
            {
              if (numberOfNeighborz == 0)
              {
                c->moveOnLattice(&traverse_F);
              }
              else if (numberOfNeighborz == 1)
              {
                c->moveOnLattice(&stepDownStair_F);
              }
            }
            else if (CW_or_CCW == -1)
            {
              if (numberOfNeighborz == 0)
              {
                c->moveOnLattice(&traverse_R);
              }
              else if(numberOfNeighborz == 1)
              {
                c->moveOnLattice(&stepDownStair_R);
              }
            }
          }
        }
      }
    }
    wifiDelay(200+random(100));
    c->lightCube(&off);
  }

/*
 * COMPLICATED VERSION ---------------
 */


  
  else // Complicated Version
  {
    for(int i = 0; i < 100; i++)
    {
    /*
     * Blink the four faces in the horizontal plane
     */
      /*
    * IF WE HAVE 0 NEIGHBOR...
    */
      //c->lightCube(&off);
  
    int currentTime = millis();
    if((currentTime - LIGHT_TOGGLE_TIME) >= 500)
    {
      LIGHT_TOGGLE_STATE = !LIGHT_TOGGLE_STATE;
      LIGHT_TOGGLE_TIME = currentTime;
      for (int face = 0; face < 6; face++)
      {
        if ((face == c->returnTopFace()) || (face == c->returnBottomFace())) // This ensures we only
        {
          c->faces[face].turnOnFaceLEDs(0,0,0,0);
        }

        // if we have a neighbor on that face....
        else
        {
          //c->faces[face].turnOnFaceLEDs(0,0,0,0);
          c->faces[face].turnOnFaceLEDs(LIGHT_TOGGLE_STATE, LIGHT_TOGGLE_STATE, LIGHT_TOGGLE_STATE, LIGHT_TOGGLE_STATE); 
        }
      }
    }

  
    
    if ((numberOfNeighborz == 0) && (c->numberOfNeighbors(2) == 0))
    {
     
    }
    /*
    * IF WE HAVE 1 NEIGHBOR...
    */
    else if ((numberOfNeighborz == 1) && (c->numberOfNeighbors(2) == 1))
    {
      
    }
  
  /*
    * IF WE HAVE 2 NEIGHBOR...
    */
    if ((numberOfNeighborz == 2) && (c->numberOfNeighbors(2) == 2))
    {
      
    }

    /*
    * IF WE HAVE 3+ NEIGHBOR...
    */
    else if (numberOfNeighborz > 2)
    {
      
    }
    wifiDelay(50+random(100));
    }
  }
  return (newBehavior);
}


Behavior sleep(Cube* c)
{
  c->blockingBlink(&red, 10);
  for (int i = 0; i < 10; i++)
  {
    Serial.println("sleep");
    delay(300);
  }
}

Behavior followArrows(Cube* c) // purple
/*
   This behavior is intended to only move ontop of a structure...
   It will exit if it isn't connected one the bottom face.
*/
{
  if (MAGIC_DEBUG) {
    Serial.println("***Beginning FOLLOWARROWS***");
  }

  Behavior nextBehavior = FOLLOW_ARROWS; // default is to keep doing what we are doing.
  int loopCounter = 0;
  nextBehavior = basicUpkeep(c, nextBehavior);  // check wifi and Magnetic Sensors
  while ((nextBehavior == FOLLOW_ARROWS) && (millis() < c->shutDownTime))
  {
    for (int i = 0; i < FACES; i ++)
    {
      if ((c->faces[i].returnNeighborAngle(0) > -1) && // If the same face shows valid angles
          (c->faces[i].returnNeighborAngle(1) > -1))// twice in a row...
      {
        int otherFace = faceArrowPointsTo(i, c->faces[i].returnNeighborAngle(0)); // find out which direction 
        //the arrow points
        if (c->goToPlaneIncludingFaces(i, otherFace) == true) // then go to plane parallel to these two faces.
        {
          int CW_or_CCW = faceClockiness(otherFace, i);
          if (CW_or_CCW == 1)
          {
            c->moveOnLattice(&traverse_F);
            delay(500);
          }
          else if (CW_or_CCW == -1)
          {
            c->moveOnLattice(&traverse_R);
            delay(500);
          }
        }
      }
    }
    loopCounter++;
    nextBehavior = basicUpkeep(c, nextBehavior);
  }
  return (nextBehavior);
}



/*
   Pre Solo Light is intented to run before we go into Solo Light Track
   The reason why this needs to exist is that if the cubes are programmed to make this the default mode
   (In the case that they restart their processors) , then
   we don't want the robots to start moving immediatly, so basically this starts blinking, and it gives the person
   who turned on the robot a chance to stop it.
*/
Behavior Pre_Solo_Light(Cube* c)
{
  if (MAGIC_DEBUG) {
    Serial.println("***ABOUT TO BEGIN SOLO_LIGHT***");
  }
  long loopCounter = 0;
  Behavior nextBehavior = PRE_SOLO_LIGHT;
  while (((nextBehavior == PRE_SOLO_LIGHT) || (nextBehavior == SOLO_LIGHT_TRACK)) && loopCounter < 3) // loop until 
  //something changes the next behavior
  {
    nextBehavior = basicUpkeep(c, nextBehavior);
    wifiDelay(100);
    c->blockingBlink(&yellow, 10, 75);
    loopCounter++;
  }
  return SOLO_LIGHT_TRACK;
}

/* Solo Light Seek is designed to have the module follow the brightest light source that it can find.
    The secondary goal is to connect to other modules.
*/
Behavior soloSeekLight(Cube* c) 
{
  // General Starting things... initialize flags, etc...
  if (MAGIC_DEBUG) {
    Serial.println("***soloSeekLight***");
  }
  Behavior nextBehavior = SOLO_LIGHT_TRACK;
  int loopCounter = 0;
  bool iAmStuck  = false;
  nextBehavior = basicUpkeep(c, nextBehavior);  // check for neighbors, etc...

  while ((nextBehavior == SOLO_LIGHT_TRACK) && // if we haven't changed state
         (c->numberOfNeighbors(0, 0) == 0)  && // and if we still have ZERO neighbors
         (millis() < c->shutDownTime))        // and if we aren't feeling sleepy
  {
    // apply sorting function to generate list of brightest faces...
    // the "true" arguement excludes the top face...
    int brightestFace = c->returnXthBrightestFace(0, true);
    int nextBrightestFace = c->returnXthBrightestFace(1, true);
    int thirdBrightestFace = c->returnXthBrightestFace(2, true);
    //
    c->lightFace(brightestFace, &green);
    wifiDelay(300);
    c->lightFace(nextBrightestFace, &teal);
    wifiDelay(300);
    c->lightFace(thirdBrightestFace, &blue);
    wifiDelay(300);
    c->lightCube(&off);
    wifiDelay(100);

    // Figure out which way we should try to move
    bool direct = false; // false = reverse...
    if (brightestFace == c->returnForwardFace())
      direct = true;
    else if (brightestFace == c->returnReverseFace())
      direct = false;
    // Ok Nothing is directly Aligned... Checking Next Brightest
    else if (nextBrightestFace == c->returnForwardFace())
      direct = true;
    else if (nextBrightestFace == c->returnReverseFace())
      direct = false;
    // Ok Checking 3rd brightest face...
    else if (thirdBrightestFace == c->returnForwardFace())
      direct = true;
    else if (thirdBrightestFace == c->returnReverseFace())
      direct = false;

    /************* Begin if else if chain **************************
        The following if/else if chain represents the choices we can take
        give that we have recently updated our information, including light sensors,
        measurement if we are stuck or not (am I stuck FLAG), etc...
       if(iAmStuck)
       This means we have previosuly tried to move and we think we are stuck
       but we have already run basic upkeep, so we will try to move the plane
       parallel with the ground to align with the lattice
    */
    if (c->currentPlaneBuffer.access(0) == PLANENONE)
    {
      c->superSpecialBlink(&red, 160);
      goToPlane(c, c->returnTopFace());
    }
    else if (iAmStuck)
    {
      // if we succeed in moving... we break out of this loop
      if (c->roll(direct, 8500) == true) // try to roll and succeed...
      {
        iAmStuck = false;
        //iMightBeStuck = false;
      }
      else
      {
        // If we fail to move, we try to move plane parallel to ground
        // int attempts = 3;
        int topFace = c->returnTopFace();
        if ((topFace > -1) && (topFace < FACES)) // if this is true we are on the ground... so we should try to 
        //change planes
        {
          goToPlane(c, topFace);
        }
        else // we are not on the ground... So we jump a little bit...
        {
          c->moveOnLattice(&traverse_F);
        }

        if (c->returnForwardFace() == -1) // this is a proxy for plane being parallel to ground... or an error
        {
          if (c->numberOfNeighborsCheckNow() == 0)
            c->moveOnLattice(&traverse_F);

          c->superSpecialBlink(&yellow, 50);
          
          wifiDelay(1500);

          if (c->numberOfNeighborsCheckNow() == 0)
            c->moveOnLattice(&traverse_R);
            
          wifiDelay(1500);
        }
      }
    }
    //****************************
    else if (c->returnForwardFace() == -1)
    {
      if (c->moveBLDCACCEL(1, GlobalMaxAccel, 1700) == false)
      {
        iAmStuck = true;
      }
      wifiDelay(100);
    }
    //**************************** this is regular light tracking...
    else
    {
      if (c->roll(direct, 9500) == false)
      {
        iAmStuck = true;
      }
    }

    //************** End if else if chain **************************
    loopCounter++;
    nextBehavior = basicUpkeep(c, nextBehavior);  // check for neighbors, etc...
  }
  return (nextBehavior);
}

/*
   This is triggered when the cube has one neighbor... and when the neighbor
   is on one of the faces other than the top and bottom faces...
   goal is to move plane to be parallel and then to climb up ontop
*/
Behavior climb(Cube* c)
{
  if (MAGIC_DEBUG) {
    Serial.println("***Beginning CLIMB***");
  }
  
  int connectedFace = -1;
  long loopCounter = 0;
  Behavior nextBehavior = CLIMB;
  nextBehavior = basicUpkeep(c, nextBehavior);

  while ((nextBehavior == CLIMB) &&
         (c->numberOfNeighbors(0, 0) == 1) && // loop until something changes the next behavior
         (millis() < c->shutDownTime))        // and if we aren't feeling sleepy
  {
    int connectedFace = c->whichFaceHasNeighbor(0);
    c->lightFace(connectedFace, &yellow);
    delay(300);
    c->lightFace(c->returnTopFace(), &red);
    delay(300);
    c->lightCube(&off);

    if ((connectedFace > -1) && (connectedFace != c->returnTopFace()) &&
        (connectedFace != c->returnBottomFace()))
    {
      if (c->isPlaneInPlaneOfFaces(connectedFace, c->returnTopFace()) == true)
      {
        delay(50);
        int CW_or_CCW = faceClockiness(connectedFace, c->returnBottomFace());
        if (CW_or_CCW == 1)
        {
          c->moveOnLattice(&cornerClimb_F);
          DIRECTION = 1;
        }
        if (CW_or_CCW == -1)
        {
          c->moveOnLattice(&cornerClimb_R);
          DIRECTION = -1;
        }
      }
      else
      {
        c->goToPlaneIncludingFaces(connectedFace, c->returnTopFace());
        wifiDelay(1000);
      }
    }
    nextBehavior = basicUpkeep(c, nextBehavior);
    loopCounter++;
    if(connectedFace == c->returnBottomFace())
    {
      nextBehavior = CHILLING;
    }
  }
  return nextBehavior;
}

//================================================================
//==========================CHILLING==============================
//================================================================

Behavior chilling(Cube* c)
{
  if (MAGIC_DEBUG) {
    Serial.println("***CHILLING***");
  }
  Behavior nextBehavior = CHILLING;
  while (nextBehavior == CHILLING) // loop until something changes the next behavior
  {
    nextBehavior = basicUpkeep(c, nextBehavior);
    wifiDelay(400);
  }
  return nextBehavior;
}

//================================================================
//==========================ACTRACTIVE==============================
//================================================================
Behavior attractive(Cube* c)
/*
   This behavior involves the cube just sitting around turning its lights on
*/
{
  Behavior nextBehavior = ATTRACTIVE;
  if (MAGIC_DEBUG) {
    Serial.println("***ATTRACTIVE***");
  }
  while (nextBehavior == ATTRACTIVE)
  {
    nextBehavior = basicUpkeep(c, nextBehavior);
    c->lightCube(&off);
    for (int face = 0; face < 6; face++)
    {
      if ((face == c->returnTopFace()) || (face == c->returnBottomFace())) // This ensures we only
        delay(1);                                                   // turn on 4 faces in horizontal plane

      // if we have a neighbor on that face....
      else if (c->faces[face].returnNeighborType(0) == TAGTYPE_REGULAR_CUBE) 
      {
        c->faces[face].turnOnFaceLEDs(1, 1, 1, 1);
      }
      else if (c->faces[face].returnNeighborPresence(0) == true) // if we have a neighbor on that face....
        delay(1);
      else
      {
        c->faces[face].turnOnFaceLEDs(1, 1, 1, 1); 
        // turns on LEDs on any exposed face thats not top/bottom/connected
      }
    }
    wifiDelay(4000);
  }
  return (nextBehavior);
}

/*----------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------- 
 *---------------------------------DUO SEEK LIGHT----------------------------------------
 *----------------------------------------------------------------------------------------------
 *----------------------------------------------------------------------------------------------
 */
Behavior duoSeekLight(Cube* c)
/*
   This tries to drive two robots together towards a light source
*/
{
  if (MAGIC_DEBUG) {
    Serial.println("***Beginning DuoSeekLight***");
  }
  Behavior nextBehavior = DUO_LIGHT_TRACK;
  int loopCounter = 0;
  bool correctPlane = false;
  int failedMoveCounter = 0;
  int connectedFace = c->whichFaceHasNeighbor(0);
  c->lightFace(connectedFace, &yellow);
  delay(200);
  c->lightFace(connectedFace, &red);
  delay(500);
  c->lightCube(&off);

  // perform basic upkeep... this involves updating sensors...
  nextBehavior = basicUpkeep(c, nextBehavior);

  /*
   * If our flywheel happens to be aligned with the ground... 
   * Then we are going to check and see if we can re-orient ourselves to face the light
   */
  if (c->returnForwardFace() == -1) // we try to nudge ourself into the correct orientation in the 
  //case that we are wrong...
  {
    for (int i = 0; i++; i < 2)
    {
      if (MAGIC_DEBUG) {
        Serial.println("TRYING ALTERNATE");
      }
      c->update();
      int brightestFace = c->returnXthBrightestFace(0, true);
      int nextBrightestFace = c->returnXthBrightestFace(1, true);
      //
      c->lightFace(brightestFace, &white);
      delay(500);
      c->lightFace(nextBrightestFace, &purple);
      delay(500);
      c->lightCube(&off);
      /*
       * Here we try to figure out which way to move
       * We see if the brightest face is the face opposite from where we are connected
       * - Meaning we are not alligned with 
       */
      if (brightestFace == oppositeFace(connectedFace))
      {
        c->blockingBlink(&purple);
        if (faceClockiness(nextBrightestFace, connectedFace) == 1)
        {
          c->printString("bldcaccel r 2000 1000");
          delay(1200);
          c->printString("bldcstop b");
        }
        else if (faceClockiness(nextBrightestFace, connectedFace) == -1)
        {
          c->printString("bldcaccel f 2000 1000");
          delay(1200);
          c->printString("bldcstop b");
        }
      }
    }
  }
  // if basic upkeep decides to change behavior, we exit now...
  // otherwise we keep running in this loop until something
  // changes the state
  while ((nextBehavior == DUO_LIGHT_TRACK) && // if we haven't changed state
         (c->numberOfNeighbors(0, 0) == 1)  && // and if we have ZERO neighbors
         (millis() < c->shutDownTime))        // and if we aren't feeling sleepy
  {
    if (correctPlane == false)
    {
      if (c->isPlaneParallel(connectedFace) == true)
      {
        correctPlane = true;
      }
      else
      {
        goToPlane(c, connectedFace);
      }
    }
    // Regular Light Tracking...
    else
    {
      int brightestFace = c->returnXthBrightestFace(0, true);
      int nextBrightestFace = c->returnXthBrightestFace(1, true);
      //
      c->lightFace(brightestFace, &yellow);
      delay(400);
      c->lightFace(nextBrightestFace, &red);
      delay(400);
      c->lightCube(&off);
      delay(100);
      // Figure out which way we should try to move
      bool direct = false; // false = reverse...
      if (brightestFace == c->returnForwardFace())
        direct = true;
      else if (brightestFace == c->returnReverseFace())
        direct = false;
      // Ok Nothing is directly Aligned... Checking Next Brightest
      else if (nextBrightestFace == c->returnForwardFace())
        direct = true;
      else if (nextBrightestFace == c->returnReverseFace())
        direct = false;
      /*
         Begin if/ else if statement tree...
      */
      if (failedMoveCounter > 2)
      {
        if ((c->returnTopFace(0) > -1) && (c->returnTopFace(0) < FACES) && (c->returnForwardFace() != -1))
        // if this is true we are on the ground... so we should try to change planes
        {
          goToPlane(c, c->returnTopFace());
        }
        else // we are not on the ground... So we jump a little bit...
        {
          c->moveOnLattice(&traverse_F);
        }
        if (c->returnForwardFace() == -1) // this is a proxy for plane being parallel to ground... or an error
        {
          c->moveOnLattice(&cornerClimb_F);
          wifiDelay(1500);
        }
      }
      else if (direct == true)
      {
        if (c->moveOnLattice(&rollDouble_F) == true)
        {
          c->superSpecialBlink(&teal, 40);
          failedMoveCounter = 0;
        }
        else
          failedMoveCounter++;
      }
      else if (direct == false)
      {
        if (c->moveOnLattice(&rollDouble_R) == true)
        {
          c->superSpecialBlink(&teal, 40);
          failedMoveCounter = 0;
        }
        else
          failedMoveCounter++;
      }
    }
    //************** End if else if chain **************************
    loopCounter++;
    nextBehavior = basicUpkeep(c, nextBehavior);  // check for neighbors, etc...
  }
  return (nextBehavior);
}

/*----------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------- 
 *---------------------------------MULTI SEEK LIGHT----------------------------------------
 *----------------------------------------------------------------------------------------------
 *----------------------------------------------------------------------------------------------
 */
Behavior multiSeekLight(Cube* c)
{
  // General Starting things... initialize flags, etc...
  if (MAGIC_DEBUG) {
    Serial.println("***Beginning MULTISeekLight***");
  }
  Behavior nextBehavior = MULTI_LIGHT_TRACK;
  int loopCounter = 0;
  bool correctPlane = false;
  int failedMoveCounter = 0;
  int connectedFace1 = c->whichFaceHasNeighbor(0);
  int connectedFace2 = c->whichFaceHasNeighbor(1);
  c->lightFace(connectedFace1, &yellow);
  wifiDelay(300);
  c->lightFace(connectedFace2, &red);
  wifiDelay(300);
  c->lightCube(&off);
  // if basic upkeep decides to change behavior, we exit now...
  // otherwise we keep running in this loop until something
  // changes the behaviour
  while ((nextBehavior == MULTI_LIGHT_TRACK) && // if we haven't changed state
         (c->numberOfNeighbors(0, 0) > 1)  && // and if we have more than one neighbor
         (millis() < c->shutDownTime))        // and if we aren't feeling sleepy
  {
    if (correctPlane == false)
    {
      if (c->isPlaneParallel(connectedFace1) == true) // if our plane is alligned with the direction we want to move
      {
        correctPlane = true;
      }
      else
      {
        goToPlane(c, connectedFace1);
      }
    }
    // Regular Light Tracking...
    else
    {
      int brightestFace = c->returnXthBrightestFace(0, true);
      int nextBrightestFace = c->returnXthBrightestFace(1, true);
      //
      c->lightFace(brightestFace, &purple);
      wifiDelay(400);
      c->lightFace(nextBrightestFace, &green);
      wifiDelay(400);
      c->lightCube(&off);
      wifiDelay(100);
      // Figure out which way we should try to move
      bool direct = false; // false = reverse...
      if (brightestFace == c->returnForwardFace())
        direct = true;
      else if (brightestFace == c->returnReverseFace())
        direct = false;
      // Ok Nothing is directly Aligned... Checking Next Brightest
      else if (nextBrightestFace == c->returnForwardFace())
        direct = true;
      else if (nextBrightestFace == c->returnReverseFace())
        direct = false;
      /*
         Begin if/ else if statement tree...
      */
      if (failedMoveCounter > 2)
      {
        if ((c->returnTopFace(0) > -1) && (c->returnTopFace(0) < FACES) && (c->returnForwardFace() != -1)) // if 
        //this is true we are on the ground... so we should try to change planes
        {
          goToPlane(c, c->returnTopFace());
        }
        else // we are not on the ground... So we jump a little bit...
        {
          c->moveOnLattice(&traverse_F);
        }
        if (c->returnForwardFace() == -1) // this is a proxy for plane being parallel to ground... or an error
        {
          c->moveOnLattice(&cornerClimb_F);
          delay(1500);
        }
      }
      else if (direct == true)
      {
        if (c->moveOnLattice(&cornerClimb_F) == true)
        {
          c->superSpecialBlink(&teal, 40);
          failedMoveCounter = 0;
        }
        else
          failedMoveCounter++;
      }
      else if (direct == false)
      {
        if (c->moveOnLattice(&cornerClimb_R) == true)
        {
          c->superSpecialBlink(&teal, 40);
          failedMoveCounter = 0;
        }
        else
          failedMoveCounter++;
      }
    }
    //************** End if else if chain **************************
    loopCounter++;
    nextBehavior = basicUpkeep(c, nextBehavior);  // check for neighbors, etc...
  }
  return (nextBehavior);
}




/*********************************************************
    Everything involving adding/removing behaviors is here...
    (1) cmdToBehaviors | converts takes in a String, and
      returns the matching behavior
    (2) behaviorsToCmd | works in the opposite way,
      converting from a Behavior into a string
    (3) CheckForBehaviors | This actually runs the behaviors
      it is running in the main loop...


     (1) and (2) are used to "pack" and "unpack" behaviors
     into strings, this is primarily used to allow cubes to
     set the behaviors of their neighbors through WIFI
*/

Behavior cmdToBehaviors(String cmd, Behavior defaultBehavior)
/*
     This function takes in a string, and if it matches to a behavior, it
     returns that behavior. If the string doesn't match, it returns
*/
{
  if (MAGIC_DEBUG) Serial.println("Running cmdToBehaviours");
  Behavior behaviorToReturn = defaultBehavior;
  if (cmd == "solo_light_track")    {
    behaviorToReturn = SOLO_LIGHT_TRACK;
  }
  else if (cmd == "duo_light_track")     {
    behaviorToReturn = DUO_LIGHT_TRACK;
  }
  else if (cmd == "follow_arrows")       {
    behaviorToReturn = FOLLOW_ARROWS;
  }
  else if (cmd == "chilling")            {
    behaviorToReturn = CHILLING;
  }
  else if (cmd == "climb")               {
    behaviorToReturn = CLIMB;
  }
  else if (cmd == "attractive")          {
    behaviorToReturn = ATTRACTIVE;
  }
  else if (cmd == "shut_down")           {
    behaviorToReturn = SHUT_DOWN;
  }
  else if (cmd == "sleep")               {
    behaviorToReturn = SLEEP;
  }
  else if (cmd == "multi_light_track")   {
    behaviorToReturn = MULTI_LIGHT_TRACK;
  }
  else if (cmd == "pre_solo_light")      {
    behaviorToReturn = PRE_SOLO_LIGHT;
  }

  return (behaviorToReturn);
}

String behaviorsToCmd(Behavior inputBehavior)
/*
     This function takes in a Behavior, and returns its name as a string.
     If the string doesn't match, it returns an empty string.
*/
{
  if (inputBehavior == SOLO_LIGHT_TRACK)
  {
    return ("solo_light_track");
  }
  else if (inputBehavior == DUO_LIGHT_TRACK)        
  {
    return ("duo_light_track");
  }
  else if (inputBehavior == FOLLOW_ARROWS)          
  {
    return ("follow_arrows");
  }
  else if (inputBehavior == CHILLING)               
  {
    return ("chilling");
  }
  else if (inputBehavior == CLIMB)                  
  {
    return ("climb");
  }
  else if (inputBehavior == ATTRACTIVE)            
  {
    return ("attractive");
  }
  else if (inputBehavior == SHUT_DOWN)             
  {
    return ("shut_down");
  }
  else if (inputBehavior == SLEEP)                 
  {
    return ("sleep");
  }
  else if (inputBehavior == MULTI_LIGHT_TRACK)     
  {
    return ("multi_light_track");
  }
  else if (inputBehavior == PRE_SOLO_LIGHT)        
  {
    return ("pre_solo_light");
  }
  // Nothing returned, so we return empty string
  return ("");
}

Behavior checkForBehaviors(Cube* c, Behavior behavior)
{
  switch (behavior) {
    case CHILLING:
      return chilling(c);
    case SOLO_LIGHT_TRACK:
      return soloSeekLight(c);
    case DUO_LIGHT_TRACK:
      return duoSeekLight(c);
    case MULTI_LIGHT_TRACK:
      return multiSeekLight(c);
    case FOLLOW_ARROWS:
      return followArrows(c);
    case CLIMB:
      return climb(c);
    case ATTRACTIVE:
      return attractive(c);
    case SLEEP:
      return sleep(c);
    case PRE_SOLO_LIGHT:
      return Pre_Solo_Light(c);
  }
}

/*
 * This prompts the cube to go to a different plane,
 * It needs to be called TWICE for it to actually work, since
 * every other time it gets called, it checks to make sure it is going to do the right thing...
 */
void goToPlane(Cube* c, int FaceToGoTo)
{
  if(DOUBLE_CHECK == true)
  {
    c->findPlaneStatus(true);
    DOUBLE_CHECK = false;
  }
  else
  {
    magicFace = FaceToGoTo;
    magicVariable = 1;
    DOUBLE_CHECK = true;
  }
}
