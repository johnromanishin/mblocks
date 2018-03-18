void change_plane_to_parallel(int plane)
{
  if(plane == 5 || plane == 6){cmd = "cp5";}
  else if (plane == 2 || plane == 4){cmd = "cp2";}
  else if (plane == 1 || plane == 3){cmd = "cp1";}
}

bool go_to_plane(int desired_plane)
{
//STEP 0: Initialize variables, check if we are already in correct plane...
    int lattice_connected = 0;
    int up_face = which_face_is_up(12000);
    face_that_was_last_up = up_face;
    int current_plane = which_plane_fast();
    if(current_plane == desired_plane){return(true);}
    
//STEP 1: Check to see if we are attached to a lattice...
    // Perform Wiggle Test --- IF NOT RETURN: also look if face_that_is_up changed... RETURN IF SO
    if(current_plane > 1000)
        {
        if(wiggle_test("f") < 500){lattice_connected = 1;}
        }
    if(up_face == which_face_is_up(12000)){lattice_connected = 1;}
    else{lattice_connected = 0;return(false);}
    
//Step 2:
    long start_time = millis();
    Serial.println("sma retract 7000"); // Begin by retracting the pin, allowing the central part to spin
    delay(400); // wait to let it take effect
    int milliamps_updateable = 4000; // these are the default values for the burst of acceleration
    int milliseconds_updateable = 100; // time default
    milliseconds_updateable += plane_change_offset;
    milliamps_updateable += 2*plane_change_offset;
    int loop_count = 0;
    int loop_count_1 = 0;
    while(millis()-start_time < 7000 && current_plane != desired_plane) // while we aren't in correct plane, and we are still under 6 seconds...
    {
     Serial.println("stillalive");
     sum_of_diff = 0;
     delay(50);
     String command_forward = "bldcaccel f " + String(2300+loop_count_1*15) + " " + String(110 + loop_count_1 + plane_change_offset/3);
     String command_reverse = "bldcaccel r " + String(2300+loop_count_1*15) + " " + String(110 + loop_count_1 + plane_change_offset/3);
     if(current_plane > 1000)
     {
     accel_jump("f", milliamps_updateable, milliseconds_updateable); // try to move plane 
     delay(400);if(is_ros_on){nh.spinOnce();}
     loop_count += 25; // This counter counts how many times we have tried accel_jump
     }
     else{
     delay(100);
     current_plane = which_plane_fast();
     if((current_plane == 52 && desired_plane == 1234) || (current_plane == 21 && desired_plane == 1536) || (current_plane == 15 && desired_plane == 2546)) {Serial.println(command_forward); delay(400);if(is_ros_on){nh.spinOnce();} loop_count_1 += 10;}
     else if( (current_plane == 52 && desired_plane == 1536) || (current_plane == 21 && desired_plane == 2546) || (current_plane == 15 && desired_plane == 1234)) {Serial.println(command_reverse); delay(400);if(is_ros_on){nh.spinOnce();} loop_count_1 += 10;}
     else if(current_plane == desired_plane){break;}
     else if(current_plane > 1000) {r_counter = 4;Serial.println("bldcaccel f 4000 140");delay(600);if(is_ros_on){nh.spinOnce();}}
     else{Serial.println("bldcaccel r 4000 200"); delay(200); Serial.println("bldcstop b");}
     }
     if(sum_of_diff < 10000){milliamps_updateable += 300; milliseconds_updateable += 40;}
     milliseconds_updateable += loop_count;
     current_plane = which_plane_fast();
     delay(20);
    }
    // This section of code runs when the timer is up... or if there is a problem with the plane
    current_plane = which_plane_fast(); //which_plane_cautious();
    delay(50);
    if(current_plane != desired_plane && current_plane > 1000) // if the central part is not alligned with any axis.
    {
     Serial.println("stillalive");
     delay(500);Serial.println("sma retract 2000");
     delay(50);Serial.println("bldcaccel r " + String(2500+plane_change_offset*15) + " " + String(110 + plane_change_offset/3));
     delay(200);Serial.println("bldcaccel f " + String(2600+plane_change_offset*15) + " " + String(110 + plane_change_offset/3));
     Serial.println("stillalive");
     }
    while(millis()-start_time < 8000){delay(400);nh.spinOnce();}
    if(current_plane == desired_plane){return(true);}
    else{return(false);}
}

int accel_jump(String for_rev, int milliamps, int milliseconds)
{
     Serial.println("stillalive");
     if(milliamps > 5500){milliamps = 5000;}
     delay(20);
     //read_accel_central(); // now AcX_c, AcY_c, and AcZ_c are updated
     //delay(75);
     String command = "bldcaccel "+ for_rev + " " + String(milliamps) + " " + String(100 + milliseconds);
     //                bldcaccel        f/r        4000                            300
     Serial.println(command);
     int numb = 2;
     if(milliseconds > 200 && milliseconds < 300){numb = 3;}
     else if(milliseconds > 300 && milliseconds < 400){numb = 4;}
     else if(milliseconds > 400 && milliseconds < 500){numb = 5;}
     else if(milliseconds > 500 && milliseconds < 600){numb = 6;}
     else if(milliseconds > 600 && milliseconds < 700){numb = 7;}
     delay(50);
     Serial.println("stillalive");
     for(int i = 0; i < numb; i++) {sum_of_diff += wait_and_detect_difference_maget();}
     Serial.println("bldcstop b");
     delay(50);
     for(int i = 0; i < 3; i++){sum_of_diff += wait_and_detect_difference_maget(); delay(100);}
     delay(30);
     Serial.println("stillalive");
}

int wait_and_detect_difference_maget()
{
 Serial.println("stillalive");
 int variance_of_mag_readings = 0;
 int sum_of_mag_readings = 0;
 int mag_readings[5] = {0,0,0,0,0};
 for(int i = 0; i<5;i++){mag_readings[i] = read_5048_angle(0x40);delay(20);}
 for(int i = 0; i<5;i++){sum_of_mag_readings += mag_readings[i];}
 for(int i = 0; i<5;i++){variance_of_mag_readings += abs(sum_of_mag_readings/5 - mag_readings[i]);}
 return(variance_of_mag_readings);
}
int wait_and_detect_difference()
{
 Serial.println("stillalive");
 delay(50);
 AcX_c_old = AcX_c; AcY_c_old = AcY_c; AcZ_c_old = AcZ_c;
 delay(5);
// Serial.println(AcX_c_old); 
// Serial.println(AcX_c);
// Serial.println(AcY_c_old);
// Serial.println(AcY_c);
// Serial.println(AcZ_c_old);
// Serial.println(AcZ_c);
 delay(30);
 //Serial.println("stillalive");
 sum_of_diff += (abs(AcX_c_old - AcX_c) + abs(AcY_c_old - AcY_c) + abs(AcZ_c_old - AcZ_c));
 //Serial.print("sum_of_diff = ");
 //Serial.println(sum_of_diff);
 delay(10);
}

//    if      ((AcX_c > (1 - e))     && (AcX_c < (1 + e))     && (AcY_c > (1 - e))     && (AcY_c < (1 + e))     && (AcZ_c > (16000 - e)) && (AcZ_c < (16000 + e)) ){best_guess = 1234;}
//    else if ((AcX_c > (1 + e))     && (AcX_c < (10000))     && (AcY_c > (1 + e))     && (AcY_c < (10000))     && (AcZ_c > -1000)       && (AcZ_c < (16000 + e)) ){best_guess = 52;}
//    else if ((AcX_c > (11000 - e)) && (AcX_c < (11000 + e)) && (AcY_c > (11000 - e)) && (AcY_c < (11000 + e)) && (AcZ_c > (1 - e))     && (AcZ_c < (1 + e))     ){best_guess = 1536;}
//    else if ((AcX_c > (10000 - e)) && (AcX_c < (16000 + e)) && (AcY_c > (-11000 - e))&& (AcY_c < (10000 - e)) && (AcZ_c > (-7000 - e)) && (AcZ_c < (1 - e))     ){best_guess = 21;}
//    else if ((AcX_c > (11000 - e)) && (AcX_c < (11000 + e)) && (AcY_c > (-11000 - e))&& (AcY_c < (-11000 + e))&& (AcZ_c > (1 - e))     && (AcZ_c < (1 + e))     ){best_guess = 1536;}
//    else if ((AcX_c > (1 - e))     && (AcX_c < (11000 + e)) && (AcY_c > (-11000 - e))&& (AcY_c < (1 + e))     && (AcZ_c > (-11000 - e))&& (AcZ_c < (14000 - e)) ){best_guess = 15;}
//    else{best_guess = -1;}
//   }









bool rock_back_and_forth()
{
Serial.println("bldcaccel f 4000 400");
if(is_ros_on){publishAwake();}
if(is_ros_on){nh.spinOnce();}
delay(400);
Serial.println("bldcstop b");
delay(400);
Serial.println("bldcaccel r 4000 400");
delay(400);
if(is_ros_on){publishAwake();} 
if(is_ros_on){nh.spinOnce();}
Serial.println("bldcstop b");
delay(400);
}

//Move returns
// 0 = move failed... 
// -number = move failed and wrong face is up + Violence makes number Higher
// 90 for 90 degree movement
// 180 for 180 degree movement
// 45, and 135 for partial moves...
int move_normal(String for_rev, String rotation_ammount, int flywheel_rpm, int brake_current, int brake_time, String ending,  int delay_time)
{
  int attempts = 3;
  int up_face = which_face_is_up(12000);
  int forward_face = which_face_is_forward(which_plane_fast());
  int reverse_face = opposite_face(forward_face);
  int expected_up_face = -1;
  int backup_expected_up_face = opposite_face(up_face);
 
  // This area reasons to figure out expected_up_face, and applies the cube specific offsets to 
  if(for_rev == "r")
  {
      brake_current = brake_current*reverse_offset; 
      if(rotation_ammount == "90 deg"){expected_up_face = forward_face;}
      else if(rotation_ammount == "180 deg"){expected_up_face = opposite_face(up_face);}
      else if(rotation_ammount == "0 deg"){expected_up_face = -1;}
      else{return(false);}
  }
      
  else if(for_rev == "f")
  {
      brake_current = brake_current*forward_offset;
      if(rotation_ammount == "90 deg"){expected_up_face = reverse_face;}
      else if(rotation_ammount == "180 deg"){expected_up_face = opposite_face(up_face);}
      else if(rotation_ammount == "0 deg"){expected_up_face = -1;}
      else{return(false);}
      
  }
  else
  {return(false);} // if string is neither "f" or "r" we exit...
  
  if(expected_up_face == -1) // this is for moves where we don't know what will be the new face... check movement ammount
  {
      rgbb(); // turn face LED's to be blue! JUST FOR FUN
      String command = "ia "+ for_rev +" " + String(flywheel_rpm)+" "+String(brake_current+special_offset)+" "+String(brake_time)+" "+ ending;
      Serial.println(command);
      delay(250); rgboff();
      attempts += 1;
      for(int i = 1; i < 3;i++){Serial.println("stillalive");delay(50);}
      delay(delay_time/2);
      for(int i = 1; i < 3;i++){Serial.println("stillalive");delay(50);}
      delay(delay_time/2);
      Serial.println("stillalive");  
  }
  else // in this case we will move until we either exhaust number of tries, or we end up in the correct face
  {
    while(up_face != backup_expected_up_face && up_face != expected_up_face && attempts <= 3 && which_face_is_up(12000) < 7)
    {
      rgbp(); // turn face LED's to be purple! JUST FOR FUN
      String command = "ia "+ for_rev +" " + String(flywheel_rpm)+" "+String(brake_current+special_offset)+" "+String(brake_time)+" "+ ending;
      Serial.println(command);
      delay(250); rgboff();
      attempts += 1;
      for(int i = 1; i < 3;i++){Serial.println("stillalive");delay(50);}
      delay(500);
      for(int i = 1; i < 3;i++){Serial.println("stillalive");delay(50);}
      delay(500);
      Serial.println("stillalive");      
      // If the command is printed again, This means we failed, so we now bump up all of the parameters, to hopefully succeed next time
      brake_current = brake_current + 300; 
      flywheel_rpm = flywheel_rpm + 400;
      brake_time = brake_time + 2;
      up_face = which_face_is_up(12000);
    }
  }
       if(up_face == expected_up_face)            {Serial.println("stillalive");rgbg();delay(250);rgboff();return(90);}
  else if(up_face == backup_expected_up_face)     {Serial.println("stillalive");rgbg();delay(250);rgboff();return(180);}
  else                                            {Serial.println("stillalive");rgbr();delay(250);rgboff(); return(0);}
}




