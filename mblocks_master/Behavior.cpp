// Commonly used commands
// c->MoveIA(&traverse_F, &buf); // makes the thing move...
//c->goToPlaneParallel(i, buf);

#include "Behavior.h"
#include "Initialization.h"     // Includes .h files for each of the "tabs" in arduino
#include "Cube.h"               // Includes .h files for each of the "tabs" in arduino
#include "Face.h"               // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"              // Includes .h files for each of the "tabs" in arduino
#include "Communication.h"      // Includes wifi
#include "Defines.h"
#include "SerialDecoder.h"

Behavior sleep(Cube* c)
{
  c->blockingBlink(&red,10);
  for(int i = 0; i < 10; i++)
  {
    Serial.println("sleep");
    delay(300);
  }
}


Behavior followArrows(Cube* c, SerialDecoderBuffer* buf)
/*
 * This behavior is intended to only move ontop of a structure...
 * It will exit if it isn't connected one the bottom face.
 */
{
  if(MAGIC_DEBUG){Serial.println("***Beginning FOLLOWARROWS***");}
  
  Behavior nextBehavior = FOLLOW_ARROWS; // default is to keep doing what we are doing.
  int loopCounter = 0;
  nextBehavior = basicUpkeep(c, nextBehavior, buf);  // check wifi and Magnetic Sensors
  while((nextBehavior == FOLLOW_ARROWS) && (millis() < c->shutDownTime))
  {
    for(int i = 0; i < FACES; i ++)
    {
      if((c->faces[i].returnNeighborAngle(0) > -1) && // If the same face shows valid angles
         (c->faces[i].returnNeighborAngle(1) > -1))// twice in a row...
      {
        int otherFace = faceArrowPointsTo(i, c->faces[i].returnNeighborAngle(0)); // find out which direction the arrow points
        if(c->goToPlaneIncludingFaces(i, otherFace, buf) == true) // then go to plane parallel to these two faces...
        {
          int CW_or_CCW = faceClockiness(otherFace, i);
          if(CW_or_CCW == 1)
          {
            c->moveIASimple(&traverse_F);
            delay(500);
          }
          else if(CW_or_CCW == -1)
          {
            c->moveIASimple(&traverse_R);
            delay(500);
          }
        }
      }
    }
    loopCounter++;
    nextBehavior = basicUpkeep(c, nextBehavior, buf);
    ////////////////////////////////
    if((loopCounter % 4) == 0)
      c->lightCube(&purple);
    else if(((loopCounter-1)%4) == 0)
      c->lightCube(&off);
    delay(300);
    ///////////////////////////////
  }
  return(nextBehavior);
}


//================================================================
//========================================================
//================================================================
Behavior Pre_Solo_Light(Cube* c, SerialDecoderBuffer* buf)
{
  if(MAGIC_DEBUG) {Serial.println("***ABOUT TO BEGIN SOLO_LIGHT***");}  
  long loopCounter = 0;
  Behavior nextBehavior = PRE_SOLO_LIGHT;
  while(nextBehavior == PRE_SOLO_LIGHT && loopCounter < 3) // loop until something changes the next behavior
  {
    nextBehavior = basicUpkeep(c, nextBehavior, buf);
    wifiDelay(100);
    c->blockingBlink(&yellow, 10, 101);

    loopCounter++;
  }
  return SOLO_LIGHT_TRACK;
}

Behavior soloSeekLight(Cube* c, SerialDecoderBuffer* buf)
{
  // General Starting things... initialize flags, etc...
  if(MAGIC_DEBUG) {Serial.println("***soloSeekLight***");}
  Behavior nextBehavior = SOLO_LIGHT_TRACK;
  int loopCounter = 0;
  bool iMightBeStuck  = false;
  bool iAmStuck  = false;
  int direction = 0; // + is forward - is reverse
  
  // perform basic upkeep... this involves updating sensors...
  nextBehavior = basicUpkeep(c, nextBehavior, buf);
  
  // if basic upkeep decides to change behavior, we exit now...
  // otherwise we keep running in this loop until something 
  // changes the state
  while((nextBehavior == SOLO_LIGHT_TRACK) && // if we haven't changed state
        (c->numberOfNeighbors(0,0) == 0)  && // and if we have ZERO neighbors
        (millis() < c->shutDownTime))        // and if we aren't feeling sleepy
  {
    // apply sorting function to generate list of brightest faces...
    // the "true" arguement excludes the top face...
    int brightestFace = c->returnXthBrightestFace(0, true);
    int nextBrightestFace = c->returnXthBrightestFace(1, true);
    int thirdBrightestFace = c->returnXthBrightestFace(2, true);
    //
    c->lightFace(brightestFace, &green);
    delay(350);
    c->lightFace(nextBrightestFace, &red);
    delay(350);
    c->lightFace(thirdBrightestFace, &blue);
    delay(350);
    c->lightCube(&off);
    delay(100);
    
    // Figure out which way we should try to move
    bool direct = false; // false = reverse...
    if(brightestFace == c->returnForwardFace())
      direct = true;
    else if(brightestFace == c->returnReverseFace())
      direct = false;
      // Ok Nothing is directly Aligned... Checking Next Brightest
    else if(nextBrightestFace == c->returnForwardFace())
      direct = true;
    else if(nextBrightestFace == c->returnReverseFace())
      direct = false;
          // Ok Checking 3rd brightest face...
    else if(thirdBrightestFace == c->returnForwardFace())
      direct = true;
    else if(thirdBrightestFace == c->returnReverseFace())
      direct = false;
    
    
    /************* Begin if else if chain **************************
     *  The following if/else if chain represents the choices we can take
     *  give that we have recently updated our information, including light sensors,
     *  measurement if we are stuck or not (am I stuck FLAG), etc...
     */

    /* if(iAmStuck)
     * This means we have previosuly tried to move and we think we are stuck
     * but we have already run basic upkeep, so we will try to move the plane 
     * parallel with the ground to align with the lattice
     */
    if(c->findPlaneStatus(true) == PLANENONE)
    {
      c->superSpecialBlink(&red, 200);
      c->superSpecialBlink(&red, 180);
      c->superSpecialBlink(&red, 160);
      c->superSpecialBlink(&red, 120);
      magicFace = c->returnTopFace(); 
      magicVariable = 1;
    }
    else if(iAmStuck)
    {
      // if we succeed in moving... we break out of this loop
      if(c->roll(direct, buf, 8000) == true) // try to roll and succeed...
      {
        iAmStuck = false; 
        iMightBeStuck = false;
      }
      else
      {
        // If we fail to move, we try to move plane parallel to ground
        //int attempts = 3;
        int topFace = c->returnTopFace();
        if((topFace > -1) && (topFace < FACES)) // if this is true we are on the ground... so we should try to change planes
        {
          magicFace = topFace; 
          magicVariable = 1; // this triggers a plane change later in the program...
        }
        else // we are not on the ground... So we jump a little bit...
        {
          c->moveIASimple(&traverse_F);
        }
        
        if(c->returnForwardFace() == -1) // this is a proxy for plane being parallel to ground... or an error
        {
           c->superSpecialBlink(&yellow, 50);
           c->moveIASimple(&traverse_F);
           delay(1500);
           if(c->numberOfNeighborsCheckNow() == 0)
           c->moveIASimple(&traverse_R);
           delay(1500);
        }
      } 
    }
    
    /* 
     *  this happens if we suspect we might be stuck...
     *  we will try to move, and if we fail
     *  then we set the stuck flag to be true,
     */
    //****************************
    else if(iMightBeStuck)
    {
      iMightBeStuck = false; // reset flag
      
      if(c->returnForwardFace() == -1)
      {
        if(c->roll(1,buf,1800,"bldcaccel f 6000 1500") == false)
          iAmStuck = true;
      }
      else // try to roll... if we fail... advance to plane changing, etc...
      {
        if(c->roll(direct, buf, 10000) == false)
        {
          delay(10);
          iAmStuck = true;
          iMightBeStuck = true;
        }
      }
    }
    //****************************
    else if(c->returnForwardFace() == -1)
    {
      if(c->roll(1,buf,1800,"bldcaccel f 6000 1500") == false)
        iMightBeStuck = true;
      delay(100);
    }
    //**************************** this is regular light tracking...
    else
    {
      if(c->roll(direct, buf, 9500) == false)
      {
        iMightBeStuck = true;
        
      }
    }
 
//************** End if else if chain **************************
    loopCounter++;
    delay(200);
    nextBehavior = basicUpkeep(c, nextBehavior, buf);  // check for neighbors, etc...
  }
  return(nextBehavior);
}

/*
 * This is triggered when the cube has one neighbor... and when the neighbor
 * is on one of the faces other than the top and bottom faces...
 * goal is to move plane to be parallel and then to climb up ontop
 * 
 */
Behavior climb(Cube* c, SerialDecoderBuffer* buf)
{
  if(MAGIC_DEBUG) {Serial.println("***Beginning CLIMB***");}
  
  int connectedFace = -1;
  long loopCounter = 0;
  Behavior nextBehavior = CLIMB;
  nextBehavior = basicUpkeep(c, nextBehavior, buf);
 
  while(nextBehavior == CLIMB) // loop until something changes the next behavior
  {
    nextBehavior = basicUpkeep(c, nextBehavior, buf);
    if(c->numberOfCubeNeighbors() == 1)
    {
      if(MAGIC_DEBUG) {Serial.println("We Have one CUBE neighbor, starting to see how to MOVE!");}
      for(int i = 0; i < 6; i++)
      {     
        if((c->faces[i].returnNeighborType(0) == TAGTYPE_REGULAR_CUBE) ||
           (c->faces[i].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE))//
        {
          if(MAGIC_DEBUG) {Serial.print("connected Face is !!: "); Serial.println(i);}
          connectedFace = i;
        }
      }
      c->lightFace(connectedFace,&yellow);
      delay(200);
      c->lightFace(c->returnTopFace(),&red);
      delay(200);
      c->lightCube(&off);
      
      if((connectedFace >= 0) && (connectedFace != c->returnTopFace()) &&
         (connectedFace != c->returnBottomFace()))
      {
        if(MAGIC_DEBUG) {Serial.println("ABOUT TO TRY TO CHANGE PLANES... WOOO!");}
        
        if(c->goToPlaneIncludingFaces(connectedFace, c->returnTopFace(), buf))
        {
          delay(50);
          int CW_or_CCW = faceClockiness(connectedFace, c->returnBottomFace());
          if(CW_or_CCW == 1)
            c->moveIASimple(&cornerClimb_F);
          if(CW_or_CCW == -1)
            c->moveIASimple(&cornerClimb_R);
        }
      }      
    }
    ////////////////////////////////
    c->superSpecialBlink(&yellow, 100);
    c->superSpecialBlink(&teal, 50);
    delay(300);
    ///////////////////////////////
    loopCounter++;
  }
  return nextBehavior;
}

//================================================================
//==========================CHILLING==============================
//================================================================
Behavior chilling(Cube* c, SerialDecoderBuffer* buf)
{
  if(MAGIC_DEBUG) {Serial.println("***CHILLING***");}  
  long loopCounter = 0;
  Behavior nextBehavior = CHILLING;
  while(nextBehavior == CHILLING) // loop until something changes the next behavior
  {
    nextBehavior = basicUpkeep(c, nextBehavior, buf);
    wifiDelay(400);
    
    if(loopCounter%5)
      c->lightCube(&blue);
    else if((loopCounter-1)%5)
      c->lightCube(&off);
    delay(10);
    
    loopCounter++;
  }
  return nextBehavior;
}

//================================================================
//==========================ACTRACTIVE==============================
//================================================================
Behavior attractive(Cube* c, SerialDecoderBuffer* buf)
{
  Behavior nextBehavior = ATTRACTIVE;
  if(MAGIC_DEBUG) {Serial.println("***ATTRACTIVE***");}   

  while(nextBehavior == ATTRACTIVE)
  {
    nextBehavior = basicUpkeep(c, nextBehavior, buf, 0, false, false); // don't blink the lights... they will be on...
    delay(100);
    
    c->lightCube(&off);
    delay(100);
    for(int i = 0; i < 6; i++)
      {
        if((i == c->returnTopFace()) || (i == c->returnBottomFace())) // This ensures we only 
          delay(1);                                              // turn on 4 faces in horizontal plane
          //break;
        else if(c->faces[i].returnNeighborType(0) == TAGTYPE_REGULAR_CUBE) // if we have a neighbor on that face....
          {
            c->faces[i].turnOnFaceLEDs(1,1,1,1); 
            delay(4000);
          }
        else if(c->faces[i].returnNeighborPresence(0) == true) // if we have a neighbor on that face....
          delay(1);
        else
          {
            c->faces[i].turnOnFaceLEDs(1,1,1,1); // turns on LEDs on any exposed face thats not top/bottom/connected
            delay(150);
          }
      }
    wifiDelay(6500); // delay 10 seconds...
  }
  for(int i = 0; i < 6; i++)
  {
    c->faces[i].turnOffFaceLEDs(); // turn off face LEDs
  }
  return(nextBehavior);
}

Behavior duoSeekLight(Cube* c, SerialDecoderBuffer* buf)
{
 // General Starting things... initialize flags, etc...
  if(MAGIC_DEBUG) {Serial.println("***DuoSeekLight***");}
  Behavior nextBehavior = DUO_LIGHT_TRACK;
  int loopCounter = 0;
  bool correctPlane = false;
  bool iMightBeStuck  = false;
  bool iAmStuck  = false;
  int connectedFace = c->whichFaceHasNeighbor();
  
  // perform basic upkeep... this involves updating sensors...
  nextBehavior = basicUpkeep(c, nextBehavior, buf, 0, false, false);
  
  // if basic upkeep decides to change behavior, we exit now...
  // otherwise we keep running in this loop until something 
  // changes the state
  while((nextBehavior == DUO_LIGHT_TRACK) && // if we haven't changed state
        (c->numberOfNeighbors(0,0) == 1)  && // and if we have ZERO neighbors
        (millis() < c->shutDownTime))        // and if we aren't feeling sleepy
  {
    // try to get into the correct plane...
    if((correctPlane == false) && (c->goToPlaneParallel(connectedFace) == true))
    {
      correctPlane = true;
      c->lightFace(connectedFace,&yellow);
      delay(500);
      c->lightFace(connectedFace,&red);
      delay(500);
    }
    // Regular Light Tracking...
    else
    {
      int brightestFace = c->returnXthBrightestFace(0, true);
      int nextBrightestFace = c->returnXthBrightestFace(1, true);
      //
      c->lightFace(brightestFace, &white);
      delay(500);
      c->lightFace(nextBrightestFace, &teal);
      delay(500);
      c->lightCube(&off);
      delay(100);
      // Figure out which way we should try to move
      bool direct = false; // false = reverse...
      if(brightestFace == c->returnForwardFace())
        direct = true;
      else if(brightestFace == c->returnReverseFace())
        direct = false;
      // Ok Nothing is directly Aligned... Checking Next Brightest
      else if(nextBrightestFace == c->returnForwardFace())
        direct = true;
      else if(nextBrightestFace == c->returnReverseFace())
        direct = false;

      if(direct == true)
      {
        c->moveIASimple(&cornerClimb_F);
        delay(1000);
      }
      else
      {
        c->moveIASimple(&cornerClimb_R);
        delay(1000);
      }
    }    
//************** End if else if chain **************************
    loopCounter++;
    delay(200);
    nextBehavior = basicUpkeep(c, nextBehavior, buf);  // check for neighbors, etc...
    c->superSpecialBlink(&blue, 150);
    c->superSpecialBlink(&teal, 150);
    c->superSpecialBlink(&green, 150);
  }
  return(nextBehavior);
}

//================================================================
//=================CRYSTALIZE==============================
//================================================================
/**
 * Cubes in the crystalize mode
 *
 * A single cube is nominated
 *
 * Extra "arrow directions" that can be used to communicate crystallization direction:
 *   4 - into center of cube
 *   5 - out of center of cube
 */
Behavior crystalize(Cube* c, painlessMesh* m)
{
  
}


Behavior basicUpkeep(Cube* c, Behavior currentBehaviorNew, SerialDecoderBuffer* buf, int lightDigit, bool checkForLightMessages, bool blinkLEDs)
/*
 * This function does basic state machine switching
 * It (1) Updates the sensors, including magnetic sensors
 * light sensors, and IMU's
 * 
 * Then it checks the wifi BUFFER and checks the magnetic tags
 * for actionable configurations
 */
{
  Behavior behaviorToReturnWIFI = currentBehaviorNew;
  Behavior behaviorToReturnFinal = currentBehaviorNew;
  /*
   * Light Digit Information...
   * Light Digits 1,2,3,4 - reserved for direction finding...
   * Light Digit 5-6 = disconnect, try again,
   * Light Digit 7-8 = Enter Duo Light Track mode
   * Light Digit 9+  = go to attractive or Climb Mode...
   */
  // Set light Digits BEFORE c-> Update Sensors... we might be a bit late...
  int noOfNeighbors = c->numberOfNeighborsCheckNow();
  if(MAGIC_DEBUG) Serial.println("WOO!");
  if(noOfNeighbors == 1)
    lightDigit = 7;
  if(noOfNeighbors > 1)
    lightDigit = 9;

  c->updateSensors(lightDigit, checkForLightMessages, blinkLEDs);
  //Now we might have updated our Light Digits...
  behaviorToReturnWIFI = checkForBasicWifiCommands(c, currentBehaviorNew, buf);
  delay(100);
  behaviorToReturnFinal = checkForMagneticTagsStandard(c, behaviorToReturnWIFI, buf);
  // Evaluate any Light Digits...
  for(int face = 0; face < 6; face++)
  {
    if(MAGIC_DEBUG)
    {
      Serial.print("NeighborLight Digit on face: ");
      Serial.print(face);
      Serial.print(" is: ");
      Serial.println(c->faces[face].returnNeighborLightDigit(0));
    }
    if(c->faces[face].returnNeighborLightDigit(0) > 0)
    {
      if((c->faces[face].returnNeighborLightDigit(0) == 5) || (c->faces[face].returnNeighborLightDigit(0) == 6))
      {
        c->superSpecialBlink(&white,200);
        c->superSpecialBlink(&white,150);
        c->superSpecialBlink(&white,100);
        c->moveIASimple(&explode_F);
        behaviorToReturnFinal = SOLO_LIGHT_TRACK;
      }
      else if((c->faces[face].returnNeighborLightDigit(0) == 7) || (c->faces[face].returnNeighborLightDigit(0) == 8))
      {
        behaviorToReturnFinal = DUO_LIGHT_TRACK;
      }
      else if(c->faces[face].returnNeighborLightDigit(0) == 10)
      {
        behaviorToReturnFinal = CLIMB;
      }
    }
  }
  
  if(MAGIC_DEBUG) 
  { 
    Serial.println("-------------------------------------------");
    Serial.println("Ending Basic Upkeep, here is what we found:");
    Serial.print("Top face: ");           Serial.println(c->returnTopFace());
    Serial.print("Current Plane: ");      Serial.println(c->returnCurrentPlane_STRING());
    Serial.print("forward Face ");        Serial.println(c->returnForwardFace());
    Serial.print("# of Neighbors: ");     Serial.println(c->numberOfNeighbors(0,0));
    Serial.print("Resultalt Behavior: "); Serial.println(behaviorsToCmd(behaviorToReturnFinal));
    Serial.println("-------------------------------------------");
  }
  
  return(behaviorToReturnFinal);
}

//=============================================================================================
//=============================WIFI Checking  CHECKING=========================================
//=============================================================================================
Behavior checkForBasicWifiCommands(Cube* c, Behavior currentBehavior, SerialDecoderBuffer* buf)
{
  if(MAGIC_DEBUG) {Serial.println("Checking for basic WIFI Commands...");}
  
  int attempts = 5;
  Behavior resultBehavior = currentBehavior;
  while(!jsonCircularBuffer.empty() && attempts > 0) //while there are still messages, and we haven't tried 5 times
  {
    StaticJsonBuffer<1000> jb;
    JsonObject& root = jb.parseObject(jsonCircularBuffer.pop());
    if((root["cubeID"] == getCubeIDFromEsp(ESP.getChipId())) || // If message matches your ID
       (root["cubeID"] == -1))                                  // or if message is brodcast
    {
      resultBehavior = cmdToBehaviors(root["cmd"], currentBehavior); // checks to see if we recieved message 
                                                                    //to switch behaviors
      if(c->cubeID > 40) // cubeID's over 40 means it is attached by a cable... not a real cube // so we print
      {
        String receivedID = root["cubeID"];
        String receivedCMD = root["cmd"];
        String messageString = "Message: From: " + receivedID + " Command is: " + receivedCMD;// + " Command is: ";
        Serial.println(messageString);
      }
    }
    attempts--;
  }
  return(resultBehavior);
}

//=============================================================================================
//=============================MAGNETIC TAGS CHECKING==========================================
//=============================================================================================
//bool returnNeighborPresence(int index);
//int returnNeighborID(int index);
//int returnNeighborAngle(int index);
//int returnNeighborFace(int index);
//TagType returnNeighborType(int index);
//TagCommand returnNeighborCommand(int index);
    
Behavior checkForMagneticTagsStandard(Cube* c, Behavior currentBehavior, SerialDecoderBuffer* buf)
{
  if(MAGIC_DEBUG) {Serial.println("Checking for MAGNETIC TAGS");}
  
  Behavior resultBehavior = currentBehavior;
  for(int i = 0; i < 6; i++)
    { 
      /*
       * If we are attached to an "arrow" 
       * we breifly blink the direction... no state change
       */
      if(c->faces[i].returnNeighborAngle(0) != -1) // This means we are seeing some "arrow"
      {
        c->lightFace(faceArrowPointsTo(i, c->faces[i].returnNeighborAngle(0)),&white);
        delay(100);
        c->lightsOff();
      }

      /* This gets activated if we are attached to an actual cube or passive cube
       * for at least two time steps...
       */
      if(((c->faces[i].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE) &&
          (c->faces[i].returnNeighborType(1) == TAGTYPE_PASSIVE_CUBE)))  
      {
        if(i == c->returnBottomFace())
        {
          resultBehavior = FOLLOW_ARROWS;
        }
        else if((c->returnForwardFace() == i) ||
                (c->returnReverseFace() == i)) 
          resultBehavior = CLIMB;
        else
          resultBehavior = ATTRACTIVE;
      }

      // If we are connected to another active cube...
      if((c->faces[i].returnNeighborType(0) == TAGTYPE_REGULAR_CUBE) &&
         (c->faces[i].returnNeighborType(1) == TAGTYPE_REGULAR_CUBE) &&
         (c->faces[i].returnNeighborType(2) == TAGTYPE_REGULAR_CUBE) &&
         (currentBehavior != DUO_LIGHT_TRACK))  
      {
        if(c->returnBottomFace() == i)
          resultBehavior = FOLLOW_ARROWS;
          
        else if(c->isFaceNeitherTopNorBottom(i)) // check the light digit...
          {      
            resultBehavior = CLIMB;
          }
      }


      /*
       * The following is the behavior we want to trigger if we see a specific
       * tag.
       */
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_SLEEP)
      {
        c->blockingBlink(&red,10);
        resultBehavior = relayBehavior(c, SLEEP);
      }
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_PURPLE ||
        (magicVariable == 1))
      {
        int z = i;
        if(magicVariable)
        {
          z = magicFace;
          magicFace = 0;
        }
        magicVariable = 0;
        
        //c->lightCube(&purple);
        //====================SEND DEBUG =====================    
        if(MAGIC_DEBUG){Serial.println("Starting to try to change planes...");}
        c->goToPlaneParallel(z);
        //mesh.sendBroadcast(Str);
        //==================END SEND DEBUG ===================  
      }
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_27)
      {
        resultBehavior = SOLO_LIGHT_TRACK;
        if((c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_27) &&
           (c->faces[i].returnNeighborCommand(2) == TAGCOMMAND_27) &&
           (c->faces[i].returnNeighborCommand(6) == TAGCOMMAND_27))
 
           {
              resultBehavior = relayBehavior(c, SOLO_LIGHT_TRACK);
           }
        
      }
      
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_23)
      {
        for(int times = 0; times < 5; times++)
        {
          Serial.println("sleep");
          delay(500);
        }
      }
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_13_ESPOFF)
      {
        for(int times = 0; times < 5; times++)
        {
          Serial.println("espoff");
          delay(500);
        }
      }
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_19)
      {
//        //====================SEND DEBUG =================== 
//        StaticJsonBuffer<512> jsonBuffer; //Space Allocated to store json instance
//        JsonObject& root2 = jsonBuffer.createObject(); // & is "c++ reference"
//        String message =  "  ID#: " + String(ESP.getChipId()) +
//                          "  on Face: " + String(i) + "  " +
//                          "  t.type: " + String(t.type) +
//                          "  t.angle: " + String(t.angle) +
//                          "  t.id: " + String(t.id) +
//                          "  t.face: " + String(t.face) +
//                          //"  t.Strength: " + String(t.strength) +
//                          "  t.command: " + String(t.command) +
//                          " --- " +
//                          " A0: " +  String(c->faces[i].returnMagnetAngle_A(0)) + 
//                          " S0: " +  String(c->faces[i].returnMagnetStrength_A(0)) + 
//                          " A1: " +  String(c->faces[i].returnMagnetAngle_B(0)) + 
//                          " S1: " +  String(c->faces[i].returnMagnetStrength_B(0));   
//                         // " Reflectivity: " + String(reflect);                    
//                          
//        root2["msg"] = message;       
//        root2["cmd"]  = "debugMSG";  
//        root2["cubeID"] = -1;
//        String newStr;
//        root2.printTo(newStr); 
//        mesh.sendBroadcast(newStr);
        //====================END SEND DEBUG ===================       
      }
   }
   /*
    * Logic Involving more than one cube...
    */    
  if(MAGIC_DEBUG) {Serial.print("Starting Multi Cube Logic...");}
  if((c->numberOfNeighbors(0,0) >= 2) && (resultBehavior != SLEEP)  && (resultBehavior != CHILLING))
  {  
     bool areAnyTagsCommands = false;
     for(int i = 0; i < 6; i++)
     {
        if(c->faces[i].returnNeighborType(0) == TAGTYPE_COMMAND)
        {
          areAnyTagsCommands = true;
        }
     }
     if(areAnyTagsCommands == false)
     {
        resultBehavior = ATTRACTIVE;
     }
  }
  if(MAGIC_DEBUG) {Serial.print("Result behavior is: ");Serial.println(behaviorsToCmd(resultBehavior));}
  return(resultBehavior);
}

Behavior relayBehavior(Cube* c, Behavior behaviorToRelay, int cubeToRelayTo, int timesToRelay)
{
  //======Temporarily Generated a Broadcast message =========
  StaticJsonBuffer<512> jsonBuffer; //Space Allocated to store json instance
  JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
  //^class type||^ Root         ^class method                   
  root["type"] = "cmd";
  root["cubeID"] = cubeToRelayTo;
  root["cmd"] = behaviorsToCmd(behaviorToRelay);
  //^ "key"   |  ^ "Value"
  String str; // generate empty string
  root.printTo(str); // print to JSON readable string...
  //======== End Generating of Broadcast message ==========
  
  for(int i = 0; i < timesToRelay; i++);
  {
    mesh.sendBroadcast(str);
    wifiDelay(400);
  }
  return(behaviorToRelay);
}


/*********************************************************
 *  Everything involving adding/removing behaviors is here...
 *  (1) cmdToBehaviors | converts takes in a String, and 
 *    returns the matching behavior
 *  (2) behaviorsToCmd | works in the opposite way,
 *    converting from a Behavior into a string
 *  (3) CheckForBehaviors | This actually runs the behaviors
 *    it is running in the main loop...
 *   
 *   
 *   (1) and (2) are used to "pack" and "unpack" behaviors
 *   into strings, this is primarily used to allow cubes to 
 *   set the behaviors of their neighbors through WIFI
 */

Behavior cmdToBehaviors(String cmd, Behavior defaultBehavior)
/*  
 *   This function takes in a string, and if it matches to a behavior, it 
 *   returns that behavior. If the string doesn't match, it returns
 */
{
  Behavior behaviorToReturn = defaultBehavior;
       if(cmd == "solo_light_track")    {behaviorToReturn = SOLO_LIGHT_TRACK;}
  else if(cmd == "duo_light_track")     {behaviorToReturn = DUO_LIGHT_TRACK;}
  else if(cmd == "follow_arrows")       {behaviorToReturn = FOLLOW_ARROWS;}
  else if(cmd == "chilling")            {behaviorToReturn = CHILLING;}
  else if(cmd == "climb")               {behaviorToReturn = CLIMB;}
  else if(cmd == "attractive")          {behaviorToReturn = ATTRACTIVE;}
  else if(cmd == "shut_down")           {behaviorToReturn = SHUT_DOWN;}
  else if(cmd == "sleep")               {behaviorToReturn = SLEEP;}
  else if(cmd == "pre_solo_light")      {behaviorToReturn = PRE_SOLO_LIGHT;}
return(behaviorToReturn);
}

String behaviorsToCmd(Behavior inputBehavior)
/*  
 *   This function takes in a string, and if it matches to a behavior, it 
 *   returns that behavior. If the string doesn't match, it returns
 */
{
  String stringToReturn = " ";
       if(inputBehavior == SOLO_LIGHT_TRACK)      {stringToReturn = "solo_light_track";} 
  else if(inputBehavior == DUO_LIGHT_TRACK)       {stringToReturn = "duo_light_track";}
  else if(inputBehavior == FOLLOW_ARROWS)         {stringToReturn = "follow_arrows";}
  else if(inputBehavior == CHILLING)              {stringToReturn = "chilling";}
  else if(inputBehavior == CLIMB)                 {stringToReturn = "climb";}
  else if(inputBehavior == ATTRACTIVE)            {stringToReturn = "attractive";}
  else if(inputBehavior == SHUT_DOWN)             {stringToReturn = "shut_down";}
  else if(inputBehavior == SLEEP)                 {stringToReturn = "sleep";}
  else if(inputBehavior == PRE_SOLO_LIGHT)        {stringToReturn = "pre_solo_light";}
return(stringToReturn);
}

Behavior checkForBehaviors(Cube* c, SerialDecoderBuffer* buf, Behavior behavior)
{
  c->behaviorBuffer.push(behavior); // adds the switch into a buffer
  if (behavior == SOLO_LIGHT_TRACK)
    behavior = soloSeekLight(c, buf);
  else if (behavior == DUO_LIGHT_TRACK)
    behavior = duoSeekLight(c, buf);
  else if (behavior == FOLLOW_ARROWS)
    behavior = followArrows(c, buf);
  else if (behavior == CLIMB)
    behavior = climb(c, buf);
  else if (behavior == CHILLING)
    behavior = chilling(c, buf);
  else if (behavior == ATTRACTIVE)
    behavior = attractive(c, buf);
  else if (behavior == SLEEP)
    behavior = sleep(c);
  else if (behavior == PRE_SOLO_LIGHT)
    behavior = Pre_Solo_Light(c, buf);
  else
  {
    //Serial.println("ERROR: unknown behavior.  Reverting to \"CHILLING\"");
    behavior = CHILLING;
  }
  return(behavior);
}
