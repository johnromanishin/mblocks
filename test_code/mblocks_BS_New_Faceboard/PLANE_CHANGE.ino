//bool go_to_plane(int desired_plane)
//{
////STEP 0: Initialize variables, check if we are already in correct plane...
//    int lattice_connected = 0;
//    int up_face = which_face_is_up(12000);
//    face_that_was_last_up = up_face;
//    int current_plane = which_plane_fast();
//    if(current_plane == desired_plane){return(true);}
//    
////STEP 1: Check to see if we are attached to a lattice...
//    // Perform Wiggle Test --- IF NOT RETURN: also look if face_that_is_up changed... RETURN IF SO
//    if(current_plane > 1000)
//        {
//        if(wiggle_test("f") < 500){lattice_connected = 1;}
//        }
//    if(up_face == which_face_is_up(12000)){lattice_connected = 1;}
//    else{lattice_connected = 0;return(false);}
//    
////Step 2:
//    long start_time = millis();
//    Serial.println("sma retract 7000"); // Begin by retracting the pin, allowing the central part to spin
//    delay(400); // wait to let it take effect
//    int milliamps_updateable = 4000; // these are the default values for the burst of acceleration
//    int milliseconds_updateable = 100; // time default
//    milliseconds_updateable += plane_change_offset;
//    milliamps_updateable += 2*plane_change_offset;
//    int loop_count = 0;
//    int loop_count_1 = 0;
//    while(millis()-start_time < 7000 && current_plane != desired_plane) // while we aren't in correct plane, and we are still under 6 seconds...
//    {
//     Serial.println("stillalive");
//     sum_of_diff = 0;
//     delay(50);
//     String command_forward = "bldcaccel f " + String(2300+loop_count_1*15) + " " + String(110 + loop_count_1 + plane_change_offset/3);
//     String command_reverse = "bldcaccel r " + String(2300+loop_count_1*15) + " " + String(110 + loop_count_1 + plane_change_offset/3);
//     if(current_plane > 1000)
//     {
//     accel_jump("f", milliamps_updateable, milliseconds_updateable); // try to move plane 
//     delay(400);if(is_ros_on){nh.spinOnce();}
//     loop_count += 25; // This counter counts how many times we have tried accel_jump
//     }
//     else{
//     delay(100);
//     current_plane = which_plane_fast();
//     if((current_plane == 52 && desired_plane == 1234) || (current_plane == 21 && desired_plane == 1536) || (current_plane == 15 && desired_plane == 2546)) {Serial.println(command_forward); delay(400);if(is_ros_on){nh.spinOnce();} loop_count_1 += 10;}
//     else if( (current_plane == 52 && desired_plane == 1536) || (current_plane == 21 && desired_plane == 2546) || (current_plane == 15 && desired_plane == 1234)) {Serial.println(command_reverse); delay(400);if(is_ros_on){nh.spinOnce();} loop_count_1 += 10;}
//     else if(current_plane == desired_plane){break;}
//     else if(current_plane > 1000) {r_counter = 4;Serial.println("bldcaccel f 4000 140");delay(600);if(is_ros_on){nh.spinOnce();}}
//     else{Serial.println("bldcaccel r 4000 200"); delay(200); Serial.println("bldcstop b");}
//     }
//     if(sum_of_diff < 10000){milliamps_updateable += 300; milliseconds_updateable += 40;}
//     milliseconds_updateable += loop_count;
//     current_plane = which_plane_fast();
//     delay(20);
//    }
//    // This section of code runs when the timer is up... or if there is a problem with the plane
//    current_plane = which_plane_fast(); //which_plane_cautious();
//    delay(50);
//    if(current_plane != desired_plane && current_plane > 1000) // if the central part is not alligned with any axis.
//    {
//     Serial.println("stillalive");
//     delay(500);Serial.println("sma retract 2000");
//     delay(50);Serial.println("bldcaccel r " + String(2500+plane_change_offset*15) + " " + String(110 + plane_change_offset/3));
//     delay(200);Serial.println("bldcaccel f " + String(2600+plane_change_offset*15) + " " + String(110 + plane_change_offset/3));
//     Serial.println("stillalive");
//     }
//    while(millis()-start_time < 8000){delay(400);nh.spinOnce();}
//    if(current_plane == desired_plane){return(true);}
//    else{return(false);}
//}
