// We wish to define four behaviors, in order of coding priority:
//  1. Step toward a light source, solo
//  2. Step toward a light source, in tandem with an adjacent block
//  3. Step in the direction of an arrow, provided on an adjacent face
//  4. Step in a direction provided via external communication (WiFi)

#include "Behavior.h"
#include "Initialization.h"       // Includes .h files for each of the "tabs" in arduino
#include "Cube.h"     // Includes .h files for each of the "tabs" in arduino
#include "Face.h"     // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"                // Includes .h files for each of the "tabs" in arduino
#include "Communication.h"        // Includes wifi 
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

Behavior chilling(Cube* c, bool r, bool g, bool b)
{
  Serial.println("WTF");
  c->blockingBlink(r,g,b,200,30);
  c->shutDown();
}

Behavior duoSeekLight()
{

}


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


