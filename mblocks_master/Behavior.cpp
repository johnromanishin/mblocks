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


Behavior Yellow(Cube* c, SerialDecoderBuffer* buf)
{
  if(DEBUG_BEHAVIOR) {Serial.println("Yellow");}
  
  Behavior nextBehavior = YELLOW;
  c->updateSensors();
  c->lightCube(&yellow);
  nextBehavior = checkForMagneticTagsStandard(c, nextBehavior, buf);
  wifiDelay(100);
  nextBehavior = checkForBasicWifiCommands(c, nextBehavior, buf);
}
Behavior Purple(Cube* c, SerialDecoderBuffer* buf)
{
  if(DEBUG_BEHAVIOR) {Serial.println("Purple");}
  c->lightCube(&purple);
  Behavior nextBehavior = PURPLE;
  c->updateSensors();
  nextBehavior = checkForMagneticTagsStandard(c, nextBehavior, buf);
  wifiDelay(100);
  nextBehavior = checkForBasicWifiCommands(c, nextBehavior, buf);
}

Behavior Teal(Cube* c, SerialDecoderBuffer* buf)
{
  if(DEBUG_BEHAVIOR) {Serial.println("Teal");}
  c->lightCube(&teal);
  Behavior nextBehavior = TEAL;
  c->updateSensors();
  nextBehavior = checkForMagneticTagsStandard(c, nextBehavior, buf);
  wifiDelay(100);
  nextBehavior = checkForBasicWifiCommands(c, nextBehavior, buf);
}

Behavior sleep(Cube* c)
{
  c->blockingBlink(&red);
  for(int i = 0; i < 10; i++)
  {
    Serial.println("sleep");
    delay(300);
  }
}

Behavior followArrows(Cube* c, SerialDecoderBuffer* buf)
{
  Behavior nextBehavior = FOLLOW_ARROWS; // default is to keep doing what we are doing.
  if(DEBUG_BEHAVIOR)
    Serial.println("followArrows");
  c->updateSensors();
  nextBehavior = checkForMagneticTagsStandard(c, nextBehavior, buf);
  wifiDelay(250);
  nextBehavior = checkForBasicWifiCommands(c, nextBehavior, buf);   
  for(int i = 0; i < 6; i ++)
  {
    if((c->faces[i].returnNeighborAngle(0) > 0) && // If the same face shows valid angles
       (c->faces[i].returnNeighborAngle(2) > 0))
    {
      //c->blockingBlink(&red,4);  // blink
      int otherFace = faceArrowPointsTo(i, c->faces[i].returnNeighborAngle(0));
      if(c->goToPlaneIncludingFaces(i, otherFace, buf)) // then go to plane parallel
      {
        int CW_or_CCW = faceClockiness(otherFace, i);
        if(CW_or_CCW == 1)
        {
          delay(2000);
          c->moveIASimple(&traverse_F);
          delay(2000);
        }
        else if(CW_or_CCW == -1)
        {
          delay(2000);
          c->moveIASimple(&traverse_R);
          delay(2000);
        }
      }
    }
  }
  return(nextBehavior);
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
      c->lightFace(brightestFace,&teal);
      delay(500);
      if(brightestFace == c->returnForwardFace())
          
          {Serial.println("bldcspeed f 6000");c->blockingBlink(&green);delay(3000);Serial.println("bldcstop b");}
          
      else if(brightestFace == c->returnReverseFace())
      
          {Serial.println("bldcspeed r 6000");c->blockingBlink(&red);delay(3000);Serial.println("bldcstop b");}
      else if(c->returnForwardFace() == c->returnXthBrightestFace(2))
          {Serial.println("bldcspeed f 6000");c->blockingBlink(&green);delay(3000);Serial.println("bldcstop b");}
      else if(c->returnReverseFace() == c->returnXthBrightestFace(2))
          {Serial.println("bldcspeed r 6000");c->blockingBlink(&red);delay(3000);Serial.println("bldcstop b");}
      else
          delay(100);
      delay(3000);
    }
  Serial.println("Changing Loops");
  return(nextBehavior);
}

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
    nextBehavior = checkForBasicWifiCommands(c, nextBehavior, buf);
    
    if(c->numberOfNeighbors(0,0) == 1)
    {
      Tag t;
      for(int i = 0; i < 6; i++)
      {     
        if(c->faces[i].returnNeighborType(0) ==  TAGTYPE_REGULAR_CUBE ||
           c->faces[i].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE)//&& // If a valid tag exists...
        {
          connectedFace = i;
        }
      }
      c->lightFace(connectedFace,&yellow);
      delay(300);
      c->lightFace(c->returnTopFace(),&red);
      delay(300);
      if((connectedFace > 0) && (connectedFace != c->returnTopFace()) &&
         (connectedFace != c->returnBottomFace()) )
      {
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
    if(loopCounter%4)
      c->lightCube(&yellow);
    else if((loopCounter+1)%4)
      c->lightCube(&off);
    //c->lightCube(!(loopCounter%4), !(loopCounter%4), false); // blinks yellow every 4 times...
    delay(300);
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
    //c->lightCube(false, false, !(loopCounter%4));
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
        c->lightFace(faceArrowPointsTo(i, c->faces[i].returnNeighborAngle(0)),&green);
      }
      
      if((c->faces[i].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE) &&
         (c->faces[i].returnNeighborType(1) == TAGTYPE_PASSIVE_CUBE))
      {
        if(i == c->returnBottomFace())
          resultBehavior = FOLLOW_ARROWS;
        else
          resultBehavior = TEST_TESTING_THANGS;
//        if((c->faces[i].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE) &&
//           (c->faces[i].returnNeighborType(1) == TAGTYPE_PASSIVE_CUBE) &&
//           (c->faces[i].returnNeighborType(2) == TAGTYPE_PASSIVE_CUBE) &&
//           (c->faces[i].returnNeighborType(3) == TAGTYPE_PASSIVE_CUBE) &&
//           (c->faces[i].returnNeighborType(4) == TAGTYPE_PASSIVE_CUBE))
//           {
//              c->blockingBlink(1,0,0,3);
//              c->blockingBlink(0,1,0,3);
//              c->blockingBlink(0,0,1,3);
//              c->blockingBlink(1,1,0,3);
//              c->blockingBlink(0,1,1,3);
//              c->blockingBlink(1,0,1,3);
//              c->blockingBlink(1,1,1,3);
//           }
      }
      
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_SLEEP)
      {
        resultBehavior = relayBehavior(c, SLEEP);
      }
      if(c->faces[i].returnNeighborCommand(0) == TAGCOMMAND_PURPLE)
      {
        c->lightCube(&purple);
        //====================SEND DEBUG =====================      
        c->goToPlaneParallel(i, buf);
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
    behavior = followArrows(c, buf);
  else if (behavior == TEST_TESTING_THANGS)
    behavior = testTestingThangs(c, buf);
  else if (behavior == CHILLING)
    behavior = chilling(c, buf);
  else if (behavior == ATTRACTIVE)
    behavior = attractive(c);
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
