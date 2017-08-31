// We wish to define four behaviors, in order of coding priority:
//  1. Step toward a light source, solo
//  2. Step toward a light source, in tandem with an adjacent block
//  3. Step in the direction of an arrow, provided on an adjacent face
//  4. Step in a direction provided via external communication (WiFi)

#include "Initialization.h"       // Includes .h files for each of the "tabs" in arduino
#include "Cube.h"     // Includes .h files for each of the "tabs" in arduino
#include "Face.h"     // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"                // Includes .h files for each of the "tabs" in arduino
#include "Communication.h"        // Includes wifi 
#include "Behavior.h"
#include "Cube.h"
#include "EspToCubeMapping.h"

void followArrows()
{
// Set rmp(6000);
// wait  (4000ms);
// ebrake //////
}

void soloSeekLight(Cube* c)
{
  while(millis() < c->shutDownTime)
  {
    c->updateSensors();
    if(c->numberOfNeighbors(0,0))
      {
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
          {Serial.println("bldcaccel f 5000 1000"); delay(2000); Serial.println("bldcstop b");delay(5000);}
    }
  c->blockingBlink(0,0,1,30,200);
  c->shutDown(); 
}

void duoSeekLight()
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
void crystalize(Cube* c, painlessMesh* m)
{
  bool amICrystalized = false;
  int ledCounter = 0;
  
  while(true)
  {
    //update the sensors
    c->updateSensors();

    // ======= service commands that come from the json buffer =======
    if(!jsonBuffer.empty())
    {
      StaticJsonBuffer<256> jb;
      JsonObject& root = jb.parseObject(jsonBuffer.pop());

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
            c->turnOnFaceLEDs(false, false, false, false);
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
            c->turnOnFaceLEDs(false, false, false, false);
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

//
//int light_track_update()
//{
////Step 1: Update Sensor Values //
//if(DEBUG){Serial.println("Running light_track_update");}
//if(light_tracking_delay_counter < 1) // THIS means that we are done with the previous iteration of this, and we now recheck the values and plan our action
//  {
//  rgboff(); //if(DEBUG){Serial.println("Beginning light_track_update");}
//  light_tracking_internal_state = "nothing";
//  int threshold = 3;
//  int for_face = which_face_is_forward(which_plane_fast());
//  int rev_face = opposite_face(for_face);
//  int up_face = which_face_is_up(12000);
//  int value_of_top_face = read_ambient(up_face);
//  int fblight_list[7] = {0,0,0,0,0,0,0};
//  int sum_of_horizontal = 0;
//  for(int x = 1; x < 7; x++){fblight_list[x] = read_ambient(x);} // Populates temp. array of light values
//  if(up_face > 0 && up_face <7)    {fblight_list[up_face] = -1;} // Zeros out the exclude face
//  else                             {for(int x = 1; x < 7; x++){fblight_list[x] = 0;}} // this means cube isn't stable, so we zero everything
//  for(int x = 1; x < 7; x++){sum_of_horizontal+=fblight_list[x];}
//  int brightest[2] = {which_face_is_brightest(fblight_list),which_face_is_brightest(fblight_list)};
//  for(int j = 10; j > 0; j--){previous_up_face[j] = previous_up_face[j-1];} previous_up_face[0] = up_face;
//
////Step 2: Choose Action
//     // Forward face has brightest light: Action move forward
//     if(brightest[0] == for_face && sum_of_horizontal > threshold) // 
//        {
//          
//          light_tracking_internal_state = "f";
//          face_rgb(for_face,0,0,1,1);
//          delay(10);
//          Serial.println("bldcspeed f 5700");
//          light_tracking_delay_counter = 95;
//        }
//     // Reverse face has brightest light: Action move forward
//     else if(brightest[0] == rev_face && sum_of_horizontal > threshold)
//          {
//          light_tracking_internal_state = "r";
//          face_rgb(rev_face,1,0,1,1);
//          delay(10);
//          Serial.println("bldcspeed r 5700");
//          light_tracking_delay_counter = 95;
//          }
//     // Cube flywheel is coplaner with ground...: go crazy!!!!!!
//          else if(for_face > 7) // This means that the cube has no "forward" face and rolls around randomly.
//          {  
//          light_tracking_internal_state = "parallel";
//          b_counter = 30;  // This just flashes the lights blue 5 times
//          Serial.println("bldcaccel f 4300 2000");
//          light_tracking_delay_counter = 60;
//          }
//      // Forward face is second brightest: Action: Roll cautionsly forward
//          else if(brightest[1] == for_face && sum_of_horizontal > threshold)
//          {
//          light_tracking_internal_state = "f";
//          face_rgb(for_face,0,1,1,1);
//          Serial.println("bldcaccel f 4000 500");
//          light_tracking_delay_counter = 40;
//          }
//      // Reverse Face is second Brightest: Action: Roll cautionsly in reverse
//          else if(brightest[1] == rev_face && sum_of_horizontal > threshold)
//          {
//          light_tracking_internal_state = "r";
//          face_rgb(rev_face,1,1,1,1);
//          Serial.println("bldcaccel r 4000 500");
//          light_tracking_delay_counter = 40;
//          }
//      // None of these conditions are met...
//          else
//          {
//          light_tracking_internal_state = "unsure";
//          Serial.println("ia f 4000 3000 20 a 20 r");
//          r_counter = 30; // blink red light
//          light_tracking_delay_counter = 80;
//          }
//          
////Step 3: Check to see if we are connected...
//  bool result = true;
//  int j = 0;
//  while(result && (j < 4)){result = (previous_up_face[j] == previous_up_face[j+1]); j++;}
//  if(result){for(int i = 0; i < 11; i++){previous_up_face[i] = i+10;}; demo = "light_track_part_2"; cmd = "g";}
//  }
//  else if(light_tracking_delay_counter == 30) {Serial.println("bldcstop b");}
//  else if(light_tracking_delay_counter == 45 && light_tracking_internal_state == "parallel") {delay(4);Serial.println("brake f 3000 20");light_tracking_delay_counter = 17;}
//  else if(light_tracking_delay_counter == 23) {delay(4);Serial.println("bldcstop b");}
//  else if(light_tracking_delay_counter == 18) {delay(8);Serial.println("bldcstop b");}
//}
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
//void move_randomly_in_horizontal_plane_three_times()
//{
//  demo = "nothing";
//  change_plane_counter = 0;
//  blink_IR_old(3);
//  blink_rainbow_old(4);
//  for(int i = 0; i < 3; i++){Serial.println("stillalive");delay(100);Serial.println("ia f 4000 3000 20");for(int i = 0; i< 3; i++){delay(1000);}}
//}
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
//  Serial.println(command);
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
//  }
//  
//  void ready_set_jump()
//  {
//    if(ambient_values[face_that_is_up][0] > 0)
//    {
//      delay(1000);if(is_ros_on){nh.spinOnce();}
//      move_normal("f","90 deg",6000, 24, 12,"e 10", 3500);
//      delay(600);if(is_ros_on){nh.spinOnce();}
//      Serial.println("stillalive"); 
//      move_normal("f","90 deg",14000, 34, 22,"e 10", 4000);
//      delay(1200);if(is_ros_on){nh.spinOnce();}
//    }
//  }
//

