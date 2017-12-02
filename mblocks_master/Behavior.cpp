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
//================================================================
//==========================Colors================================
//================================================================
Behavior Yellow(Cube* c, SerialDecoderBuffer* buf)
{
  if(MAGIC_DEBUG) {Serial.println("Teal");}
  Behavior nextBehavior = YELLOW;
  while((basicUpkeep(c, nextBehavior, buf) == YELLOW) && (millis() < c->shutDownTime))
  {
    c->lightCube(&yellow);
    delay(1000);
  }
}
/////
/////
Behavior Purple(Cube* c, SerialDecoderBuffer* buf)
{
  if(MAGIC_DEBUG) {Serial.println("Teal");}
  Behavior nextBehavior = PURPLE;
  while((basicUpkeep(c, nextBehavior, buf) == PURPLE) && (millis() < c->shutDownTime))
  {
    c->lightCube(&purple);
    delay(1000);
  }
}
/////
/////
Behavior Teal(Cube* c, SerialDecoderBuffer* buf)
{
  if(MAGIC_DEBUG) {Serial.println("Teal");}
  Behavior nextBehavior = TEAL;
  while((basicUpkeep(c, nextBehavior, buf) == TEAL) && (millis() < c->shutDownTime))
  {
    c->lightCube(&teal);
    delay(1000);
  }
}
/////
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
{
  if(MAGIC_DEBUG){Serial.println("***followArrows***");}
  Behavior nextBehavior = FOLLOW_ARROWS; // default is to keep doing what we are doing.
  int loopCounter = 0;
  nextBehavior = basicUpkeep(c, nextBehavior, buf);  
  while((nextBehavior == FOLLOW_ARROWS) && (millis() < c->shutDownTime))
  {
    for(int i = 0; i < FACES; i ++)
    {
      if((c->faces[i].returnNeighborAngle(0) > -1) && // If the same face shows valid angles
         (c->faces[i].returnNeighborAngle(1) > -1))
      {
        int otherFace = faceArrowPointsTo(i, c->faces[i].returnNeighborAngle(0));
        if(c->goToPlaneIncludingFaces(i, otherFace, buf) == true) // then go to plane parallel
        {
          int CW_or_CCW = faceClockiness(otherFace, i);
          if(CW_or_CCW == 1)
          {
            delay(1000);
            c->moveIASimple(&traverse_F);
            delay(1000);
          }
          else if(CW_or_CCW == -1)
          {
            delay(1000);
            c->moveIASimple(&traverse_R);
            delay(1000);
          }
        }
      }
    }
    loopCounter++;
    nextBehavior = basicUpkeep(c, nextBehavior, buf);
    ////////////////////////////////
    if(loopCounter%4)
      c->lightCube(&purple);
    else if((loopCounter-1)%4)
      c->lightCube(&off);
    delay(300);
    ///////////////////////////////
  }
  return(nextBehavior);
}

Behavior soloSeekLight(Cube* c, SerialDecoderBuffer* buf)
{
  // General Starting things... initialize flags, etc...
  if(MAGIC_DEBUG) {Serial.println("***soloSeekLight***");}
  Behavior nextBehavior = SOLO_LIGHT_TRACK;
  int loopCounter = 0;
  bool iAmPrettyStuck = false;
  bool iMightBeStuck  = false;
  
  // perform basic upkeep... this involves updating sensors...
  nextBehavior = basicUpkeep(c, nextBehavior, buf);

  // if basic upkeep decides to change behavior, we exit now...
  // otherwise we keep running in this loop until something 
  // changes the state
  while(nextBehavior == SOLO_LIGHT_TRACK && (c->numberOfNeighbors(0,0) == 0))
  {
    // apply sorting function to generate list of brightest faces...
    // the "true" arguement excludes the top face...
    int brightestFace = c->returnXthBrightestFace(0, true);
    int nextBrightestFace = c->returnXthBrightestFace(1, true);
    int thirdBrightestFace = c->returnXthBrightestFace(2, true);

    c->lightFace(brightestFace, &green);
    delay(500);
    c->lightFace(nextBrightestFace, &teal);
    delay(400);
    c->lightFace(thirdBrightestFace, &blue);
    delay(400);
    c->lightCube(&off);
    delay(200);
    /************* Begin if else if chain **************************
     *  The following if/else if chain represents the choices we can take
     *  give that we have recently updated our information, including light sensors,
     *  measurement if we are stuck or not (am I stuck FLAG), etc...
     */

    /*
     * This means we have previosuly tried to move and we think we are stuck
     * but we have already run basic upkeep, so we will try to move the plane 
     * parallel with the ground to align with the lattice
     */
    if(iAmPrettyStuck)
    {
      // if we succeed in moving... we break out of this loop
      if(c->roll(-1, buf, 8000)) // try to roll and succeed...
      {
        iAmPrettyStuck = false; 
        iMightBeStuck = false;
        c->blockingBlink(&white);
      }
      else
      {
        // If we fail to move, we try to move plane parallel to ground
        //int attempts = 3;
        delay(1000);
        int topFace = c->returnTopFace();
        bool changed = false;
        if(topFace > 0 && topFace < FACES)
        {
          //changed = c->goToPlaneParallel(topFace);
          magicFace = topFace; // this kinda does this...
          magicVariable = 1;
        }
        if(c->returnForwardFace() == -1)
        {
           c->moveIASimple(&traverse_F);
           delay(2000);
           c->moveIASimple(&traverse_R);
           delay(2000);
        }
        else
        {
          c->blockingBlink(&red,20);
        }
        //if(!changed)
        //{
         // magicVariable = 1;
          
          //delay(1000);
          //c->updateSensors();
          //delay(3000);
         // c->goToPlaneParallel(topFace);
        //}
      } 
    }
    
    /* 
     *  this happens if we suspect we might be stuck...
     *  we will try three times to move, and if each one fails
     *  then we set the stuck flag to be true,
     */
    else if(iMightBeStuck)
        {
          //iAmPrettyStuck = true;
          iMightBeStuck = false; // reset flag
          if(!(c->roll(-1, buf, 8000))) // try to roll if we fail... advance to more change plane
            {
              delay(10);
              c->blinkAmerica();
              iAmPrettyStuck = true;
              iMightBeStuck = true;
            }
          delay(1000);
             //we try to move again! and if so... we will start something else...
         // if(!c->moveIASimple(&softShake_F))
          //{
           // c->blinkRainbow();
//            iAmPrettyStuck = true;
//            iMightBeStuck = true;
          //}  
        }
    else if(c->returnForwardFace() == -1)
    {
      Serial.println("bldcaccel f 6000 1500");
      delay(1600);
      Serial.println("bldcstop b");
      delay(6000);
      iMightBeStuck = true;
    }
    else if(brightestFace == c->returnForwardFace())
    {
      if(!c->roll(1, buf, 11000))
        iMightBeStuck = true;
    }
    else if(brightestFace == c->returnReverseFace())
    {
      if(!c->roll(-1, buf, 11000))
        iMightBeStuck = true;
    }
    else if(nextBrightestFace == c->returnForwardFace())
    {
      if(!c->roll(1, buf, 6500))
        iMightBeStuck = true;
    }
    else if(nextBrightestFace == c->returnReverseFace())
    {
      if(!c->roll(-1, buf, 6500))
        iMightBeStuck = true;
    }
    else if(thirdBrightestFace == c->returnReverseFace())
    {
      if(!c->roll(-1, buf, 6500))
        iMightBeStuck = true;
    }
    else if(thirdBrightestFace == c->returnForwardFace())
    {
     if(!c->roll(1, buf, 6500))
        iMightBeStuck = true;
    }
    else
      if(!c->roll(1, buf, 7000)) 
        iMightBeStuck = true;
//************** End if else if chain **************************
    loopCounter++;
    delay(1000);
    nextBehavior = basicUpkeep(c, nextBehavior, buf);  // check for neighbors, etc...
  }
  c->blockingBlink(&white,6);
  return(nextBehavior);
}

//================================================================
//==========================CLIMB=================================
//================================================================
Behavior climb(Cube* c, SerialDecoderBuffer* buf)
{
  if(MAGIC_DEBUG) {Serial.println("***CLIMB***");}
  int connectedFace = -1;
  long loopCounter = 0;
  Behavior nextBehavior = CLIMB;
  nextBehavior = basicUpkeep(c, nextBehavior, buf);
  while(nextBehavior == CLIMB) // loop until something changes the next behavior
  {
    nextBehavior = basicUpkeep(c, nextBehavior, buf);
    if(c->numberOfNeighbors(0,0) == 1)
    {
      if(MAGIC_DEBUG) {Serial.println("We Have one neighbor, starting to see how to MOVE!");}
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
      delay(300);
      c->lightFace(c->returnTopFace(),&red);
      delay(300);
      if((connectedFace >= 0) && (connectedFace != c->returnTopFace()) &&
         (connectedFace != c->returnBottomFace()) )
      {
        if(MAGIC_DEBUG) {Serial.println("ABOUT TO TRY TO CHANGE PLANES... WOOO!");}
        if(c->goToPlaneIncludingFaces(connectedFace, c->returnTopFace(), buf))
        {
          c->blockingBlink(&purple,7);
          c->clearRGB();
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
    if(loopCounter%4)
      c->lightCube(&yellow);
    else if((loopCounter-1)%4)
      c->lightCube(&off);
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
//  c->blockingBlink(&red);
//  c->blockingBlink(&white);

//  c->blockingBlink(&blue);
  while(nextBehavior == ATTRACTIVE)
  {
    nextBehavior = basicUpkeep(c, nextBehavior, buf);
    delay(100);
    c->lightCube(&off);
    for(int i = 0; i < 6; i++)
      {
        if((i == c->returnTopFace()) || (i == c->returnBottomFace())) // This ensures we only 
          delay(1);                                              // turn on 4 faces in horizontal plane
          //break;
        else if(c->faces[i].returnNeighborPresence(0) == true) // if we have a neighbor on that face....
          delay(1);
        else
          {
            c->faces[i].turnOnFaceLEDs(1,1,1,1); // turns on LEDs on any exposed face that isn't top/bottom/connected
            delay(300);
          }
      }
    wifiDelay(6000); // delay 10 seconds...
    for(int i = 0; i < 6; i++)
    {
      c->faces[i].turnOffFaceLEDs(); // turn off face LEDs
    }
  }
  return(nextBehavior);
}

Behavior duoSeekLight()
{

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


Behavior basicUpkeep(Cube* c, Behavior currentBehaviorNew, SerialDecoderBuffer* buf)
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
  Behavior behaviorToReturnMAGNETIC = currentBehaviorNew;
  c->updateSensors();
  behaviorToReturnWIFI = checkForBasicWifiCommands(c, currentBehaviorNew, buf);
  delay(100);
  behaviorToReturnMAGNETIC = checkForMagneticTagsStandard(c, behaviorToReturnWIFI, buf);

  if(MAGIC_DEBUG) 
  { 
    Serial.println("-------------------------------------------");
    Serial.println("Ending Basic Upkeep, here is what we found:");
    Serial.print("Top face: ");           Serial.println(c->returnTopFace());
    Serial.print("Current Plane: ");      Serial.println(c->returnCurrentPlane_STRING());
    Serial.print("forward Face ");        Serial.println(c->returnForwardFace());
    Serial.print("# of Neighbors: ");     Serial.println(c->numberOfNeighbors(0,0));
    Serial.print("Resultalt Behavior: "); Serial.println(behaviorsToCmd(behaviorToReturnMAGNETIC));
    Serial.println("-------------------------------------------");
  }
  
  return(behaviorToReturnMAGNETIC);
}

//=============================================================================================
//=============================WIFI Checking  CHECKING=========================================
//=============================================================================================
Behavior checkForBasicWifiCommands(Cube* c, Behavior currentBehavior, SerialDecoderBuffer* buf)
{
  if(MAGIC_DEBUG) {Serial.println("Checking for basic WIFI Commands...");}
  int attempts = 5;
  Behavior resultBehavior = currentBehavior;
  while(!jsonCircularBuffer.empty() && attempts > 0) // while there are still messages, and we haven't tried 5 times...
  {
    StaticJsonBuffer<1000> jb;
    JsonObject& root = jb.parseObject(jsonCircularBuffer.pop());
    if((root["cubeID"] == getCubeIDFromEsp(ESP.getChipId())) || // If message matches your ID
       (root["cubeID"] == -1))                                  // or if message is brodcast
    {
      resultBehavior = cmdToBehaviors(root["cmd"], currentBehavior); // checks to see if we recieved message to switch behaviors
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
  for(int i =0; i < 6; i++)
    { 
      //if(MAGIC_DEBUG) {Serial.print("checking Face: "); Serial.println(i);}
      /*
       * If we are attached to an "arrow" 
       * we breifly blink the direction... no state change
       */
      if(c->faces[i].returnNeighborAngle(0) != -1) // This means we are seeing some "arrow"
      {
        c->lightFace(faceArrowPointsTo(i, c->faces[i].returnNeighborAngle(0)),&white);
      }

      /* This gets activated if we are attached to an actual cube or passive cube
       * for at least two time steps...
       */
      if(((c->faces[i].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE) &&
          (c->faces[i].returnNeighborType(1) == TAGTYPE_PASSIVE_CUBE)))  
      {
        if(i == c->returnBottomFace())
        {
          c->blinkSpecial();
          resultBehavior = FOLLOW_ARROWS;
        }
        else if((c->returnForwardFace() == i) ||
                (c->returnReverseFace() == i)) 
          resultBehavior = CLIMB;
        else
          resultBehavior = ATTRACTIVE;
      }

      if(((c->faces[i].returnNeighborType(0) == TAGTYPE_REGULAR_CUBE) &&
          (c->faces[i].returnNeighborType(1) == TAGTYPE_REGULAR_CUBE)))  
      {
        if(c->returnBottomFace() == i)
          resultBehavior = FOLLOW_ARROWS;
        else if(c->isFaceNeitherTopNorBottom(i))
          resultBehavior = CLIMB;
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
        c->blinkAmerica();
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
        Serial.println("ia f 5000 4000 12");
        delay(5000);
        //c->roll(-1, buf, 8000);
       // delay(2000);
        //c->goToPlaneParallel(i);
        resultBehavior = relayBehavior(c, PURPLE);
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
  if(c->numberOfNeighbors(0,0) >= 2)
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
//        c->blockingBlink(&red);
//        c->blockingBlink(&white);
//        c->blockingBlink(&blue);
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
  else if(cmd == "duo_light_tracj")     {behaviorToReturn = DUO_LIGHT_TRACK;}
  else if(cmd == "follow_arrows")       {behaviorToReturn = FOLLOW_ARROWS;}
  else if(cmd == "chilling")            {behaviorToReturn = CHILLING;}
  else if(cmd == "climb")               {behaviorToReturn = CLIMB;}
  else if(cmd == "attractive")          {behaviorToReturn = ATTRACTIVE;}
  else if(cmd == "shut_down")           {behaviorToReturn = SHUT_DOWN;}
  else if(cmd == "sleep")               {behaviorToReturn = SLEEP;}
  else if(cmd == "yellow")              {behaviorToReturn = YELLOW;}
  else if(cmd == "purple")              {behaviorToReturn = PURPLE;}
  else if(cmd == "teal")                {behaviorToReturn = TEAL;}
  else if(cmd == "red")                 {behaviorToReturn = RED;}
  else if(cmd == "blue")                {behaviorToReturn = BLUE;}
  else if(cmd == "green")               {behaviorToReturn = GREEN;}
  else if(cmd == "white")               {behaviorToReturn = WHITE;}
  else if(cmd == "lightsoff")           {behaviorToReturn = LIGHTSOFF;}
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
  else if(inputBehavior == YELLOW)                {stringToReturn = "yellow";}
  else if(inputBehavior == PURPLE)                {stringToReturn = "purple";}
  else if(inputBehavior == TEAL)                  {stringToReturn = "teal";}
  else if(inputBehavior == RED)                   {stringToReturn = "red";}
  else if(inputBehavior == BLUE)                  {stringToReturn = "blue";}
  else if(inputBehavior == GREEN)                 {stringToReturn = "green";}
  else if(inputBehavior == WHITE)                 {stringToReturn = "white";}
  else if(inputBehavior == LIGHTSOFF)             {stringToReturn = "lightsoff";}
return(stringToReturn);
}

Behavior checkForBehaviors(Cube* c, SerialDecoderBuffer* buf, Behavior behavior)
{
  c->behaviorBuffer.push(behavior); // adds the switch into a buffer
  if (behavior == SOLO_LIGHT_TRACK)
    behavior = soloSeekLight(c, buf);
  else if (behavior == DUO_LIGHT_TRACK)
    behavior = duoSeekLight();
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
  else if (behavior == YELLOW)
    behavior = Yellow(c, buf);
  else if (behavior == PURPLE)
    behavior = Purple(c, buf);
  else if (behavior == TEAL)
    behavior = Teal(c, buf);
  else
  {
    //Serial.println("ERROR: unknown behavior.  Reverting to \"CHILLING\"");
    behavior = CHILLING;
  }
  return(behavior);
}
