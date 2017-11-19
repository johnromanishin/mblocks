#include "Behavior.h"
#include "Initialization.h"     // Includes .h files for each of the "tabs" in arduino
#include "Cube.h"               // Includes .h files for each of the "tabs" in arduino
#include "Face.h"               // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"              // Includes .h files for each of the "tabs" in arduino
#include "Communication.h"      // Includes wifi
#include "Defines.h"
#include "SerialDecoder.h"

Behavior Yellow(Cube* c, SerialDecoderBuffer* buf)
{
  if(DEBUG_BEHAVIOR) {Serial.println("Yellow");}
  
  Behavior nextBehavior = YELLOW;
  c->updateSensors();
  c->lightCube(1,1,0);
  nextBehavior = checkForMagneticTagsStandard(c, nextBehavior, buf);
  wifiDelay(100);
  nextBehavior = checkForBasicWifiCommands(c, nextBehavior, buf);
}
Behavior Purple(Cube* c, SerialDecoderBuffer* buf)
{
  if(DEBUG_BEHAVIOR) {Serial.println("Purple");}
  c->lightCube(1,0,1);
  Behavior nextBehavior = PURPLE;
  c->updateSensors();
  nextBehavior = checkForMagneticTagsStandard(c, nextBehavior, buf);
  wifiDelay(100);
  nextBehavior = checkForBasicWifiCommands(c, nextBehavior, buf);
}

Behavior Teal(Cube* c, SerialDecoderBuffer* buf)
{
  if(DEBUG_BEHAVIOR) {Serial.println("Teal");}
  c->lightCube(0,1,1);
  Behavior nextBehavior = TEAL;
  c->updateSensors();
  nextBehavior = checkForMagneticTagsStandard(c, nextBehavior, buf);
  wifiDelay(100);
  nextBehavior = checkForBasicWifiCommands(c, nextBehavior, buf);
}

Behavior sleep()
{
  for(int i = 0; i < 10; i++)
  {
    Serial.println("sleep");
    delay(300);
  }
}

Behavior followArrows()
{
  if(DEBUG_BEHAVIOR) {Serial.println("followArrows");}
// Set rmp(6000);
// wait  (4000ms);
// ebrake //////
}

Behavior soloSeekLight(Cube* c, SerialDecoderBuffer* buf)
{
  if(DEBUG_BEHAVIOR) {Serial.println("soloSeekLight");}
  Behavior nextBehavior = SOLO_LIGHT_TRACK;

  while(millis() < c->shutDownTime)
  {
    c->updateSensors();
    if(c->numberOfNeighbors(0,0))
      {
          nextBehavior = TEST_TESTING_THANGS;
          break;
      }
    int brightestFace = c->returnXthBrightestFace(0);
    if(c->returnXthBrightestFace(0) == c->returnTopFace()) // now brightest Face now excludes the top face
      {
        brightestFace = c->returnXthBrightestFace(1);
      }
      c->lightFace(brightestFace,0,1,1);
      delay(500);
      if(brightestFace == c->returnForwardFace())
          {Serial.println("bldcspeed f 6000");c->blockingBlink(0,1,0);delay(3000);Serial.println("bldcstop b");}
      else if(brightestFace == c->returnReverseFace())
          {Serial.println("bldcspeed r 6000");c->blockingBlink(1,0,0);delay(3000);Serial.println("bldcstop b");}
      else if(c->returnForwardFace() == c->returnXthBrightestFace(2))
          {Serial.println("bldcspeed f 6000");c->blockingBlink(0,1,0);delay(3000);Serial.println("bldcstop b");}
      else if(c->returnReverseFace() == c->returnXthBrightestFace(2))
          {Serial.println("bldcspeed r 6000");c->blockingBlink(1,0,0);delay(3000);Serial.println("bldcstop b");}
      else
          delay(100);
    }
  Serial.println("Changing Loops");
  return(nextBehavior);
}
//c->goToPlaneParallel(i, buf);
//================================================================
//==========================TESTING THANGS========================
//================================================================
Behavior testTestingThangs(Cube* c, SerialDecoderBuffer* buf)
{
  if(DEBUG_BEHAVIOR) {Serial.println("testTestingThangs");}

  int connectedFace = -1;
  long loopCounter = 0;
  Behavior nextBehavior = TEST_TESTING_THANGS;
  while(nextBehavior == TEST_TESTING_THANGS) // loop until something changes the next behavior
  {
    c->updateSensors();
    nextBehavior = checkForMagneticTagsStandard(c, nextBehavior, buf);
    if(nextBehavior != TEST_TESTING_THANGS) {Serial.println("WHAT THE FUCKING FUCK");}
    wifiDelay(100);
    nextBehavior = checkForBasicWifiCommands(c, nextBehavior, buf);
    
    if(c->numberOfNeighbors(0,0) == 1)
    {
      Tag t;
      for(int i = 0; i < 6; i++)
      {      
        analyzeTag(c->faces[i].returnMagnetAngle_A(0),          // This calls fubnction which reads tags, populate data fields
                   c->faces[i].returnMagnetStrength_A(0),       //
                   c->faces[i].returnMagnetAngle_B(0),          //
                   c->faces[i].returnMagnetStrength_B(0), &t);  //  
        if(t.type ==  TAGTYPE_REGULAR_CUBE || t.type == TAGTYPE_PASSIVE_CUBE)//&& // If a valid tag exists...
        {
          connectedFace = i;
        }
      }
      c->lightFace(connectedFace,0,1,1);
      delay(300);
      c->lightFace(c->returnTopFace(),1,1,1);
      delay(300);
      if((connectedFace > 0) && (connectedFace != c->returnTopFace()) &&
          connectedFace != c->returnBottomFace())
      {
        if(c->goToPlaneIncludingFaces(connectedFace, c->returnTopFace(), buf))
        {
          c->blockingBlink(1,0,1,3);
          c->clearRGB();
          delay(50);
          for(int i = 0; i < 6; i++)
          {
            c->faces[c->returnTopFace()].blinkOutMessage(i);
            delay(100);
          }
          //Serial.println("ia f 15500 3800 8 e 10");
          delay(8000);
        }
      }
    }
    c->lightCube(!(loopCounter%4), !(loopCounter%4), false); // blinks yellow every 4 times...
    delay(10);
    loopCounter++;
  }
  return nextBehavior;
}

//================================================================
//==========================CHILLING==============================
//================================================================
Behavior chilling(Cube* c, SerialDecoderBuffer* buf)
{
  if(DEBUG_BEHAVIOR) {Serial.println("chilling");}  
  long loopCounter = 0;
  Behavior nextBehavior = CHILLING;
  while(nextBehavior == CHILLING) // loop until something changes the next behavior
  {
    c->updateSensors();
    nextBehavior = checkForMagneticTagsStandard(c, nextBehavior, buf);
    wifiDelay(400);
    nextBehavior = checkForBasicWifiCommands(c, nextBehavior, buf);
    c->lightCube(false, false, !(loopCounter%4));
    delay(10);
    loopCounter++;
  }
  return nextBehavior;
}

//================================================================
//==========================ACTRACTIVE==============================
//================================================================
Behavior attractive(Cube* c)
{
  if(DEBUG_BEHAVIOR) {Serial.println("attractive");}  
  c->updateSensors();
  for(int i = 0; i < 6; i++)
    {
      if(i == c->returnTopFace() || i == c->returnBottomFace()) // This ensures we only 
                                                                // turn on 4 faces in horizontal plane
        break;
      else
        c->faces[i].turnOnFaceLEDs(1,1,1,1); // turns on LEDs
    }
  mesh.update();
  delay(100);
  return(CHILLING);
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
  if(DEBUG_BEHAVIOR) {Serial.println("crystalize");}
  bool amICrystalized = false;
  int ledCounter = 0;

  while(true)
  {
    //update the sensors
    c->updateSensors();

    // ======= service commands that come from the json buffer =======
    if(!jsonCircularBuffer.empty())
    {
      StaticJsonBuffer<256> jb;
      JsonObject& root = jb.parseObject(jsonCircularBuffer.pop());

      if((root["cubeID"] == getCubeIDFromEsp(ESP.getChipId())) ||
         (root["cubeID"] == -1))
      {
        if(root["cmd"] == "blinkOnce")
          c->blockingBlink(1, 1, 1, 1, 100);
        else if(root["cmd"] == "clearArrows")
          clearAllMappings(&c->arrowMap);
        else if(root["cmd"] == "arrowUpdateAllFaces")
        {
          ArrowMapEntry mapEntry;
          mapEntry.ID = root["arrowID"];
          for(int i = 0; i < 6; i++)
          {
            mapEntry.arrows[i] = root["arrowDirections"][i];
          }
          addArrowMapping(&c->arrowMap, &mapEntry);
        }
        else if(root["cmd"] == "arrowUpdateOneFace")
        {
          addArrowMappingOneFace(&c->arrowMap, root["arrowID"], root["faceToUpdate"], root["newValue"]);
        }
        else if(root["cmd"] == "seedCrystal")
        {
          //if the cube is nominated to be the seed crystal, then it points its arrows in a pre-set direction
          //the arrows on the 0, 1, 2, and 3 faces all point towards face 5.  Face 5 points out of the center
          //of the cube and face 4 points into the center of the cube.
          amICrystalized = true;
          ledCounter = 0;
          ArrowMapEntry mapEntry;
          mapEntry.ID = getCubeIDFromEsp(ESP.getChipId());
          mapEntry.arrows[0] = 0; //XXX TODO?
          mapEntry.arrows[1] = 0; //XXX TODO ?
          mapEntry.arrows[2] = 0; //XXX TODO ?
          mapEntry.arrows[3] = 0; //XXX TODO ?
          mapEntry.arrows[4] = 4; //face 4's arrow points into the center of the cube.
          mapEntry.arrows[5] = 5; //face 5's arrow points out of the center of the cube.
          addArrowMapping(&c->arrowMap, &mapEntry);
        }
      }
    }

    // ======= if we are crystalized, then just stupidly blink =======
    if(amICrystalized)
    {
      //turn all of the corner lights blue
      c->lightCube(0, 0, 1);

      //blink faces to point in directions of arrows
      //in the first step of the blink animation, we light faces pointing "into" the center of the cube.
      ledCounter++;
      if((ledCounter >= 0) && (ledCounter < 4))
      {
        for(int i = 0; i < 6; i++)
        {
          if(lookupMapping(&c->arrowMap, getCubeIDFromEsp(ESP.getChipId()), i) == 4)
            c->faces[i].turnOnFaceLEDs(true, true, true, true);
          else
            c->faces[i].turnOnFaceLEDs(false, false, false, false);
        }
      }
      else if((ledCounter >= 4) && (ledCounter < 8))
      {
        for(int i = 0; i < 6; i++)
        {
          int subsequentRot = lookupMapping(&c->arrowMap, getCubeIDFromEsp(ESP.getChipId()), i);
          if((subsequentRot >= 0) && (subsequentRot <= 3))
          {
            subsequentRot += 2;
            if(subsequentRot > 3)
              subsequentRot -= 4;
            int subsequentFace = faceRotations[i][subsequentRot];
            c->setFaceLEDsAtEdge(i, subsequentFace);
          }
          else
            c->faces[i].turnOnFaceLEDs(false, false, false, false);
        }
      }
      else if((ledCounter >= 8) && (ledCounter < 12))
      {
        for(int i = 0; i < 6; i++)
        {
          int subsequentRot = lookupMapping(&c->arrowMap, getCubeIDFromEsp(ESP.getChipId()), i);
          if((subsequentRot >= 0) && (subsequentRot <= 3))
          {
            if(subsequentRot > 3)
              subsequentRot -= 4;
            int subsequentFace = faceRotations[i][subsequentRot];
            c->setFaceLEDsAtEdge(i, subsequentFace);
          }
          else
            c->faces[i].turnOnFaceLEDs(false, false, false, false);
        }
      }
      else if((ledCounter >= 12) && (ledCounter < 16))
      {
        for(int i = 0; i < 6; i++)
        {
          if(lookupMapping(&c->arrowMap, getCubeIDFromEsp(ESP.getChipId()), i) == 5)
            c->faces[i].turnOnFaceLEDs(true, true, true, true);
          else
            c->faces[i].turnOnFaceLEDs(false, false, false, false);
        }
      }
      else if(ledCounter == 16)
        ledCounter = 0;
    }

    // ======= If we haven't crystalized yet,
    else
    {
      //check for neighbors
    }
  }
}

//=============================================================================================
//=============================WIFI Checking  CHECKING=========================================
//=============================================================================================
Behavior checkForBasicWifiCommands(Cube* c, Behavior currentBehavior, SerialDecoderBuffer* buf)
{
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
      if(cubeID > 40) // cubeID's over 40 means it is attached by a cable... not a real cube // so we print
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
  Behavior resultBehavior = currentBehavior;
  for(int i = 0; i < 6; i++)
    {
      if(c->faces[i].returnNeighborAngle(0) != -1) // This means we are seeing some "arrow"
      {
        c->lightFace(faceArrowPointsTo(i, c->faces[i].returnNeighborAngle(0)),0,1,0);
      }
      
      if(c->faces[i].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE)
      {
        resultBehavior = FOLLOW_ARROWS;
        if((c->faces[i].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE) &&
           (c->faces[i].returnNeighborType(1) == TAGTYPE_PASSIVE_CUBE) &&
           (c->faces[i].returnNeighborType(2) == TAGTYPE_PASSIVE_CUBE) &&
           (c->faces[i].returnNeighborType(3) == TAGTYPE_PASSIVE_CUBE) &&
           (c->faces[i].returnNeighborType(4) == TAGTYPE_PASSIVE_CUBE))
           {
              c->blockingBlink(1,0,0,3);
              c->blockingBlink(0,1,0,3);
              c->blockingBlink(0,0,1,3);
              c->blockingBlink(1,1,0,3);
              c->blockingBlink(0,1,1,3);
              c->blockingBlink(1,0,1,3);
              c->blockingBlink(1,1,1,3);
           }
      }
      
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_SLEEP)
      {
        resultBehavior = relayBehavior(c, SLEEP);
      }
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_PURPLE)
      {
        c->lightCube(1,0,1);
        //====================SEND DEBUG =====================      
        c->goToPlaneParallel(i, buf);
        //mesh.sendBroadcast(Str);
        //==================END SEND DEBUG ===================  
      }
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_27)
      {
        resultBehavior = TEAL;
        if((c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_27) &&
           (c->faces[i].returnNeighborCommand(2) == TAGCOMMAND_27) &&
           (c->faces[i].returnNeighborCommand(6) == TAGCOMMAND_27))
 
           {
              resultBehavior = relayBehavior(c, TEAL);
           }
        
      }
      
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_23)
      {
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
  else if(cmd == "test_testing_thangs") {behaviorToReturn = TEST_TESTING_THANGS;}
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
  else if(inputBehavior == TEST_TESTING_THANGS)   {stringToReturn = "test_testing_thangs";}
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
  if (behavior == SOLO_LIGHT_TRACK)
    behavior = soloSeekLight(c, buf);
  else if (behavior == DUO_LIGHT_TRACK)
    behavior = duoSeekLight();
  else if (behavior == FOLLOW_ARROWS)
    behavior = followArrows();
  else if (behavior == TEST_TESTING_THANGS)
    behavior = testTestingThangs(c, buf);
  else if (behavior == CHILLING)
    behavior = chilling(c, buf);
  else if (behavior == ATTRACTIVE)
    behavior = attractive(c);
  else if (behavior == SLEEP)
    behavior = sleep();
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
