#include "Behavior.h"
#include "Initialization.h"     // Includes .h files for each of the "tabs" in arduino
#include "Cube.h"               // Includes .h files for each of the "tabs" in arduino
#include "Face.h"               // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"              // Includes .h files for each of the "tabs" in arduino
#include "Communication.h"      // Includes wifi
#include "Motion.h"
#include "Defines.h"
#include "SerialDecoder.h"
#include "MagTag.h"

Behavior followArrows()
{
// Set rmp(6000);
// wait  (4000ms);
// ebrake //////
}

Behavior soloSeekLight(Cube* c, SerialDecoderBuffer* buf)
{
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
          //{Serial.println("bldcaccel f 5000 1000"); delay(2000); Serial.println("bldcstop b");delay(5000);}
    }
  Serial.println("Changing Loops");
  return(nextBehavior);
}

Behavior testTestingThangs(Cube* c, SerialDecoderBuffer* buf)
{
   Serial.println("sma retract 6000");
   while(1)
  {
    switch(c->findLikelyPlane())
    {
      case plane0123:
      {
        c->blockingBlink(0,1,0,1,50);
        break;
      }
      case plane0425:
      {
        c->blockingBlink(1, 0, 0, 1, 50);
        break;
      }
      case plane1453:
      {
        c->blockingBlink(0, 0, 1, 1, 50);
        break;
      }
      default:
      {
        break;
      }
    }
  }
   return(CHILLING);
}

/**
 * Checks for WiFi commands and magnetic tags.
 */
Behavior chilling(Cube* c)
{
  Behavior nextBehavior = CHILLING;
  long millisAccum = 0;
  long millisPrev = millis();
  
  while(nextBehavior == CHILLING)
  {
    c->updateSensors();
    //======= Check for magnetic tags =======
    for(int i = 0; i < 6; i++)
    {
      Tag t;
      analyzeTag(c->faces[i].returnMagnetAngle_A(0), 
                 c->faces[i].returnMagnetStrength_A(0),
                 c->faces[i].returnMagnetAngle_B(0), 
                 c->faces[i].returnMagnetStrength_B(0), &t);
      if(t.command == TAGCOMMAND_SLEEP)
      {
        nextBehavior = RELAY_SLEEP;
      }
      if(t.command == TAGCOMMAND_PURPLE)
      {
        c->blockingBlink(1,0,1);
      }
    }
    //=======end Check for Magnetic Tags=====
    wifiDelay(200);
    
    //======= check for wifi commands =======
    if(!jsonCircularBuffer.empty())
    {
      StaticJsonBuffer<256> jb;
      JsonObject& root = jb.parseObject(jsonCircularBuffer.pop());

      if((root["cubeID"] == getCubeIDFromEsp(ESP.getChipId())) || // If message matches your ID
         (root["cubeID"] == -1))                                  // or if message is brodcast
      {
        if(root["cmd"] == "sleep")
        {
          nextBehavior = SHUT_DOWN;
          mesh.update();
        }
      }
    }
    
    //======= blink =======
    long m = millis();
    millisAccum += (m - millisPrev);
    millisPrev = m;
    if((millisAccum >= 0) && (millisAccum < 500))
      c->lightCube(false, false, true);
    else if(millisAccum < 1500)
      c->lightCube(false, false, false);
    else
      millisAccum -= 1500;
    //===== end blink =====
    
    delay(10);
  }
  
  return nextBehavior;
}

Behavior attractive(Cube* c)
{
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
      c->lightCube(0, 0, 0.5);

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

Behavior relaySleepMessage(Cube* c)
{
  if(DEBUG1) Serial.println("telling others to go to sleep");

  //============= Generates a Broadcast message ==========
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["type"] = "cmd";
  root["cubeID"] = -1;
  root["cmd"] = "sleep";
  String str;
  root.printTo(str);
  mesh.sendBroadcast(str);
  //======== End Generating of Broadcast message ==========
  wifiDelay(2000);
  return SHUT_DOWN;
}

//================ Utilities ==============================

void wifiDelay(int delayTime)
{
  long millisNow = millis();
  while((millis() - millisNow) < delayTime)
  {
    mesh.update();
  }
}
////////// How Long did this take?
////long begin_time = millis();
////Serial.print("Function took: ");Serial.println(millis()-begin_time);
//////////
