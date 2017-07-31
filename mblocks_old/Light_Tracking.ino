//////// How Long did this take?
//long begin_time = millis();
//Serial.print("Function took: ");Serial.println(millis()-begin_time);
//////// 

int display_brightest_face()
{
  digitalWrite(Switch, HIGH); rgboff(); //if(DEBUG){Serial.println("Beginning light_track_update");}
  light_tracking_internal_state = "nothing";
  int threshold = 3;
  int for_face = which_face_is_forward(which_plane_fast());
  int rev_face = opposite_face(for_face);
  int up_face = which_face_is_up(12000);
  int value_of_top_face = read_ambient(up_face);
  int fblight_list[7] = {0,0,0,0,0,0,0};
  int sum_of_horizontal = 0;
  for(int x = 1; x < 7; x++){fblight_list[x] = read_ambient(x);} // Populates temp. array of light values
  if(up_face > 0 && up_face <7)    {fblight_list[up_face] = -1;} // Zeros out the exclude face
  else                             {for(int x = 1; x < 7; x++){fblight_list[x] = 0;}} // this means cube isn't stable, so we zero everything
  for(int x = 1; x < 7; x++){sum_of_horizontal+=fblight_list[x];}
  int brightest[2] = {which_face_is_brightest(fblight_list),which_face_is_brightest(fblight_list)};
  Serial.print("Brightest face is: ");Serial.println(brightest[0]);
  //Serial.print("
  face_rgb(brightest[0],0,1,1,1);
}

int light_track_update()
{
//Step 1: Update Sensor Values //
if(DEBUG){Serial.println("Running light_track_update");}
if(light_tracking_delay_counter < 1) // THIS means that we are done with the previous iteration of this, and we now recheck the values and plan our action
  {
  rgboff(); //if(DEBUG){Serial.println("Beginning light_track_update");}
  light_tracking_internal_state = "nothing";
  int threshold = 3;
  int for_face = which_face_is_forward(which_plane_fast());
  int rev_face = opposite_face(for_face);
  int up_face = which_face_is_up(12000);
  int value_of_top_face = read_ambient(up_face);
  int fblight_list[7] = {0,0,0,0,0,0,0};
  int sum_of_horizontal = 0;
  for(int x = 1; x < 7; x++){fblight_list[x] = read_ambient(x);} // Populates temp. array of light values
  if(up_face > 0 && up_face <7)    {fblight_list[up_face] = -1;} // Zeros out the exclude face
  else                             {for(int x = 1; x < 7; x++){fblight_list[x] = 0;}} // this means cube isn't stable, so we zero everything
  for(int x = 1; x < 7; x++){sum_of_horizontal+=fblight_list[x];}
  int brightest[2] = {which_face_is_brightest(fblight_list),which_face_is_brightest(fblight_list)};
  for(int j = 10; j > 0; j--){previous_up_face[j] = previous_up_face[j-1];} previous_up_face[0] = up_face;

//Step 2: Choose Action
     // Forward face has brightest light: Action move forward
     if(brightest[0] == for_face && sum_of_horizontal > threshold) // 
        {
          
          light_tracking_internal_state = "f";
          face_rgb(for_face,0,0,1,1);
          delay(10);
          Serial.println("bldcspeed f 5700");
          light_tracking_delay_counter = 95;
        }
     // Reverse face has brightest light: Action move forward
     else if(brightest[0] == rev_face && sum_of_horizontal > threshold)
          {
          light_tracking_internal_state = "r";
          face_rgb(rev_face,1,0,1,1);
          delay(10);
          Serial.println("bldcspeed r 5700");
          light_tracking_delay_counter = 95;
          }
     // Cube flywheel is coplaner with ground...: go crazy!!!!!!
          else if(for_face > 7) // This means that the cube has no "forward" face and rolls around randomly.
          {  
          light_tracking_internal_state = "parallel";
          b_counter = 30;  // This just flashes the lights blue 5 times
          Serial.println("bldcaccel f 4300 2000");
          light_tracking_delay_counter = 60;
          }
      // Forward face is second brightest: Action: Roll cautionsly forward
          else if(brightest[1] == for_face && sum_of_horizontal > threshold)
          {
          light_tracking_internal_state = "f";
          face_rgb(for_face,0,1,1,1);
          Serial.println("bldcaccel f 4000 500");
          light_tracking_delay_counter = 40;
          }
      // Reverse Face is second Brightest: Action: Roll cautionsly in reverse
          else if(brightest[1] == rev_face && sum_of_horizontal > threshold)
          {
          light_tracking_internal_state = "r";
          face_rgb(rev_face,1,1,1,1);
          Serial.println("bldcaccel r 4000 500");
          light_tracking_delay_counter = 40;
          }
      // None of these conditions are met...
          else
          {
          light_tracking_internal_state = "unsure";
          Serial.println("ia f 4000 3000 20 a 20 r");
          r_counter = 30; // blink red light
          light_tracking_delay_counter = 80;
          }
          
//Step 3: Check to see if we are connected...
  bool result = true;
  int j = 0;
  while(result && (j < 4)){result = (previous_up_face[j] == previous_up_face[j+1]); j++;}
  if(result){for(int i = 0; i < 11; i++){previous_up_face[i] = i+10;}; demo = "light_track_part_2"; cmd = "g";}
  }
  else if(light_tracking_delay_counter == 30) {Serial.println("bldcstop b");}
  else if(light_tracking_delay_counter == 45 && light_tracking_internal_state == "parallel") {delay(4);Serial.println("brake f 3000 20");light_tracking_delay_counter = 17;}
  else if(light_tracking_delay_counter == 23) {delay(4);Serial.println("bldcstop b");}
  else if(light_tracking_delay_counter == 18) {delay(8);Serial.println("bldcstop b");}
}

void light_track_update_part2()
{
  
  bool result = false;
  if   (change_plane_counter < 3){change_plane_counter++;change_plane_to_parallel(which_face_is_up(12000));}
  else  // now we do
    {
      if(which_plane_fast() > 1000){move_randomly_in_horizontal_plane_three_times();}
    }
  
  // Step 1: Look up up face...
  // Step 2: Figure out what plane corresponds to horizontal...
  // if(current_plane != horizontal plane)
//  {
//    change plane...
//  }
     // else if(not 100% connected){Serial.println("ia f/r 5000 4000 10");
     // else if(100% connected){flash_green(5);cmd = "g"}
     // else {  }// we don't know what is going on here...
}

void move_randomly_in_horizontal_plane_three_times()
{
  demo = "nothing";
  change_plane_counter = 0;
  blink_IR_old(3);
  blink_rainbow_old(4);
  for(int i = 0; i < 3; i++){Serial.println("stillalive");delay(100);Serial.println("ia f 4000 3000 20");for(int i = 0; i< 3; i++){delay(1000);}}
}

int wiggle_test(String for_rev)
//
//read_accel() read_gyro();
//AcX,AcY,AcZ,GyX,GyY,GyZ,
{
  if(for_rev == "f")        {}
  else if(for_rev == "r")   {}
  else                      {for_rev = "f";}
  Serial.println("stillalive");
  if(DEBUG){Serial.println("BEGINNING WIGGLE_TEST");}
  int movement = 0;
  delay(50);
  String command = "bldcaccel "+ for_rev + " " + String(2800 + 3*plane_change_offset) + " " + String(700 + plane_change_offset);
  Serial.println(command);
  ////////////////////////////// Reads the gyro for 12*65 ms
  for(int i = 0; i < 12; i++)
  {
   movement += read_gyro(MPU_parasite)/100;
   delay(65);
  }
  Serial.print("stillalive");
  delay(40);
  delay(plane_change_offset);
  //////////////////////////////
  Serial.println("bldcstop b");
  delay(40);
  Serial.println("bldcstop b");
  delay(40);
  Serial.print("stillalive");
  ////////////////////////////// Reads the gyro for 12*65 ms
  for(int i = 0; i < 8; i++)
  {
   movement += read_gyro(MPU_parasite)/100;
   delay(30);
  }
  //////////////////////////////
  Serial.println("stillalive");
  return(movement);
}

int which_face_is_brightest(int fblight_temp[])
// Sorts input list (fblight_temp). Running it a second time, returns 2nd brightest...
// running a 3rd time returns 3rd brightest...
{
  int brightest_list[6] = {0,0,0,0,0,0};
  for(int i = 1; i < 7 ;i++){if(fblight_temp[brightest_list[0]] < fblight_temp[i]){brightest_list[0] = (i);}}
  fblight_temp[brightest_list[0]] = 0;
  return(brightest_list[0]);
}


  void light_track_part_2() 
  {
    light_track_update_part2();
  }

  void light_track_update_part_2() {light_track_update_part2();}

  void roll_to_plane()
  {
    if(ambient_values[face_that_is_up][0] > 0)
    {
           if(most_recent_traverse_result == 180 )   {cmd = "g"; demo = "nothing";}
      else if(attempts_traverse > 5)                 {cmd = "r"; demo = "nothing";}
      else if(most_recent_traverse_result == 90)     {Serial.println("stillalive"); most_recent_traverse_result = move_normal("f","90 deg",6000, 24, 12,"e 10", 4000);attempts_traverse++;}
      else if(most_recent_traverse_result == 0)      {Serial.println("stillalive"); most_recent_traverse_result = move_normal("f","90 deg",6000, 24, 12,"e 10", 4000);attempts_traverse++;}
    }
    else if(ambient_values[face_that_is_up][0])   {y_counter = 8;}
  }
  
  void ready_set_jump()
  {
    if(ambient_values[face_that_is_up][0] > 0)
    {
      delay(1000);if(is_ros_on){nh.spinOnce();}
      move_normal("f","90 deg",6000, 24, 12,"e 10", 3500);
      delay(600);if(is_ros_on){nh.spinOnce();}
      Serial.println("stillalive"); 
      move_normal("f","90 deg",14000, 34, 22,"e 10", 4000);
      delay(1200);if(is_ros_on){nh.spinOnce();}
    }
  }


