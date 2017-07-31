//void print_out_connections()
//{
//  Serial.println("__________________");
//  Serial.print("HI! I am cube number: "); Serial.println(cube_id);
//  //
//  for(int face = 1; face < FACES; face++)
//  {
//    
//    Serial.print(" face: ");
//    Serial.print(face);
//    Serial.print(" is connected to cube");
//    Serial.print(face_neighbors[face][n_cube_id]);
//    Serial.print(" face ");
//    Serial.print(face_neighbors[face][n_face_id]);
//    //Serial.print(" face ");
//    //Serial.print(face_neighbors[face][n_face_id]);
//    Serial.println(" WOOOOOOO!");
//    
//  }
//  Serial.println("__________________");
//}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// BEGIN MAIN LOOP///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//long quick_timer            = 0;
//int quick_timer_length      = 250; // in milliseconds
//int x10xfast_loop_counter   = 0; // this counts 1...2....3....4..5...6.. every (quick_timer_length) ms
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//long slow_timer             = 0;
//int slow_timer_length       = 1000; // in milliseconds This sets how often the portion of the code is run
//int loop_counter            = 0;
//////////////////////////////////////////////////
//void loop()
//{
//  if(true)                   { things_to_do_as_fast_as_possible(); } // happens as fast as possible
//  if(millis() > quick_timer) { things_to_do_at_fast_rate_______(); } // happens roughly every 100 ms
//  if(millis() > slow_timer)  { things_to_do_at_slow_rate_______(); } // happens roughly every 1000 ms
//}
//
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// FAST RATE ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//void essentials_of_fast_things()
//{
//   x10xfast_loop_counter++; quick_timer = millis() + quick_timer_length; // Deal with updating loop timer
//   shift_and_update_IMU();
//   shift_and_update_lights();
//}
//
//void  things_to_do_at_fast_rate_______()
//{
//  essentials_of_fast_things();
//  if(is_ros_on){process_general_ros();}
//  move_counter--; 
//  light_tracking_delay_counter--;
//  
//       if(demo == "light_track" && ambient_values[face_that_is_up][0] > 3)       {light_track_update();}
//  else if(demo == "show_brightest")                                              {display_brightest_face();}
//}
//
//
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// SLOW RATE ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//void essentials_of_slow_things()
//{
//  loop_counter++; 
//  check_5048_frame();
//  Serial.println("stillalive"); // general upkeep work...
//  process_5048();
//  slow_timer = millis() + slow_timer_length+random(50);                 // Deal with updating loop timer
//  shift_and_update_ambient_sensors();
//  delay(5);
//  publish_neighbor_info();
//  if(DEBUG)                                                             {Serial.print("loop_counter = "); Serial.println(loop_counter);}
//  //if(motion_sum_log[0] == 0 && motion_sum_log[3] == 0)                  {attempt_reset_i2c(); for(int i = 0; i< 3; i++){Serial.println("esprst"); delay(40);}}
//  if(is_ros_on)                                                         {publishAwake();}  
//  if(loop_counter % 19 == 0)                                            {fbrxen(true);}
//}
//
//void things_to_do_at_slow_rate_______()
//{
//  ///////////////////////////////////////////////General Upkeep///////////////////////////////////////////////
//  essentials_of_slow_things();
//  if(DEBUG && loop_counter % 5){Serial.print(cmd);Serial.println(" is the current cmd");}
//    ///////////////////////////////////////////////demos///////////////////////////////////////////////
//       if(demo == "light_track_part_2")    {light_track_update_part_2();}
//  else if(demo == "roll_to_plane")        {roll_to_plane();}
//  else if(demo == "ready_set_jump")       {ready_set_jump();}
//  else if(demo == "find_connections")     
//    {
//    Serial.println("yo!");
//    }
//    ///////////////////////////////////////////////Random Checking///////////////////////////////////////////////
//  //Serial.println(demo);
//  //if(motion_sum_log[0] > 8000 && motion_sum_log[1])                     {main_alt_choose_color_menu();} // This activates the menu system if we shake the module
//  if(loop_counter % 5  == 0)                                            {Serial.print("sum_of_motion = ");{Serial.println(motion_sum_log[0]);}}
//  if(loop_counter % 251 == 0)                                           {get_battery_voltage();}
//  if(loop_counter % 10 == 0)                                            {which_plane_fast();}
//  if(loop_counter % 11 == 0)                                            {which_face_is_forward(which_plane_fast());}
//  //if(loop_counter % 13 == 0 && is_ros_on)                               {publishstatus();}
//  if(loop_counter % 27 == 0)                                            {if(get_vin() > 3400){turn_off_esp();}}
//  if(loop_counter % (60*10) == 0)                                       {cmd = "sleep";} // this puts the module to sleep after 10 minutes of running
//} 
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// HELPER FUNCTIONS//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//void process_general_ros()
//{
//  if(cmd_received != "nothing") // only process this once...
//  {
//  if(cmd_received.substring(0,2).toInt() == cube_id){cmd = cmd_received.substring(2,3);} // this command was meant for a specific cube.. only works with colors WIP
//  else if(cmd_received.substring(0,2).toInt() > 0){}
//  else if(cmd_received.substring(0,2).toInt() == 0){cmd = cmd_received;}
//  cmd_received = "nothing";
//  }
//}
//
//void print_out_ambient_buffer(int face)
//{
//  Serial.print("Face: ");Serial.print(face);Serial.print("  [most recent]");
//  for(int i = 0; i < ARRAY_LENGTH_LONG-1; i++)
//  {
//    Serial.print(" | ");Serial.print(ambient_values[face][i]);
//   }
//   Serial.print(" | ");Serial.println(ambient_values[face][ARRAY_LENGTH_LONG-1]);
//}
//
//void reset_face_neighbors()
//{
//  for(int face = 0; face < 7; face++)
//    {
//      for(int index = 0; index < 7;  index++)
//          {
//          face_neighbors[face][index] = 0;
//          }
//    }
//  Serial.println("all done! Erasing current connections");
//  print_out_connections();
//}
//
//void print_out_connections()
//{
//  Serial.println("__________________");
//  Serial.print("HI! I am cube number: "); Serial.println(cube_id);
//  //
//  for(int face = 1; face < FACES; face++)
//  {
//    
//    Serial.print(" face: ");
//    Serial.print(face);
//    Serial.print(" is connected to cube");
//    Serial.print(face_neighbors[face][n_cube_id]);
//    Serial.print(" face ");
//    Serial.print(face_neighbors[face][n_face_id]);
//    //Serial.print(" face ");
//    //Serial.print(face_neighbors[face][n_face_id]);
//    Serial.println(" WOOOOOOO!");
//    
//  }
//  Serial.println("__________________");
//}
//void reset_esp()
//{
//  Serial.println("esprst");
//}
//
//void shift_and_update_lights()
//{
//   blink_green();
//   blink_yellow();
//   blink_red();
//   blink_blue();
//   blink_purple();
//   blink_teal();  
//   blink_white();
//   blink_ir();
//   blink_ir_face();
//}
//
//void shift_and_update_ambient_sensors()
//{
//   for(int face = 1; face < FACES; face++)
//   {
//      for(int j = ARRAY_LENGTH_LONG-1; j > 0; j--){ambient_values[face][j] = ambient_values[face][j-1];}
//      ambient_values[face][0] = read_ambient(face); 
//   }
//}
//
//void shift_and_update_IMU()
//{
//  for(int j = ARRAY_LENGTH_LONG;  j > 0; j--){motion_sum_log[j] = motion_sum_log[j-1];}  motion_sum_log[0] = read_gyro_and_accel(MPU_parasite); 
//  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){GyX_log[j] = GyX_log[j-1];}  GyX_log[0] = GyX;
//  for(int a = ARRAY_LENGTH_SHORT; a > 0; a--){GyY_log[a] = GyY_log[a-1];}  GyY_log[0] = GyY;
//  for(int b = ARRAY_LENGTH_SHORT; b > 0; b--){GyZ_log[b] = GyZ_log[b-1];}  GyZ_log[0] = GyZ;
//  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){AcX_log[j] = AcX_log[j-1];}  AcX_log[0] = AcX;
//  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){AcY_log[j] = AcY_log[j-1];}  AcY_log[0] = AcY;
//  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){AcZ_log[j] = AcZ_log[j-1];}  AcZ_log[0] = AcZ;
//}
//
//String check_for_patterns_gyros()
//{
//int low = 1200;
//int high = 5000;
//String color = "default";
/////////////////////////////////////////THESE represent rolling about 3 axis...
//     if(motion_sum_log[1] > 15000 && motion_sum_log[8] > 15000 && motion_sum_log[17] > 15000)                                           {color = "angry";cmd = "chill";}
//     if(GyX_log[0] > low && GyX_log[0] < high && GyX_log[3] > low && GyX_log[3] < high && GyX_log[8] > low && GyX_log[8] < high)        {color = "y"; y_counter = 4;}
//else if(GyX_log[0] < -low && GyX_log[0] > -high && GyX_log[3] < -low && GyX_log[3] > -high &&GyX_log[8] < -low && GyX_log[8] > -high)   {color = "p"; p_counter = 4;}
//else if(GyY_log[0] > low && GyY_log[0] < high && GyY_log[3] > low&& GyY_log[3] < high &&GyY_log[8] > low && GyY_log[8] < high)          {color = "r"; r_counter = 4;}
//else if(GyY_log[0] < -low && GyY_log[0] > -high && GyY_log[3] < -low && GyY_log[3] > -high &&GyY_log[8] < -low && GyY_log[8] > -high)   {color = "b"; b_counter = 4;}
//else if(GyZ_log[0] > low && GyZ_log[0] < high && GyZ_log[3] > low && GyZ_log[3] < high &&GyZ_log[8] > low && GyZ_log[8] < high)         {color = "t"; t_counter = 4;}
//else if(GyZ_log[0] < -low && GyZ_log[0] > -high && GyZ_log[3] < -low && GyZ_log[3] > -high &&GyZ_log[8] < -low && GyZ_log[8] > -high)   {color = "g"; g_counter = 4;}
//return(color);
//}
//
//void attempt_reset_i2c()
//{
//Serial.println("RESETING i2c NOW!!!");
//digitalWrite(Switch, LOW);
//delay(100);
//digitalWrite(Switch, HIGH);
//Wire.begin(2, 14);
//}
//
//void things_to_do_as_fast_as_possible()
//{
//      if(cmd == "sleep")                          {go_to_sleep();}
//      if(cmd == "off")                            {rgboff();IRLED_OFF();delay(40);cmd ="chill";}// go_to_plane(int plane)
//      if(cmd == "esp_off")                        {turn_off_esp(); cmd = "chill";}
//      if(cmd == "esp_reset")                      {reset_esp();    cmd = "chill";}
//      if(is_ros_on)                               {nh.spinOnce();} 
//      if(cmd == "update" && is_ros_on)            {publishstatus(); cmd = cmd_previous;}
//      if(cmd == "chill")                          {delay(1);}
//      if(cmd == "special")                        {delay(1);}
//      
//      if(cmd == "r")                              {rgbr(); color = "r"; cmd = "chill";}
// else if(cmd == "g")                              {rgbg(); color = "g"; cmd = "chill";}
// else if(cmd == "b")                              {rgbb(); color = "b"; cmd = "chill";}
// else if(cmd == "p")                              {rgbp(); color = "p"; cmd = "chill";}
// else if(cmd == "y")                              {rgby(); color = "y"; cmd = "chill";}
// else if(cmd == "t")                              {rgbt(); color = "t"; cmd = "chill";} 
// else if(cmd == "w")                              {rgbw(); color = "w"; cmd = "chill";} 
//    
//      cmd_previous = cmd;
//      
//}
//
//
//
//void check_for_commands()
//{
//      if(cmd == "find_connections")               {reset_face_neighbors(); demo = "find_connections"; cmd = "chill"; Serial.println("Demo reset");} 
//      if(cmd == "f1")                             {cmd = "chill";}
//      if(cmd == "f2")                             {cmd = "chill";}
//      if(cmd == "lit")                            {face_rgb(which_face_is_up(12000),1,0,1,0);}
//      if(cmd == "rain")                           {taste_the_rainbow();}
//      if(cmd == "cp1")                            {if(go_to_plane(2546)){cmd = "g";} else{cmd = "r";}}
//      if(cmd == "cp5")                            {if(go_to_plane(1234)){cmd = "g";} else{cmd = "r";}}
//      if(cmd == "cp2")                            {if(go_to_plane(1536)){cmd = "g";} else{cmd = "r";}}
//      if(cmd == "dance")                          {Serial.println("bldcaccel f 5000 800"); cmd ="rain";}
//      if(color == "r")                            {if(x10xfast_loop_counter % 5 == 0) {rgbr();}}
// else if(color == "g")                            {if(x10xfast_loop_counter % 5 == 0) {rgbg();}}
// else if(color == "b")                            {if(x10xfast_loop_counter % 5 == 0) {rgbb();}}
// else if(color == "p")                            {if(x10xfast_loop_counter % 5 == 0) {rgbp();}}
// else if(color == "y")                            {if(x10xfast_loop_counter % 5 == 0) {rgby();}} 
// else if(color == "t")                            {if(x10xfast_loop_counter % 5 == 0) {rgbt();}}
// else if(color == "w")                            {if(x10xfast_loop_counter % 5 == 0) {rgbw();}}
//  if(cmd.substring(0,8) == "forward" || cmd.substring(0,8) == "reverse")
//  {
//            
//         if(cmd == "forward_traverse" || cmd == "traverse_forward")   {move_normal("f","90 deg",6000, 24, 12,"e 10", 3000);cmd ="chill";}
//    else if(cmd == "reverse_traverse" || cmd == "traverse_reverse")   {move_normal("r","90 deg",6000, 24, 12,"e 10", 3000);cmd ="chill";}
//    else if(cmd == "forward_corner" || cmd == "corner_forward")       {move_normal("f","180 deg",14000, 40, 5,"e 10", 6000);cmd ="chill";}
//    else if(cmd == "reverse_corner" || cmd == "corner_reverse")       {move_normal("r","180 deg",14000, 40, 5,"e 10", 6000);cmd ="chill";}
//    else if(cmd == "forward_stair")                                   {move_normal("f","90 deg",6000, 23, 10,"e 10", 3000);cmd ="chill";}
//    else if(cmd == "reverse_stair")                                   {move_normal("r","90 deg",6000, 23, 10,"e 10", 3000);cmd ="chill";}
//    else if(cmd == "forward_down")                                    {move_normal("f","0 deg",6000, 24, 12,"e 10", 6000);cmd ="chill";}
//    else if(cmd == "reverse_down")                                    {move_normal("r","0 deg",6000, 24, 12,"e 10", 6000);cmd ="chill";}
//    else if(cmd == "forward_wall")                                    {move_normal("f","90 deg",6000, 23, 10,"e 10", 3000);cmd ="chill";}
//    else if(cmd == "reverse_wall")                                    {move_normal("r","90 deg",6000, 23, 10,"e 10", 3000);cmd ="chill";}
//    else if(cmd == "forward_horizontal_traverse")                     {move_normal("f","180 deg",14000, 40, 5,"e 10", 6000);cmd ="chill";}
//    else if(cmd == "reverse_horizontal_traverse")                     {move_normal("r","180 deg",14000, 40, 5,"e 10", 6000);cmd ="chill";}
//  }
//}



//int blink_green_old(int times)
//{
//Serial.println("stillalive");
//for(int i = 0; i <times;i++){rgbg();delay(100);rgboff();delay(75);} 
//}
//// FUN36
//
//
//int blink_yellow_old(int times)
//{
//Serial.println("stillalive");
//for(int i = 0; i <times;i++){rgby();delay(100);rgboff();delay(75);} 
//}
//// FUN37
//
//
//int blink_purple_old(int times)
//{
//Serial.println("stillalive");
//for(int i = 0; i <times;i++){rgbp();delay(100);rgboff();delay(75);} 
//}
//// FUN38
//
//
//int blink_blue_old(int times)
//{
//Serial.println("stillalive");
//for(int i = 0; i <times;i++){rgbb();delay(100);rgboff();delay(75);} 
//}
//// FUN39
//
//
//int blink_red_old(int times)
//{
//Serial.println("stillalive");
//for(int i = 0; i <times;i++){rgbr();delay(100);rgboff();delay(75);} 
//}
//// FUN40
//
//
//int blink_teal_old(int times)
//{
//Serial.println("stillalive");
//for(int i = 0; i <times;i++){rgbr();delay(100);rgboff();delay(75);} 
//}
//// FUN41
//
//
//int blink_white_old(int times)
//{
//Serial.println("stillalive");
//for(int i = 0; i <times;i++){rgbw();delay(800);rgboff();delay(750);} 
//}
//// FUN42
//
//
//int blink_IR_old(int times)
//{
//Serial.println("stillalive");
//for(int i = 0; i <times;i++){irall();delay(100);IRLED_OFF();delay(750);} 
//}
//// FUN43
//
//
//int blink_rainbow_old(int times)
//{
//for(int i = 0; i <times;i++)
//{Serial.println("stillalive");rgby();delay(130);rgbr();delay(130);rgbp();Serial.println("stillalive");delay(130);rgbb();delay(130);rgbt();delay(130);rgbg();delay(130);Serial.println("stillalive");} 
//}
// 
//int read_accel_central(){ 
//// Function typically takes about 30 milliseconds to complete
//// This function attempts to update these variables
//// AcX_c, AcY_c, AcZ_c
//// Yes this is a giant clusterfuck of convoluted logic... but it seems to work
//// this is the format of the string returned from the NRF51422:
//// imugravityi: 7800 534 31818
//  long begin_function = millis(); // record the moment when the program starts
//  while(Serial.available()){Serial.read();} //This is meant to flush any existing serial characters
//  delay(5);
//  Serial.println("imugravityi"); // Prints imugravityi to the NRF51422
//  delay(20); // Wait a little while for the the Nrf51422 to process this command
//  String temp_string = ""; // Setup a string to begin 
//  int loop_count = 1; // yes... this is the only way I know how to do this
//  char c = 'c';
//  while (Serial.available() > 0 && (millis()-begin_function) < 50) // while there are things in the serial buffer...
//  {
//        if(c != ' '){c = Serial.read();}
//        if(c == ' ') // 
//            {
//                delayMicroseconds(150);
//                c = Serial.read();
//                while(c != ' ' && Serial.available() > 0)
//                  {
//                  temp_string += c;
//                  delayMicroseconds(150);
//                  c = Serial.read();
//                  }
//                if(loop_count == 1){AcX_c = temp_string.toInt()/2;}
//                else if(loop_count == 2){AcY_c = temp_string.toInt()/2;}
//                else if(loop_count == 3){AcZ_c = temp_string.toInt()/2;}
//                loop_count++;
//                temp_string = ""; // reset the temporary string
//             } 
//   }
//  return (abs(AcX_c)+abs(AcY_c)+abs(AcZ_c));
//} //FUN48

//bool main_alt_waiting_on_light(int increments_to_wait, int face, int threshold = 6)
//{
//  //  int threshold = 6;
//  if(DEBUG){Serial.println("Beginning main_alt_waiting_on_light");}
//  long begin_function = millis();
//  Serial.println("stillalive");
//  int incrementer = 0;
//  int loop_counter = 0;
//  int time_intervale = 1000;
//  int initial_ambient_reading = read_ambient(face);
//  Serial.print("initial Ambient Reading = "); Serial.println(initial_ambient_reading);
//  while((read_ambient(face) - initial_ambient_reading) < threshold) 
//    {
//    loop_counter++;
//    if ((millis() - begin_function) > time_intervale) 
//      {
//        Serial.print("loop counter = ");Serial.print(loop_counter);
//        Serial.println("stillalive");Serial.println(incrementer); incrementer++; begin_function+=time_intervale;
//      }
//    if (incrementer > increments_to_wait){if(DEBUG){Serial.println("I was waiting, but nothing showed up... ");} return false;}
//    delay(1);
//    }
//    return true;
//}

//String main_alt_choose_color_menu()
//{
//  if(DEBUG){Serial.println("main_alt_choose_color_menu");}
//  /////// SET UP Environmental Variables
//  String chosen_color = "default"; 
//  String cLog[ARRAY_LENGTH_LONG] = {"r","g","b","y","r","p","r", "g", "b", "y", "r", "p", "r", "g", "b", "y", "r", "p"}; // history of color selections, randomized
//  //////////////////////////////////////
//  reset_main_loop_variables();
//  while(chosen_color == "default" && loop_counter < 50)
//  {
//    if(true)                   {things_to_do_as_fast_as_possible();}
//    if(millis() > quick_timer) { 
//                                 essentials_of_fast_things(); 
//                                 for(int a = 17; a > 0; a--){cLog[a] = cLog[a-1];} cLog[0] = check_for_patterns_gyros(); // shifts the gyro values, looks for a matxh
//                                 if(cLog[1] == cLog[3] && cLog[3] == cLog[7]   && cLog[3]  == cLog[16] && cLog[3] == cLog[3] && cLog[8] == cLog[3] && cLog[11] == cLog[2] && cLog[3] == cLog[15] && cLog[3] != "default"){chosen_color = cLog[3];}
//                               }
//    if(millis() > slow_timer)  {
//                                essentials_of_slow_things();
//                                if(r_counter < 1 && g_counter < 1 && b_counter < 1 && y_counter < 1 && p_counter < 1 && t_counter < 1){ir_counter[0] = 8;} // blink IR twice if we are just sitting there
//                               } 
//  }
//  reset_main_loop_variables();
//  evaluate_output_of_menu(chosen_color);
//  return chosen_color;
//}

//
//String evaluate_output_of_menu(String chosen_color)
//{
//  if(DEBUG){Serial.println("evaluate_output_of_menu");}
//       if(chosen_color == "b")  {blink_sequence_1();blink_blue_old(3);   go_to_sleep();}
//  else if(chosen_color == "g")  {blink_sequence_1();blink_green_old(3);  demo = "lame_and_green";    cmd = "g";}
//  else if(chosen_color == "r")  {blink_sequence_1();blink_red_old(3);    demo = "jump_out_of_hand"; Serial.println("ia f 15000 4500 50");}
//  else if(chosen_color == "y")  {blink_sequence_1();blink_yellow_old(3); demo = "roll_to_plane";    delay(1600);if(is_ros_on){nh.spinOnce();}}
//  else if(chosen_color == "t")  {blink_sequence_1();blink_teal_old(3);   demo = "show_brightest";} //  cmd = "rain";} //which_demo = "cool_color_demo";   cmd = "rain";}
//  else if(chosen_color == "p")  {blink_sequence_1();blink_purple_old(3); demo = "light_track";} // Goes to Sleep...
//  else                          {chosen_color = "default";}
//  return(chosen_color);
//}




//char temp_message[9] ;
//
//char M_L = 'L'; // l = location
//char M_H = 'H'; // h = hey message
//char M_C = 'C'; // c = color
//char M_M = 'M'; // m = motion message
//char M_S = 'S'; // synchronized Command
//char M_D = 'E'; // ERROR - no message
//// color commands
//char M_R = 'R';  
//char M_G = 'G'; 
//char M_B = 'B'; 
//char M_P = 'P'; 
//char M_Y = 'Y';
//char M_T = 'T'; 

//int is_this_face_able_to_send[FACES] =        {0, 1, 1, 1, 1, 1, 1};
//int i_just_sent_an_ir_message[FACES] =        {0, 0, 0, 0, 0, 0, 0};
//int message_general_counter[FACES] =          {0, 0, 0, 0, 0, 0, 0};
//int ir_face_buffer_previous[FACES] =          {0, 0, 0, 0, 0, 0, 0};



//bool process_message(int face)
//{
//  int fb_message_size_temp =  fb_message_size(face);
//  if (fb_message_size_temp >= 5) 
//     {
//      fb_receiveFromRxBuffer(face, fb_message_size_temp);  /// contents are now in temp_message[] - an array
//      if(DEBUG){for(int i = 0; i < 15; i++){Serial.print(" "); Serial.print(temp_message[i]);}}
//      }    
//   else if(fb_message_size_temp > 0 && ir_face_buffer_previous[face] > 0) // this means buffer is still partially full, after one check...
//      {
//          Serial.println("checked partial message");
//          
//          fb_receiveFromRxBuffer(face, fb_message_size_temp);
////          for(int i = 0; i < fb_message_size_temp; i ++) 
////            {
////              temp_message_value = 53;
////                   if(temp_message[i] == 'R'){temp_message_char = 'r'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'G'){temp_message_char = 'g'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'B'){temp_message_char = 'b'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'Y'){temp_message_char = 'y'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'P'){temp_message_char = 'p'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'W'){temp_message_char = 'w'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'T'){temp_message_char = 't'; process_color_message (face, temp_message_value, temp_message_char);}
////            }
//          fbrxflush(face);
//          fb_message_size_temp = 0;
//      }
//   else { }  // this means that fb_message_size(face) is less than or equal to zero
//   ir_face_buffer_previous[face] = fb_message_size_temp;
//   erase_received_message();  
//}
//              
//bool process_messages_all_faces()
//{
//  for(int face = 1; face < ACTIVE_FACES; face++)
//  {
//    if(i_just_sent_an_ir_message[face] > 0) // this is designed to not check for messages immeaditly after sending them...
//    {
//      i_just_sent_an_ir_message[face]--; 
//      if(i_just_sent_an_ir_message[face] == 1) {fbrxflush(face);}
//    }
//    else {process_message(face);}
//  }
//}
////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////
//  // index                      [0]    [1]     [2]      [3]     [4]     [5]     [6]    [7]
////                              type   type    type    [id1]   [id2]    char    type   type
////char location_message[8] =   {'l',  'l',    'l',      '1',    '9',    'a',    'l',   'l'}; // do not pass this message on...
////char hey_message[8] =        {'h',  'h',    'h',      '1',    '4',    'h',    'h',   'l'}; // pass these on by default
//
//void erase_received_message()
//{
//  for(int i = 0; i < ARRAY_LENGTH_LONG; i++){temp_message[i] = '_';}
//}
//
//
//int bitArrayToInt32(bool arr[], int count_)
//{
//    int ret = 0;
//    int tmp;
//    for (int i = 0; i < count_; i++) {
//        tmp = arr[i];
//        ret |= tmp << (count_ - i - 1);
//    }
//    return ret;
//}
//
//void general_counter(int counter_array[])
//{
//   for(int face = 1; face < ACTIVE_FACES; face++)
//  {
//    if(counter_array[face] < 1) {} // we are done... wait for this number to get bumped up again
//    else
//    {     
//           if(counter_array[face] == 5 )  {}
//      else if(counter_array[face] == 2 )  {}
//      counter_array[face]--;
//    }
//  }
//}
//
////color_message, MESSAGE_THRESHOLD);
//bool send_ir_message_on_faces(bool face1, bool face2, bool face3, bool face4, bool face5, bool face6, char message[], int message_length)
//{
//  if(face1) {send_ir_message_char(1, message, message_length);}
//  if(face2) {send_ir_message_char(2, message, message_length);}
//  if(face3) {send_ir_message_char(3, message, message_length);}
//  if(face4) {send_ir_message_char(4, message, message_length);}
//  if(face5) {send_ir_message_char(5, message, message_length);}
//  if(face6) {send_ir_message_char(6, message, message_length);}
//}
//
//bool send_ir_message_char(int face, char message[], int message_length)
//{
//  IRLED(face,0,0,0,0);
//  delay(1);
//  fbtxled(face,1,1,1,1);
//  delay(1);
//  fbtx_send_msg_char(face, message_length, message);
//  i_just_sent_an_ir_message[face] = 3;
//  return(true);
//}
//
/////////// SENDING
//// fbtxcount <facenumber>
//// fbtx <facenumber> <message>
////**** fbtxled <faceNumber> [<irLEDNumber1>] [<irLEDNumber2>] [<irLEDNumber3>] [<irLEDNumber3>]
//
///////// RECEIVING
////**** fbrx <faceNumber> <byteCount>
////**** fbrxflush <faceNumber>
////**** fbrxcount <facenumber>
//
//bool fbtx_send_msg_char(int face, int numBytes, char message[]) 
//{
//    char twiBuf[30];
//    if (face > 6 || face < 1) {return false;}
//    twiBuf[0] = 0x30; //FB_REGISTER_ADDR_TX_BUF;
//    for(int i = 0; i < numBytes; i++){twiBuf[i+1] = message[i];}
//    
//    Wire.beginTransmission(face);
//    for(int i = 0; i < (1 + numBytes); i++)
//    {
//      Wire.write(byte(twiBuf[i]));
//    }
//    Wire.endTransmission();
//    return(true);
//}
//
//int fb_message_size(int face)//, uint8_t *bytes) 
//{
//    int number = 0;
//    if ((face < 1) || (face > 6)) {return -1;}        
//    Wire.beginTransmission(face);// 
//    Wire.write(byte(0x41)); //FB_REGISTER_ADDR_RX_CONSUMED_COUNT 
//    Wire.endTransmission();
//    
//    Wire.requestFrom(face, 1);
//    if (Wire.available() > 0) {number = Wire.read(); Wire.endTransmission();} // read the number stored in faceboard
//    if(number > 0){Serial.print("fb_message_size_buffer: ");Serial.print(number);Serial.print(" on face: ");Serial.println(face);}
//    return number;
//    
//}
//
//bool fb_receiveFromRxBuffer(int face, int numBytes)//, uint8_t *bytes) 
//{
//    if ((face < 1) || (face > 6)) {return -1;}        
//    Wire.beginTransmission(face);// 
//    Wire.write(byte(0x40)); // int i2c1 = 0x40; //FB_REGISTER_ADDR_RX_BUF  0x40 
//    Wire.endTransmission();
//    
//    Wire.requestFrom(face, numBytes);
//    if (Wire.available() >= numBytes) //ambientLight  = twiBuf[0] << 2;
//    {
//      for(int i = 0; i < numBytes; i++)
//      {
//      temp_message[i] = Wire.read(); 
//      }    
//      if(DEBUG){for(int i = 0; i < numBytes; i++) {Serial.print(temp_message[i]);} Serial.println(" ");}
//    }
//    return true;
//}
//
//
//bool fbrxflush(int face)
//{
//  if(DEBUG){Serial.print("Flushing buffer on face: ");Serial.println(face);}
//  if (face < 1 || face > 6){return(false);}
//  digitalWrite(Switch, HIGH);
//  int i2c1 = 0x42; // FB_REGISTER_ADDR_RX_FLUSH  
//  int i2c2 = 0x01;
//  Wire.beginTransmission(face);
//  Wire.write(byte(i2c1));
//  Wire.write(byte(i2c2));
//  Wire.endTransmission();
//  return(true);
//}
//
//bool fbtxled(int face, bool led1, bool led2, bool led3, bool led4)
//{
//  if (face < 1){return(false);}  
//  int i2c1 = 0x33; // FB_REGISTER_ADDR_TX_LED_SELECT
//  int i2c2 = 0x00;
//  if (face > 6) {return(false);}
//  i2c2 = led1 ? 0x01 : 0x00;
//  i2c2 |= led2 ? 0x02 : 0x00;
//  i2c2 |= led3 ? 0x04 : 0x00;
//  i2c2 |= led4 ? 0x08 : 0x00;
//  Wire.beginTransmission(face);
//  Wire.write(byte(i2c1));
//  Wire.write(byte(i2c2));
//  Wire.endTransmission();
//  return(true);
//}
//








//int look_up_other_indexes(int id_number)
//{
//  for(int index = 0; index < how_many_cubes; index++)
//  {
//    if(index == id_number)
//      {
//       return ID_numbers[index];
//      }
//  }
//}

//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// index                                            [0]     [1]     [2]         [3]         [4]       [5]     [6]    [7]    [8]    [9]
////                                                 type    type     (value[1])  (value[2])  char    type   type
//char location_message[STANDARD_MESSAGE_LENGTH] =   {M_L,    M_L,    '1',        '9',        'a',      M_L,    M_L}; // do not pass this message on...
//char hey_message[STANDARD_MESSAGE_LENGTH] =        {M_H,    M_H,    '1',        '4',        'h',      M_H,    M_H}; // pass these on by default
//char color_message[STANDARD_MESSAGE_LENGTH] =      {M_C,    M_C,    '5',        '5',        'y',      M_C,    M_C}; // if message ID is > 50 - pass it on to all friends...
//char motion_message[STANDARD_MESSAGE_LENGTH] =     {M_M,    M_M,    '6',        '9',        't',      M_M,    M_M}; // These are not passed on by default
//char special_message[STANDARD_MESSAGE_LENGTH] =    {M_S,    M_S,    '6',        '6',        'g',      M_S,    M_S}; // to be defined
////                                                                                    f/r
////Receiving Cube                     //   bldcspeed "char[f/r]"  5+(value[1]) RPM .... Wait for Rising Edge... 
////Sending   Cube                     //   send Synchronise... (do bldcspeed self...)   Wait (time limit)               
//
//char color_message_short[STANDARD_MESSAGE_LENGTH]= {'G',    'G',    'G',        'G'};
//char temp_active_message[STANDARD_MESSAGE_LENGTH]= {M_C,    M_C,    '5',        '5',        'y',      M_C,    M_C};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////END OF OLD MESSAGE PASSING CODE/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//// int  look_for_signature_in_ambient(int face)
//// void fill_out_temp_message(char define_message_TYPE, int define_message_value, char define_message_char)
//// bool process_message(int face)
//// bool process_messages_all_faces()
//// void prepare_standard_messages()
//// bool read_message_information_and_align  (char message[])
//// 
//// void schedule_message_send()
//// bool fbtxled(int face, bool led1, bool led2, bool led3, bool led4)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// index                                            [0]     [1]     [2]      [3]    [4]    [5]     [6]    [7]
////                                                  type   type     type  [value] [value]  char    type   type
//// char location_message[STANDARD_MESSAGE_LENGTH] = {'l',  'l',    'l',      'l'    '1',    '9',    'a',  'l'}; // do not pass this message on...
//
//int look_for_signature_in_ambient(int face)
//{
//  
//}
//
//void fill_out_temp_message(char define_message_TYPE, int define_message_value, char define_message_char)
//{
//  char buf [3];
//  sprintf(buf, "%02i",define_message_value);
//  int digit1 = 0;
//  int digit2 = 0;
//  digit1 = buf [0];
//  digit2 = buf [1];
//  
//   temp_active_message[0] = define_message_TYPE;
//   temp_active_message[1] = define_message_TYPE;
//   temp_active_message[2] = char(digit1);
//   temp_active_message[3] = char(digit2);
//   temp_active_message[4] = define_message_char;
//   temp_active_message[5] = define_message_TYPE;
//   temp_active_message[6] = define_message_TYPE;
//
//}
//
//bool process_message(int face)
//{
//  int fb_message_size_temp =  fb_message_size(face);
//  if (fb_message_size_temp >= MESSAGE_THRESHOLD)
//     {
//      fb_receiveFromRxBuffer(face, fb_message_size_temp);  /// contents are now in temp_message[] - an array
//      if(fb_message_size_temp == MESSAGE_THRESHOLD && isupper(temp_message[1]))     {temp_message[5] = temp_message[1];} //  Pad message if it is too short...
//      if(fb_message_size_temp == (MESSAGE_THRESHOLD+1) && isupper(temp_message[1])) {temp_message[6] = temp_message[1];} //  Pad message if it is too short...
//      if(DEBUG){for(int i = 0; i < 15; i++){Serial.print(" "); Serial.print(temp_message[i]);}}
//      /// temp_message_type       - char
//      /// temp_message_id         - int            these get updated according to the below
//      /// temp_message_char      - char
//        if(read_message_information_and_align(temp_message))
//            {
//              if(DEBUG)
//              {
//                Serial.print("PROCESSED A MESSAGE!!!!: "); 
//                Serial.print("  type: ");Serial.print(temp_message_type);
//                Serial.print("  value: ");Serial.print(temp_message_value);
//                Serial.print("  char: ");Serial.print(temp_message_char);
//              }
//            
//                     if(temp_message_type == 'S')  {process_special_message (face, temp_message_value, temp_message_char);}
//                else if(temp_message_type == 'L')  {process_location_message(face, temp_message_value, temp_message_char);}         
//                else if(temp_message_type == 'H')  {process_hey_message     (face, temp_message_value, temp_message_char);}
//                else if(temp_message_type == 'C')  {process_color_message   (face, temp_message_value, temp_message_char);}
//                else if(temp_message_type == 'M')  {process_motion_message  (face, temp_message_value, temp_message_char);}
//            }
//        else // this is if the message type is not accaptable... do nothing
//          {
//            delay(1);
//            Serial.println(" I JUST FOUND A MESSAGE THAT DIDNT MEET CRITERIA =(");
//            for(int i = 0; i < MESSAGE_THRESHOLD; i++){Serial.print(" | ");Serial.print(temp_message[i]);}
//            Serial.print(" : is the Message text is: ");
//          }
//        
//      }    
//   else if(fb_message_size_temp > 0 && ir_face_buffer_previous[face] > 0) // this means buffer is still partially full, after one check...
//      {
//          Serial.println("checked partial message");
//          
//          fb_receiveFromRxBuffer(face, fb_message_size_temp);
////          for(int i = 0; i < fb_message_size_temp; i ++) 
////            {
////              temp_message_value = 53;
////                   if(temp_message[i] == 'R'){temp_message_char = 'r'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'G'){temp_message_char = 'g'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'B'){temp_message_char = 'b'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'Y'){temp_message_char = 'y'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'P'){temp_message_char = 'p'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'W'){temp_message_char = 'w'; process_color_message (face, temp_message_value, temp_message_char);}
////              else if(temp_message[i] == 'T'){temp_message_char = 't'; process_color_message (face, temp_message_value, temp_message_char);}
////            }
//          fbrxflush(face);
//          fb_message_size_temp = 0;
//      }
//   else { }  // this means that fb_message_size(face) is less than or equal to zero
//   ir_face_buffer_previous[face] = fb_message_size_temp;
//   erase_received_message();  
//}
//              
//bool process_messages_all_faces()
//{
//  for(int face = 1; face < ACTIVE_FACES; face++)
//  {
//    if(i_just_sent_an_ir_message[face] > 0) // this is designed to not check for messages immeaditly after sending them...
//    {
//      i_just_sent_an_ir_message[face]--; 
//      if(i_just_sent_an_ir_message[face] == 1) {fbrxflush(face);}
//    }
//    else {process_message(face);}
//  }
//}
////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////
//void prepare_standard_messages()
//{
// if(DEBUG_MSG){Serial.println("Beginning prepare_standard_messages");}
// edit_message_value(location_message, cube_id);
// edit_message_value(special_message,  cube_id);
//}
//
//// index                                                                  [0]     [1]     [2]      [3]    [4]    [5]      [6]      [7]     [8]   [9]
////                                                 type   type    type    type  [value] [value]  char    type   type
////char location_message[STANDARD_MESSAGE_LENGTH]   {M_L,  M_L,    M_L,    M_L,    '1',    '9',    'a',    M_L,   M_L,   M_L};
//
//bool read_message_information_and_align  (char message[])
//{
//  if(DEBUG){Serial.println("read_message_information_and_align");}
//  bool success = false;
//  int max_tries = 5;
//  int tries = 0;
//  while(tries < max_tries)
//  {
//      if(message[tries] == message[4+tries] && isupper(message[tries]) && isdigit(message[1+tries]) && isdigit(message[2+tries]) && islower(message[3+tries]))
//        {
//            success = true;
//            temp_message_value = ((message[1+tries] - '0')*10 + (message[2+tries] - '0'));
//            temp_message_char = message[3+tries];
//                 if(message[tries] == 'S')  {temp_message_type = 'S';}
//            else if(message[tries] == 'L')  {temp_message_type = 'L';}
//            else if(message[tries] == 'H')  {temp_message_type = 'H';}
//            else if(message[tries] == 'C')  {temp_message_type = 'C';}
//            else if(message[tries] == 'M')  {temp_message_type = 'M';}
//            else                              {temp_message_type = 'X'; success = false; Serial.println("read_message_type failed!");}
//            
//            break;
//        }
//      tries++; if(DEBUG){Serial.println("Incremented tries...");}
//  }
//   Serial.print("TRIES: "); Serial.println(tries); 
//   return success;
//}
//
////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////
//  // index                      [0]    [1]     [2]      [3]     [4]     [5]     [6]    [7]
////                              type   type    type    [id1]   [id2]    char    type   type
////char location_message[8] =   {'l',  'l',    'l',      '1',    '9',    'a',    'l',   'l'}; // do not pass this message on...
////char hey_message[8] =        {'h',  'h',    'h',      '1',    '4',    'h',    'h',   'l'}; // pass these on by default
//
//bool edit_message_char (char message[], char new_char)
//{  
//  bool success = true;
//  message[message_info_begin_index+char_position_offset] = new_char;
//  return success;
//}
//
//bool edit_message_value    (char message[], int new_value)
//{
//  char buf [3];
//  sprintf(buf, "%02i", new_value);
//  int digit1 = 0;
//  int digit2 = 0;
//  digit1 = buf [0];
//  digit2 = buf [1];
//  message[message_info_begin_index]   = char(digit1);
//  message[message_info_begin_index+1] = char(digit2);
//  bool success = true;
//  return success;
//}
//
//void erase_received_message()
//{
//  for(int i = 0; i < ARRAY_LENGTH_LONG; i++){temp_message[i] = '_';}
//}
//
//
//int bitArrayToInt32(bool arr[], int count_)
//{
//    int ret = 0;
//    int tmp;
//    for (int i = 0; i < count_; i++) {
//        tmp = arr[i];
//        ret |= tmp << (count_ - i - 1);
//    }
//    return ret;
//}
//
//// Pulse should look like this:
//// _|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_
//
////               [16] [15] [14] [13] [12] [11] [10] [9]  [8]  [7]  [6]  [5]  [4]  [3]   [2]  [1] 
////               *              *             *              *              *                        
////               _    _    A    A   A    _    _    _   b1    b2   b3   c1   c2    c2     _    [send message...]
//// led0 - 0  0   0    0    1    1   1    0    0    0    0    0    0    0    0     0      0
//// led1 - 0  1   0    0    1    1   1    0    0    0    0    0    0    1    1     1      0
//// led2 - 1  0   0    0    1    1   1    0    0    0    1    1    1    0    0     0      0
//// led3 - 1  1   0    0    1    1   1    0    0    0    1    1    1    1    1     1      0
//
//void general_counter(int counter_array[])
//
//{
//   for(int face = 1; face < ACTIVE_FACES; face++)
//  {
//    if(counter_array[face] < 1) {} // we are done... wait for this number to get bumped up again
//    else
//    {     
//           if(counter_array[face] == 5 )  {}
//      else if(counter_array[face] == 2 )  {}
//      counter_array[face]--;
//    }
//  }
//}
//
//void signature_blink()
//{
//   for(int face = 1; face < ACTIVE_FACES; face++)
//  {
//    if(signature_blink_array[face] < 1) {} // we are done... wait for this number to get bumped up again
//    else
//    {     
//           if(signature_blink_array[face] == 4 )  {IRLED(face,1,1,1,1);}
//      else if(signature_blink_array[face] == 1 )  {IRLED(face,0,0,0,0);}
//      signature_blink_array[face]--;
//    }
//  }
//}
//
//void location_message_special_blink()
//{
//  for(int face = 1; face < ACTIVE_FACES; face++)
//  {
//    if(message_location_blink_counter[face] < 1) {} // we are done... wait for this number to get bumped up again
//    else
//    {
//      if(message_location_blink_counter[face] == 1)
//            {
//              //location_message[index_of_face_number] = char(face_a); //location_message[index_of_face_number + 2] = char(face);
//              //send_ir_message_char(face, location_message_long, long_message_length);
//                char new_char_temp = 'a';
//                     if(face == 1){new_char_temp = 'a';}
//                else if(face == 2){new_char_temp = 'b';}
//                else if(face == 3){new_char_temp = 'c';}
//                else if(face == 4){new_char_temp = 'd';}
//                else if(face == 5){new_char_temp = 'e';}
//                else if(face == 6){new_char_temp = 'f';}                
//                edit_message_char(location_message, new_char_temp);
//                send_ir_message_char(face, location_message, STANDARD_MESSAGE_LENGTH);
//                if(DEBUG){Serial.println("I JUST SENT A LOCATION MESSAGE");}
//            }
//      else if(message_location_blink_counter[face] == 14)  {IRLED(face,1,1,1,1);}
//      else if(message_location_blink_counter[face] == 11)  {IRLED(face,0,0,0,0);}
//      else if(message_location_blink_counter[face] == 8 )  {IRLED(face,0,0,1,1);}
//      else if(message_location_blink_counter[face] == 5 )  {IRLED(face,0,1,0,1);}
//      else if(message_location_blink_counter[face] == 2 )  {IRLED(face,0,0,0,0);}
//      message_location_blink_counter[face]--;
//    }
//  }
//}
//
////color_message, MESSAGE_THRESHOLD);
//bool send_ir_message_on_faces(bool face1, bool face2, bool face3, bool face4, bool face5, bool face6, char message[])
//{
//  if(face1) {send_ir_message_char(1, message, MESSAGE_THRESHOLD);}
//  if(face2) {send_ir_message_char(2, message, MESSAGE_THRESHOLD);}
//  if(face3) {send_ir_message_char(3, message, MESSAGE_THRESHOLD);}
//  if(face4) {send_ir_message_char(4, message, MESSAGE_THRESHOLD);}
//  if(face5) {send_ir_message_char(5, message, MESSAGE_THRESHOLD);}
//  if(face6) {send_ir_message_char(6, message, MESSAGE_THRESHOLD);}
//}
//
//bool send_ir_message_char(int face, char message[], int message_length)
//{
//  IRLED(face,0,0,0,0);
//  delay(1);
//  fbtxled(face,1,1,1,1);
//  delay(1);
//  fbtx_send_msg_char(face, message_length, message);
//  i_just_sent_an_ir_message[face] = 3;
//  return(true);
//}
/////////// SENDING
//// fbtxcount <facenumber>
//// fbtx <facenumber> <message>
////**** fbtxled <faceNumber> [<irLEDNumber1>] [<irLEDNumber2>] [<irLEDNumber3>] [<irLEDNumber3>]
//
///////// RECEIVING
////**** fbrx <faceNumber> <byteCount>
////**** fbrxflush <faceNumber>
////**** fbrxcount <facenumber>
//
//bool fbtx_send_msg_char(int face, int numBytes, char message[]) 
//{
//    char twiBuf[30];
//    if (face > 6 || face < 1) {return false;}
//    twiBuf[0] = 0x30; //FB_REGISTER_ADDR_TX_BUF;
//    for(int i = 0; i < numBytes; i++){twiBuf[i+1] = message[i];}
//    
//    Wire.beginTransmission(face);
//    for(int i = 0; i < (1 + numBytes); i++)
//    {
//      Wire.write(byte(twiBuf[i]));
//    }
//    Wire.endTransmission();
//    return(true);
//}
//
//int fb_message_size(int face)//, uint8_t *bytes) 
//{
//    int number = 0;
//    if ((face < 1) || (face > 6)) {return -1;}        
//    Wire.beginTransmission(face);// 
//    Wire.write(byte(0x41)); //FB_REGISTER_ADDR_RX_CONSUMED_COUNT 
//    Wire.endTransmission();
//    
//    Wire.requestFrom(face, 1);
//    if (Wire.available() > 0) {number = Wire.read(); Wire.endTransmission();} // read the number stored in faceboard
//    if(number > 0){Serial.print("fb_message_size_buffer: ");Serial.print(number);Serial.print(" on face: ");Serial.println(face);}
//    return number;
//    
//}
//
//bool fb_receiveFromRxBuffer(int face, int numBytes)//, uint8_t *bytes) 
//{
//    if ((face < 1) || (face > 6)) {return -1;}        
//    Wire.beginTransmission(face);// 
//    Wire.write(byte(0x40)); // int i2c1 = 0x40; //FB_REGISTER_ADDR_RX_BUF  0x40 
//    Wire.endTransmission();
//    
//    Wire.requestFrom(face, numBytes);
//    if (Wire.available() >= numBytes) //ambientLight  = twiBuf[0] << 2;
//    {
//      for(int i = 0; i < numBytes; i++)
//      {
//      temp_message[i] = Wire.read(); 
//      }    
//      if(DEBUG){for(int i = 0; i < numBytes; i++) {Serial.print(temp_message[i]);} Serial.println(" ");}
//    }
//    return true;
//}
//
//
//bool fbrxflush(int face)
//{
//  if(DEBUG){Serial.print("Flushing buffer on face: ");Serial.println(face);}
//  if (face < 1 || face > 6){return(false);}
//  digitalWrite(Switch, HIGH);
//  int i2c1 = 0x42; // FB_REGISTER_ADDR_RX_FLUSH  
//  int i2c2 = 0x01;
//  Wire.beginTransmission(face);
//  Wire.write(byte(i2c1));
//  Wire.write(byte(i2c2));
//  Wire.endTransmission();
//  return(true);
//}
//
//bool fbtxled(int face, bool led1, bool led2, bool led3, bool led4)
//{
//  if (face < 1){return(false);}  
//  int i2c1 = 0x33; // FB_REGISTER_ADDR_TX_LED_SELECT
//  int i2c2 = 0x00;
//  if (face > 6) {return(false);}
//  i2c2 = led1 ? 0x01 : 0x00;
//  i2c2 |= led2 ? 0x02 : 0x00;
//  i2c2 |= led3 ? 0x04 : 0x00;
//  i2c2 |= led4 ? 0x08 : 0x00;
//  Wire.beginTransmission(face);
//  Wire.write(byte(i2c1));
//  Wire.write(byte(i2c2));
//  Wire.endTransmission();
//  return(true);
//}
//
//
//void schedule_message_send()
//{
//  for(int face = 1; face < ACTIVE_FACES; face++)
//  {
//    if(message_general_counter[face] > 0) 
//    {
//      send_ir_message_char(face, temp_active_message, MESSAGE_THRESHOLD);
//      message_general_counter[face]--;
//    } // we are done... wait for this number to get bumped up again
//    else{}// we are done... wait for this number to get bumped up again
//  }
//}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////END OF OLD MESSAGE PASSING CODE/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//bool fb_getIRTxLEDs(int face, bool led1, bool led2, bool led3, bool led4) 
//{
//    if ((face < 1) || (face > 6)) {return -1;}
//    uint8_t twiBuf[2];
//    bool success = true;
//    twiBuf[0] = 0x33; // FB_REGISTER_ADDR_TX_LED_SELECT   
//    Wire.beginTransmission(face);// 
//    Wire.write(byte(0x40)); // int i2c1 = 0x40; //FB_REGISTER_ADDR_RX_BUF            0x40 
//    Wire.endTransmission();
//    
//    Wire.requestFrom(face, 1);
//    if (numBytes <= Wire.available()) //ambientLight  = twiBuf[0] << 2;
//    {
//      for(int i = 0; i < numBytes; i++)
//      {
//      temp_message[i] = Wire.read(); 
//      }    
//      //for(int i = 0; i < numBytes; i++) { Serial.print(temp_message[i]); }
//    }
//    return true;
//
//    success &= twi_master_transfer((faceNum << 1), twiBuf, 1, true);
//    success &= twi_master_transfer((faceNum << 1) | TWI_READ_BIT, twiBuf, 1, true);
//
//    twi_master_deinit();
//
//    if (success) {
//  led1 = (twiBuf[0] & 0x01) ? true : false;
//  led2 = (twiBuf[0] & 0x02) ? true : false;
//  led3 = (twiBuf[0] & 0x04) ? true : false;
//  led4 = (twiBuf[0] & 0x08) ? true : false;
//    }
//
//    return success;
//}





///////////////////////
//bool send_ir_message_string(int face, String message)
//{
//  long begin_time = millis();
//  IRLED(face,0,0,0,0);
//  delay(1);
//  fbtxled(face,1,1,1,1);
//  delay(1);
//  fbtx_send_msg_string(face, message.length(), message);
//  i_just_sent_an_ir_message[face] = 3;
//  return(true);
//}

//bool fbtx_send_msg_string(int face, int numBytes, String message)
//{
//    digitalWrite(Switch, HIGH);
//    char twiBuf[30];
//    if (face > 6 || face < 1) {return false;}
//    twiBuf[0] = 0x30; //FB_REGISTER_ADDR_TX_BUF;
//    twiBuf[1] = twiBuf[2] = twiBuf[3] = 'l';    
//    for(int i = 0; i < numBytes; i++){twiBuf[i+4] = message.charAt(i);}
//    
//    Wire.beginTransmission(face);
//    for(int i = 0; i < (4 + numBytes); i++)
//    {
//     Wire.write(byte(twiBuf[i]));
//    }
//    Wire.endTransmission();
//    return(true);
//}


//#include <Adafruit_MPL3115A2.h>\
//  baro.begin(); // Eveuntually get average from BARO - long initial_baro;

//long initial_baro; // this will be the calibrated ofset for the barometer

  /////////////////////////// Populate 5 bit ID //////////////////////////
//  int j = 0;
//  for(int i = 4; i >= 0 ; i--)  
//  {five_bit_id_array[i] = bitRead(five_bit_id_int, j); j++;}
  ////////////////////////////////////////////////////////////////////////


  //
//bool read_message_value    (char message[])
//{
//  if(DEBUG){Serial.println("read_message_value");}
//  bool success = true;
//  if(isdigit(message[message_value_index_1]) && isdigit(message[message_value_index_1+1])) // Check if both are digits 0-9
//    {temp_message_value = ((message[message_value_index_1] - '0')*10 + (message[message_value_index_1+1] - '0'));} // converts this into a digit
//  else{success = false; Serial.println("read_message_value failed!");}
//  return success;
//}

//bool read_message_char (char message[])
//{
//  if(DEBUG){Serial.println("read_message_char");}
//  bool success = true;
//  if(isalpha(message[message_char_index]))    {temp_message_char = message[message_char_index];}
//  else                                        {success = false;Serial.println("read_message_char failed!");}
//  return success;
//}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////THINGS FROM GHETTO LIGHT DECODING////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
//void read_on_face_testing_only(int face)
//{
//  while(read_ambient(face) < 8)
//  {delay(1);}
//  while(read_ambient(face) > 8)
//  {delay(1);}
//  Serial.println("MADE IT PAST PEAK 1");
//  while(read_ambient(face) < 8)
//  {delay(1);}
//  while(read_ambient(face) > 8)
//  {delay(1);}
//  Serial.println("MADE IT PAST PEAK 2");
//  while(read_ambient(face) < 8)
//  {delay(1);}
//  Serial.println("WAITING FOR FINAL PEAK TO END");
//  while(read_ambient(face) > 8)
//  {delay(1);}
//  sample_10_ms(face);
//  delay(500);
//}

//int convert_int_to_binary_array(int number, int array_length)
//{
//  int j = 0;
//  for(int i = array_length-1; i >= 0 ; i--)
//  {
//  blink_pattern[i] = bitRead(number, j); j++;
//  }
//}
//
//int                       samples[90];
//int  fast_ambient_sampling_time__[90];
//int  fast_ambient_sampling_count_[90];
//int  light_high_threshold             = 28;
//int  is_message_good                  = 1;
//int  filtered_message_13_bit[]        = {0,0,0,0,0,0,0,0,0,0,0,0,0};
//bool filtered_message_13_bit_bool[]   = {0,0,0,0,0,0,0,0,0,0,0,0,0};
//int  incomingByte                     = 0;
//int  blink_pattern[9];

//                                       --beginning------  | 0/1  ID0   ID1   ID2   ID3   ID4    S1    S2   | F1    F2    F3   LED1   LED2   clear
//int message_to_blink[22] = {0,  1,  0,  1,  0,  1,  1,  0,     0,   0,    0,    0,    0,    0,     1,    0,     0,    0,    0,    2,     3,   0};
//                         [0] [1] [2] [3] [4] [5] [6] [7]  | [8]  [9]   [10]  [11]  [12]  [13]   [14]  [15] | [16]  [17]  [18]  [19]   [20] [21]
// Received Message                                         | [0]  [1]   [2]   [3]   [4]   [5]    [6]   [7]  | [8]   [9]   [10]  [11]   [12]
//                                                            ||||||||||||||||||||||||||||||||||||||||||     
//    Cube ID                                     ----------------------------      
//    
// Sampling 8ms     | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | 
// Message type:
//                  High      Low      bit_0          bit_1      bit_2      bit_3      bit_4      bit_5      bit_6      bit_7     
// Sending Message [100 ms] [48 ms]    [48 ms]        [48 ms]    [48 ms]    [48 ms]    [48 ms]    [48 ms]    [48 ms]    [48 ms]    
//                                    Message Type    
//                                    1 = location    CubeID_1   CubeID_2   CubeID_3   CubeID_4   CubeID_5   Parity 1   Parity 2    
//                                    0 = command     

//
//void send_location_message_test(int face)
//{
//  message_to_blink[8] = 1;
//  bool face_number_binary[3] = {0, 0, 0}; //// I KNOW THE FOLLOWING IS GHETTO AS F***, Apologies to any real programmers who happen to read this
//  //                           [0][1][2]
//  if     (face == 1) {face_number_binary[2] = 1;}                           // [001]
//  else if(face == 2) {face_number_binary[1] = 1;}                           // [010]
//  else if(face == 3) {face_number_binary[1] = 1;face_number_binary[2] = 1;} // [011]
//  else if(face == 4) {face_number_binary[0] = 1;}                           // [100]
//  else if(face == 5) {face_number_binary[0] = 1;face_number_binary[2] = 1;} // [101]
//  else if(face == 6) {face_number_binary[0] = 1;face_number_binary[1] = 1;} // [110]
//  for(int i = 9;  i < 14; i++) {message_to_blink[i] = five_bit_id_array[i-9];}
//  for(int i = 16; i < 19; i++) {message_to_blink[i] = face_number_binary[i-16];}
//  //void blink_array(int bit_array[], int array_length, int face, int blink_period)
//  blink_array(message_to_blink, 22, face, 50);
//  Serial.println("stillalive");
//  delay(500);
//  //Serial.println("stillalive");
//}



//void sample_10_ms(int face)
//{
//  int num_samples       = 13;
//  int begin_time        = millis();
//  int  sampling_period  = 9;
//  int  sample_counter   = 0;
//  long sample_timer     = begin_time + sampling_period;
//  
//  int received_data_bit = 0;
//  int received_id       = 0;
//  int received_face     = 0;
//  int received_command  = 0;
//  
//  while(sample_counter < (num_samples*5+15))
//  { 
//  if(millis() > sample_timer)
//     {
//       sample_timer = millis() + sampling_period;
//       fast_ambient_sampling_time__[sample_counter] = millis()-begin_time;
//       sample_counter++; // Increment a counter       
//       samples[sample_counter] =  read_ambient(face);
//       fast_ambient_sampling_count_[sample_counter] = sample_counter;
//     }  
//  }
//for(int i = 0; i < num_samples; i++)
//{
//filtered_message_13_bit[i] = samples[((i*5)+4)] + samples[((i*5)+5)] + samples[((i*5)+6)] + samples[((i*5)+7)] + samples[((i*5)+8)];
//}
//
//for(int i = 0; i < num_samples; i++)
//{
//if      (filtered_message_13_bit[i] > light_high_threshold*3) {filtered_message_13_bit_bool[i] = 1;}
//else if (filtered_message_13_bit[i] < light_high_threshold*2) {filtered_message_13_bit_bool[i] = 0;}
//else                                                          {is_message_good = -1;}
//}
//for(int i = 0; i < (num_samples*5+15); i++)
//{
//  Serial.print(samples[i]);
//  Serial.print(" Clock = ");
//  Serial.print(fast_ambient_sampling_time__[i]);
//  Serial.print(" Count = ");
//  Serial.println(fast_ambient_sampling_count_[i]);
//  if((i+2)%5 == 0){Serial.print("-----------"); Serial.println((i+2)%5);}
//}
//Serial.println("THIS IS THE ACCUMULATED MASSAGE!!!");
//for(int i = 0; i < num_samples; i++)
//{
//   Serial.println(filtered_message_13_bit[i]);
//}
//Serial.println("ENDING - ACCUMULATED MASSAGE!!!");
//
//if(filtered_message_13_bit_bool[0] == 1) {Serial.println("LOOKING FOR NEIGHBORS");}
//if(filtered_message_13_bit_bool[6] == 1) {Serial.println("WOoooOOOO!!!!!");}
//for(int i = 0; i < 5; i++) {five_bit_id_array_received[i] = filtered_message_13_bit_bool[i+1];}
//five_bit_id_int_received         = bitArrayToInt32(five_bit_id_array_received, 5);
//
//for(int i = 0; i < 3; i++) {received_face_number_bits[i] = filtered_message_13_bit_bool[i+8];}
//received_face_number             = bitArrayToInt32(received_face_number_bits, 3);
//
//for(int i = 0; i < 2; i++) {received_LED_number_bits[i] = filtered_message_13_bit_bool[i+11];}
//received_LED_number              = bitArrayToInt32(received_LED_number_bits, 2);
//
//Serial.print("And the NEIGHBORS ID IS.... ");
//Serial.println(five_bit_id_int_received);
//Serial.print("And the NEIGHBORS'S FACE NUMBER .... ");
//Serial.println(received_face_number);
//Serial.print("And the NEIGHBORS LED NUMBER IS .... ");
//Serial.println(received_LED_number);
//}



//void blink_array(int bit_array[], int array_length, int face, int blink_period)
//{
//  for(int i = 0; i < array_length; i++)
//  {
//   if      (bit_array[i] == 1) {IRLED(face,1,1,1,1);}
//   else if (bit_array[i] == 2) {IRLED(face,0,0,1,1);}
//   else if (bit_array[i] == 3) {IRLED(face,0,1,0,1);}
//   else             {IRLED(face,0,0,0,0);}
//   delay(blink_period);
//  }
//  IRLED(face,0,0,0,0);
//}


//void accept_data_from_serial_testing()
//{
//if (Serial.available() > 0) 
//    {
//                // read the incoming byte:
//                incomingByte = Serial.parseInt();
//
//                // say what you got:
//                Serial.print("I received: ");
//                Serial.println(incomingByte);
//                
//                convert_int_to_binary_array(incomingByte,9);
//                for(int i = 6; i < 15; i++) {message_to_blink[i] = blink_pattern[i-6];}
//                //blink_boolean_array(bool bit_array[]     ,int array_length, int face, int blink_period);
//                blink_array  (message_to_blink,15              , 3       , 50              );
//      }
//}

//void stupid_test(int face)
//{
//  char testingbs[4];
//  for(int i = 0; i < 3; i++)
//  {
//  testingbs[i] = bitRead(face, i);
//  Serial.print(bitRead(face, i));
//  }
//  Serial.print("is the bit for face: ");Serial.println( face);
//}
// for(int i = 1; i < 7; i++){stupid_test(i);}
// 
//int  five_bit_id_array[5]             = {0,0,0,0,0};
//int  five_bit_id_int                  = 0;
//
//bool five_bit_id_array_received[5]    = {0,0,0,0,0};
//int  five_bit_id_int_received         = 0;
//
//bool received_face_number_bits[3]    = {0,0,0};
//int  received_face_number             = 0;
//
//bool received_LED_number_bits[2]    = {0,0};
//int  received_LED_number            = 0;

//   simple_loop_counter++;
//   write_ID(byte(0x15), byte(simple_loop_counter));
//   Serial.println("---------------------------------------");
//   Serial.print(read_register(1,byte(0x12))); Serial.println("  data in 0x12");
//   Serial.print(read_register(1,byte(0x13))); Serial.println("  data in 0x13");
//   Serial.print(read_register(1,byte(0x14))); Serial.println("  data in 0x14");
//   Serial.print(read_register(1,byte(0x15))); Serial.println("  data in 0x15");
//   Serial.print(read_register(1,byte(0x16))); Serial.println("  data in 0x16");
//   Serial.print(read_register(1,byte(0x17))); Serial.println("  data in 0x17");
//   Serial.print(read_register(1,byte(0x18))); Serial.println("  data in 0x18");
//   Serial.print(read_register(1,byte(0x19))); Serial.println("  data in 0x19");
//   Serial.println("---------------------------------------");
//   Serial.print(read_register(1,byte(0x09))); Serial.println("  ASSEMBLED DATA");
//   Serial.println("---------------------------------------");
//   Serial.print(read_ambient_new(1));Serial.println(" IS THE AMBIENT READING OLD");
//   //Serial.println("stillalive");
   //for(int i = 1; i < 7; i++){send_location_message_test(i);}
   //Serial.print(read_ambient_new(1));Serial.println(" IS THE AMBIENT READING ");
   //Serial.print(read_ambient(1));Serial.println(" IS THE AMBIENT READING OLD");
   //Serial.print(read_register(1));Serial.println(" IS THE REGISTER");
   //accept_data_from_serial_testing();
   //read_on_face_testing_only(1);
   //   which_demo = "fast_blink";
//Serial.println(five_bit_id_int);
//for(int i = 0; i < 5; i++){Serial.print(five_bit_id_array[i]);}
//delay(500);

//bool write_ID(byte register_address, byte databyte)//(int face)
//{
//
//  if(DEBUG){Serial.println("Beginning FBRXEN");}
//  digitalWrite(Switch, HIGH); // Takecontrol of I2C BUS
//  delay(3);
//  //for (int x = 1);//; x< 7 ; x++) {
//  Wire.beginTransmission(1);
//  Wire.write(register_address);
//  Wire.write(databyte);
//  Wire.endTransmission(); 
//
//}

//unsigned int read_register(int face, byte i2c_register)//(int face)
//{
//  unsigned int reading = 0;
//  digitalWrite(Switch, HIGH);
//  if(face > 6 || face < 1) {return(-1);}
//  Wire.beginTransmission(face);
//  Wire.write(i2c_register); // this is the register
//  Wire.endTransmission();
//  Wire.requestFrom(face, 1);
//  if (Wire.available()) //ambientLight  = twiBuf[0] << 2;
//  {
//    reading =  Wire.read();     //  ambientLight |= twiBuf[1] >> 6;
//    return reading;
//  }
//  else{return(-1);}
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//bool fbtx_send(int face, bool flash) {
//  if (face < 1 || face > 6){return(false);}
//  digitalWrite(Switch, HIGH);
//  byte i2c1 = 0x34; // FB_REGISTER_ADDR_TX_MSG_CONTROL
//  byte i2c2 = flash ? 0x02 : 0x00;
//  i2c2 |= 0x01;
//  Wire.beginTransmission(face);
//  Wire.write(byte(i2c1));
//  Wire.write(byte(i2c2));
//  Wire.endTransmission();
//  return true;
//}

//bool fbtx_que_message(int face, int numBytes, char message[]) {
//    digitalWrite(Switch, HIGH);
//    char twiBuf[128];
//    if (face > 6 || face < 1) {return false;}
//    twiBuf[0]  = 0x35; //FB_REGISTER_ADDR_TX_MSG_BUF         0x35
//    twiBuf[1] = twiBuf[2] = twiBuf[3] = 0xB7; // pad message as there is a tendency to drop the first few characters
//    for(int i = 0; i < numBytes; i++){twiBuf[i+4] = message[i];}
//    Wire.beginTransmission(face);
//    for(int i = 0; i < (4 + numBytes); i++)
//    {
//     Wire.write(byte(twiBuf[i]));
//    }
//    Wire.endTransmission();
//    return(true);
//}


//<datatype> array [DIM_0_SIZE] [DIM_1_SIZE] = {
//  //as many vals as dim1
// {val,val,val},
// {val,val,val}//as many rows as dim0
//};

//Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();



/*
 * 
 * 
 * 1. Setup 
 *    a. Wifi Setup
 *    b. Ros Setup
 *    c. Sensors Setup
 * 
 * 2. Main Loop State Machine ... Does Ros things...
 *    States:
 *    1. Chilling - Publishes regular updates of sensors values// accepts short commands
 *          Listens to General ROS topic, accepts commands based on general or on cube specific topic
 *    
 *    2. Driving a car*
 *    
 *    3. Aggregate - Does not regularly update sensors... Internally evaluates sensor readings to move toward light
 *    
 *       Begin... Get 6 light readings... Check Current orientation (1234, 1536, 2546)
 *       Determine if free to move with fn::Wiggle (20% of the time do this 2x or stronger)
 *       Find Face with Highest value, above a threshold. 
 *            Determine if flywheel is currently aligned with this.
 *                If so - slowly roll fw or bkwards.
 *                if not - random num generator ? Wiggle : Jump
 *                
 *    4. Climb?
 *    5. Sleep?
 *    6. Reset?
 *    7. Demo? - hard programmed demo routine
 * 
 * 
 * 
 * 
 * 3. Short Commands
 *    1. Turn on/off IR LED 1....24
 *    2. Turn on RGB led 1-8 to R or G or B
 *    3. All RGB specific color
 *    4. Any IA command
 *    
 *    
 * 4. Longer Commands
 * 
 *    3. Wiggle_test
 *        Check gravity.
 *        Begin recording array of GYRO values
 *        Do a short, sharp Move
 *        IF (CP_state) is alligned with Ground
 *        
 *        ELSE
 *    
 *    
 *    
 *    4. IA_MOVE
 *        Updates internal 6 light readings, (1234, 1536, 2546) and 3x Gravity Readings.
 *        Determine expected sucessfull gravity readings from a Lookup Table.
 *        Attempt to move using (standard values*Specific Cube Calibration number).
 *            Checks to see if successful, unsucessfull, or WTF!! (look at gravity, maybe array of gyro readings)
 *            If successful - send sucess message to (ROS + Flash LED green)
 *            (If unsucessfull and fail counter <3) - Wait 1000ms, then try IA again with 1.(fail_Counter)*Current value 1.(fail_Counter)*Speed value
 *                  Increment fail_Counter.
 *            if WTF - check wiggle.
 *                 If attached, send message.
 *                 If Free - send message
 *                 <Break>
 *            If fail counter >3
 *                Turn LED's Red
 *                Send Message
 *                <break>
 * 
 *     5. CP(1234 (a) or 1536 (b), or 2546 (c))
 *         Does Wiggle Test (Connected or not?)
 *         if (Connected)
 *            Check Magnet readings...
 *            SMA retract(3000)
 *            Begin recording array of Magnet Values... Also Acceleromater... and Gyro Values
 *                IF (1234 to 1536...) Possible Transistions: (a)->(b), (b)->(c), (c)->(a), (b)->(a), (c)->(b), (a)->(c)
 *                      While (Not disloged) -- Accerate (or ebrake...) eneough to disloge with increasing power
 *                 Check magnet state... IF correct... way to go! 
 *                 <break>
 *                 
 *                 If not correct, either in (a), (b), (c)
 *                 If not check array readings to try to determine if in region between (a-b) (b-c) or (c-a)
 *                  
 *                 Choose reasonable value for FLYWHEEL
 *                 Loop back, and try again, increase SMA retract time
 *                 
 *         if (Not Connected)
 *                Same as above, but with lower power values.
 * 
 * 
 */



 // Notes
//const char WiFiSSID[] = "TP-LINK_9B88A0"; // Sebastian's router
//const char WiFiPSK[] = "279B88A0";// Sebastian's router

// Format for sensor values Message
// U_int16 array
////////////////////////////////////
//
//
// Cube ID 0xVVV,                             State[0]
// Face1_light,                               State[1]
// Face2_light,                               State[2]
// Face3_light,                               State[3] 
// Face4_light,                               State[4]
// Face5_light,                               State[5] 
// Face6_light,                               State[6]
// Magnet sensor Reading                      State[7]                    (1234, 1536, 2546 or 0000) with 0000 meaning undetermined or error
// Grav_face1_x,                              State[8]
// Grav_face1_y,                              State[9]
// Grav_face1_z,                              State[10]
// Gyro_face1_x,                              State[11]
// Gyro_face1_y,                              State[12]
// Gyro_face1_z,                              State[13]
// TEMP_ACCEL   ,                             State[14] 
// ALS_parasite,                              State[15]        
// Altitude_parasite,                         State[16]                     Value in mm, zeroed on initial setup
//
//
//    while (Serial.available() > 0) // while there are things in the serial buffer...
//  {
//      char c = Serial.read();
//      if (c != '\n') 
//    {
//        ser_str += String(c);
//    } else 
//    {
//        serial_msg.data = ser_str.c_str();
//        pub_ser.publish(&serial_msg);
//        ser_str = "";
//    }
//  }


//void special_blink(int face) // each time this is called, it will perform the next action.
//// If the function is called every 50ms it will take 1 second to complete
////   else if(blink_counter == 13){for(int i = 1; i < 7; i++){IRLED(face,0,0,0,1);}
//// Pulse should look like this:
//// _|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_
//// A A A A A - - - 1 1 1 2 2 2 3 3 3 4 4 4
//{
//  if(blink_counter == 22){blink_counter = 0;} // Starts sequential counter 0 causes it to not be stop running...
//  else if(blink_counter == 0){return;} // this means that the counter has already run once...
//  else if(blink_counter == 1){IRLED(face,1,1,1,1);}
//  else if(blink_counter == 2){delay(1);}
//  else if(blink_counter == 3){delay(1);}
//  else if(blink_counter == 4){delay(1);}
//  else if(blink_counter == 5){delay(1);}
//  else if(blink_counter == 6){IRLED_OFF();}
//  else if(blink_counter == 7){delay(1);} // Delay 1 time step
//  else if(blink_counter == 8){delay(1);} // Delay 1 time step
//  else if(blink_counter == 9){IRLED(face,1,0,0,0);} // Turn on IRLED # 1 for 2 time steps
//  else if(blink_counter == 10){delay(1);} // Delay 1 time step
//  else if(blink_counter == 11){delay(1);} // Delay 1 time step
//  else if(blink_counter == 12){IRLED(face,0,1,0,0);} // Turn on IRLED # 2 for 2 time steps
//  else if(blink_counter == 13){delay(1);} // Delay 1 time step
//  else if(blink_counter == 14){delay(1);} // Delay 1 time step
//  else if(blink_counter == 15){IRLED(face,0,0,1,0);} // Turn on IRLED # 3 for 2 time steps
//  else if(blink_counter == 16){delay(1);} // Delay 1 time step
//  else if(blink_counter == 17){delay(1);} // Delay 1 time step
//  else if(blink_counter == 18){IRLED(face,0,0,0,1);} // Turn on IRLED # 4 for 2 time steps
//  else if(blink_counter == 19){delay(1);} // Delay 1 time step
//  else if(blink_counter == 20){delay(1);} // Delay 1 time step
//  else if(blink_counter == 21){IRLED_OFF();}
//  blink_counter++;
//}
//
//void find_connections()
//{
//  // Step 0: Update Zero faces... turn off all IR LED's and regular LED's... 
//  // long funny_bs = millis();
//    // ... Function...
//    // ...............
//    // Serial.print("This functiontook: ");
//    // Serial.println(millis() - funny_bs);
//  update_zero_faces();
//  //for(int i = 1; i <7; i++) {if(zero_faces[i] == 1){Serial.print("ZERO FACE: "); Serial.println(zero_faces[i]);}}
//  rgboff();
//  IRLED_OFF();
//  //for(int i = 1; i <7; i++){Serial.print("Face " + String(i) + " status: "); Serial.println(connected_faces[i]);}
//  for(int face = 0; face < 7; face++)
//  { 
//    // Step 1: Start for loop over faces to TRANSMIT... Update ROS things...
//    if(zero_faces[face] == 1)
//      {
//        if(is_ros_on){nh.spinOnce(); publishAwake();}
//        delay(random(10,80));
//        int ir_val[7][7] = {{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}}; // Face[1][prev.1.prev.2.3.4.5
//        // Step 2: Start 60ms Loop:
//        for(int i = 0; i < 13; i++)
//          {
//   // Blink correct part of message...
//            if(i%2 == 1){face_rgb(face, 1, 0, 1, 0); IRLED(face,1,1,1,1);} else {rgboff(); IRLED_OFF();} // Blinks IR LED's on "face" face
//   // Update ir_values_(faces on zero list)
//            delay(5);
//            for(int j = 1; j < 7; j++)
//            {
//            int readinn = read_ambient(j);
//            //if(zero_faces[j] == 1){face_rgb(j, 1, 0, 0, 0);
//            if(readinn > 1){{ir_val[j][0] = 1;} if(j == face && i%2 == 1){ir_val[j][0] = -1;}}
//            else if(readinn <= 1) {ir_val[j][0] = 0;}
//            else {ir_val[j][0] = -1;}
//            
//            } // Updates all faces item 0
//   // For all faces... Check to see if we see THE pattern... 
//            
//            for(int i = 1; i < 7; i++)
//            {if(ir_val[i][0] == 0 && ir_val[i][1] == 1 && ir_val[i][2] == 0 && ir_val[i][3] == 1 && ir_val[i][4] == 0 && ir_val[i][5] == 1)
//              {connected_faces[i] += 1;}}
//   // For faces on connection_list ... also check to see if they are all high... if so remove from list...
//             for(int i = 1; i < 7; i++)
//            {if(ir_val[i][0] == 1 && ir_val[i][1] == 1 && ir_val[i][2] == 1 && ir_val[i][3] == 1 && ir_val[i][4] == 1 && ir_val[i][5] == 1)
//              {connected_faces[i] = 0;}}
//   // Shift all values one to the left...
//            for(int i = 1; i < 7; i++)
//            {
//              ir_val[i][5] = ir_val[i][4];
//              ir_val[i][4] = ir_val[i][3];
//              ir_val[i][3] = ir_val[i][2];
//              ir_val[i][2] = ir_val[i][1];
//              ir_val[i][1] = ir_val[i][0];
//            }
//            delay(50);
//         }
//      }
//   }
//}
//
//void display_connected()
//{
//  for(int i = 1; i<7; i++)
//  {if(connected_faces[i] >= 1){face_rgb(i,0,1,0,1); delay(150); rgboff();}}
//}
//void update_zero_faces()
//{
//  int previous_values[7] = {0,0,0,0,0,0,0};
//  for(int i = 1; i <7; i++)
//  {
//  int light_value_ = read_ambient(i);
//  if(light_value_ < 1 && light_value_ != -1){zero_faces[i] = 1; previous_values[i] = light_value_;}
//  else{zero_faces[i] = 0;}
//  if(opposite_face(which_face_is_up(12000)) == i)
//  {zero_faces[i] = 0;}
//  }
//}

 // each time this is called, it will perform the next action.
// If the function is called every 50ms it will take 1 second to complete
//   else if(blink_counter == 13){for(int i = 1; i < 7; i++){IRLED(face,0,0,0,1);}
//
////if(ambient_values[face][0] == 0 && ambient_values[face][3] == 1 && ambient_values[face][4] == 1 && ambient_values[face][5] == 1 && ambient_values[face][8] == 0)
//        {
//          //Serial.print("FOUND A PULSE! on face number: ");Serial.println(face);
//          message_since_seeing_pulse[face]    = _50ms_loop_counter;
//          am_i_waiting_for_a_high_value[face] = 1;
//          //loop_counter
////          while(read_ambient(face) < high_ambient_threshold && (millis() - counter_begin) < 700)
////            {
////              delay(10);
////            }
//          //face_neighbors_angles[face] = millis() - counter_begin;
//          //message_blink_counter[face] = 1; // Send a message back as soon as possible
//          //connection_wait_time = millis() - counter_begin;
////          Serial.print("Time before second pulse = ");
////          Serial.println(connection_wait_time);
//        }





