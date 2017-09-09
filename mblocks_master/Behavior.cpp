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
   //if(
   //if
   //Move(&traverse, "f", 4, buf);
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
    mesh.update();

    //======= check for wifi commands =======
    if(!jsonCircularBuffer.empty())
    {
      StaticJsonBuffer<256> jb;
      JsonObject& root = jb.parseObject(jsonCircularBuffer.pop());

      if((root["cubeID"] == getCubeIDFromEsp(ESP.getChipId())) ||
         (root["cubeID"] == -1))
      {
        if(root["cmd"] == "sleep")
        {
          nextBehavior = SHUT_DOWN;
          mesh.update();
        }
      }
    }
    
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

    delay(10);
  }
  return nextBehavior;
}

Behavior attractive(Cube* c)
{
  c->updateSensors();
  for(int i = 0; i < 6; i++)
    {
      if(i == c->returnTopFace() || i == c->returnBottomFace()) // This ensures we only turn on 4 faces in horizontal plane
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
  Serial.println("telling others to go to sleep");
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["type"] = "cmd";
  root["cubeID"] = -1;
  root["cmd"] = "sleep";
  
  String str;
  root.printTo(str);
  mesh.sendBroadcast(str);

  long millisNow = millis();
  while((millis() - millisNow) < 1000)
  {
    mesh.update();
  }
  
  return SHUT_DOWN;
}

//int display_brightest_face()
//{
//  for(int x = 1; x < 7; x++){fblight_list[x] = read_ambient(x);} // Populates temp. array of light values
//
//  if(up_face > 0 && up_face <7)    {fblight_list[up_face] = -1;} // Zeros out the exclude face
//
//  else                             {for(int x = 1; x < 7; x++){fblight_list[x] = 0;}} // this means cube isn't stable, so we zero everything
//
//  for(int x = 1; x < 7; x++){sum_of_horizontal+=fblight_list[x];}
//
//  int brightest[2] = {which_face_is_brightest(fblight_list),which_face_is_brightest(fblight_list)};
//
//  Serial.print("Brightest face is: ");Serial.println(brightest[0]);
//
//  //Serial.print("
//
//  face_rgb(brightest[0],0,1,1,1);
//}

////////// How Long did this take?
////long begin_time = millis();
////Serial.print("Function took: ");Serial.println(millis()-begin_time);
//////////
//

//void light_track_update_part2()
//{
//
//  bool result = false;
//  if   (change_plane_counter < 3){change_plane_counter++;change_plane_to_parallel(which_face_is_up(12000));}
//  else  // now we do
//    {
//      if(which_plane_fast() > 1000){move_randomly_in_horizontal_plane_three_times();}
//    }
//
//  // Step 1: Look up up face...
//  // Step 2: Figure out what plane corresponds to horizontal...
//  // if(current_plane != horizontal plane)
////  {
////    change plane...
////  }
//     // else if(not 100% connected){Serial.println("ia f/r 5000 4000 10");
//     // else if(100% connected){flash_green(5);cmd = "g"}
//     // else {  }// we don't know what is going on here...
//}
//
//
//int wiggle_test(String for_rev)
////
////read_accel() read_gyro();
////AcX,AcY,AcZ,GyX,GyY,GyZ,
//{
//  if(for_rev == "f")        {}
//  else if(for_rev == "r")   {}
//  else                      {for_rev = "f";}
//  Serial.println("stillalive");
//  if(DEBUG){Serial.println("BEGINNING WIGGLE_TEST");}
//  int movement = 0;
//  delay(50);
//  String command = "bldcaccel "+ for_rev + " " + String(2800 + 3*plane_change_offset) + " " + String(700 + plane_change_offset);
//  Serial.println(command);mesh.sendBroadcast
//  ////////////////////////////// Reads the gyro for 12*65 ms
//  for(int i = 0; i < 12; i++)
//  {
//   movement += read_gyro(MPU_parasite)/100;
//   delay(65);
//  }
//  Serial.print("stillalive");
//  delay(40);
//  delay(plane_change_offset);
//  //////////////////////////////
//  Serial.println("bldcstop b");
//  delay(40);
//  Serial.println("bldcstop b");
//  delay(40);
//  Serial.print("stillalive");
//  ////////////////////////////// Reads the gyro for 12*65 ms
//  for(int i = 0; i < 8; i++)
//  {
//   movement += read_gyro(MPU_parasite)/100;
//   delay(30);
//  }
//  //////////////////////////////
//  Serial.println("stillalive");
//  return(movement);
//}
//
//int which_face_is_brightest(int fblight_temp[])
//// Sorts input list (fblight_temp). Running it a second time, returns 2nd brightest...
//// running a 3rd time returns 3rd brightest...
//{
//  int brightest_list[6] = {0,0,0,0,0,0};
//  for(int i = 1; i < 7 ;i++){if(fblight_temp[brightest_list[0]] < fblight_temp[i]){brightest_list[0] = (i);}}
//  fblight_temp[brightest_list[0]] = 0;
//  return(brightest_list[0]);
//}
//
//
//  void light_track_part_2()
//  {
//    light_track_update_part2();
//  }
//
//  void light_track_update_part_2() {light_track_update_part2();}
//
//  void roll_to_plane()
//  {
//    if(ambient_values[face_that_is_up][0] > 0)
//    {
//           if(most_recent_traverse_result == 180 )   {cmd = "g"; demo = "nothing";}
//      else if(attempts_traverse > 5)                 {cmd = "r"; demo = "nothing";}
//      else if(most_recent_traverse_result == 90)     {Serial.println("stillalive"); most_recent_traverse_result = move_normal("f","90 deg",6000, 24, 12,"e 10", 4000);attempts_traverse++;}
//      else if(most_recent_traverse_result == 0)      {Serial.println("stillalive"); most_recent_traverse_result = move_normal("f","90 deg",6000, 24, 12,"e 10", 4000);attempts_traverse++;}
//    }
//    else if(ambient_values[face_that_is_up][0])   {y_counter = 8;}
