//FACE      orient    forward     reverse     ...
//Face 1    1234           90  -- 180
//Face 2    1234           90  -- 180
//Face 3    1234           0   -- 90 
//Face 4    1234           90  -- 180
//Face 5    1234
//Face 6    1234
//Face 1    2546           90  -- 180
//Face 2    2546
//Face 3    2546
//Face 4    1234f
//Face 5    1234
//Face 6    1234
//Face 1    1234           90  -- 180
//Face 2    1234
//Face 3    1234
//Face 4    1234
//Face 5    1234
//Face 6    1234
////                            1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30
//// FACE Numbers/Orientations                                                                      6B  6A  5B  5A  4B  4A  3B  3A  2B  2A  1B  1A  
//// Cube ID Numbers            1   1                                                                                                               1
//// Passive Cube ID Numbers    P_2 P_2                     P_D P_D                     P_3 P_3 P_3                                                 P_2
//// Commands                               R   R   G   G               B   B   P   P   Y   Y                                                                                    
    
//int mag___values_array[12]        = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
//int angle_values_array[12]        = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
//int signed_angle_values_array[12] = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};

void process_5048()
{
  for(int face = 1; face < 7; face++)
    {
      check_and_process_mag_sensors(face);// angle1 angle2 mag1 mag2 signed_angle1...2... are now updated for face [face]
      int angle1 = angle_values_array[face*2-2];
      int angle2 = angle_values_array[face*2-1];
      // Produces the following:
      // mag_digit_1 = 0; -+ 1 to 30 or 0 if no face
      // mag_digit_2 = 0; -+ 1 to 30 or 0 if no face
      // magnetic_neighbors[face] = 0 (No Neighbors)  | 1 one sensor indicates neighbor or | 2 Both sensors show Neighbors
      //mag1 = read_5048_agc(address1);             delay(3);
      //angle1 = read_5048_angle(address1)/45.5;    delay(10);
      //mag2 = read_5048_agc(address2);             delay(3);
      //angle2 = read_5048_angle(address2)/45.5;    delay(10);
//        if(mag1 > 0 || mag2 > 0)
//          {
//            Serial.print("mag_digit_1: "); Serial.print(mag_digit_1); Serial.print("    ");  Serial.print("ANGLE1: "); Serial.println(angle1); 
//            Serial.print("mag_digit_2: "); Serial.print(mag_digit_2); Serial.print("    ");  Serial.print("ANGLE2: "); Serial.println(angle2);
//          }
          
           if(magnetic_neighbors_most_recent[face] > 1 && magnetic_neighbors[face] == 0)                    {face_rgb(face,0,0,0,1);} // turns off LED's if no longer connected
           
      else if(magnetic_neighbors[face] > 1 && angle1 > 250 && angle1 < 270 && angle2 > 80 && angle2 < 100)  {face_rgb(face,1,0,1,1);} // Purple
      else if(magnetic_neighbors[face] > 1 && angle2 > 250 && angle2 < 270 && angle1 > 80 && angle1 < 100)  {face_rgb(face,0,1,0,1);}
      else if(magnetic_neighbors[face] > 1 && angle1 > 330 && angle1 < 350 && angle2 > 170 && angle2 < 190) {face_rgb(face,0,0,1,1);}
      else if(magnetic_neighbors[face] > 1 && angle2 > 330 && angle2 < 350 && angle1 > 170 && angle1 < 190) {face_rgb(face,1,0,0,1);}

      // NEED TO WORK ON THIS
      else if(magnetic_neighbors[face] > 1 && (face == 1 || face == 2 || face == 4) && angle1 > 70 && angle1 < 110 && angle2 > 165 && angle2 < 195)   {Serial.println("ia f 6000 2200 10");delay(1500);Serial.println("stillalive");delay(1300);Serial.println("stillalive");}//cmd = "forward_traverse";} 
      else if(magnetic_neighbors[face] > 1 && face == 3 && angle2 < 105 && angle2 > 75 && (angle1 > 348 || angle1 < 12))                              {Serial.println("ia f 6000 2200 10");delay(1500);Serial.println("stillalive");delay(1300);Serial.println("stillalive");}
      //
      
      else if(magnetic_neighbors[face] > 1 && angle1 > 300 && angle2 > 300)                                 {face_rgb(face,1,1,0,1);} // yellow
      else if(magnetic_neighbors[face] > 1)                                                                 {face_rgb(face,1,1,0,1);} //cmd = "rain";} // yellow
      magnetic_neighbors_most_recent[face] = magnetic_neighbors[face]; // shift the time value by one step
    }
}

void check_5048_frame()
{
  mag4 = read_5048_agc(address4);             delay(3);
  angle4 = read_5048_angle(address4)/45.5;    delay(10);
  if(mag4 >= 255) {signed_angle4 = angle4*(-1);}  else{signed_angle4 = angle4;}
}


void check_and_process_mag_sensors(int face) // This function updates a bunch of variables for the magnetic connections
{
  int mag_digit_1 = 0;
  int mag_digit_2 = 0;
  int signed_angle1;
  int signed_angle2;

  int mag1 = read_5048_agc(address1);             delay(3);  // reads the value of the magnetic field strength for sensor_1
  int angle1 = read_5048_angle(address1)/45.5;    delay(10); // reads the ANGLE value for sensor_1 - converts from 14 Bit to an integer in degrees.
  int mag2 = read_5048_agc(address2);             delay(3);  // reads the value of the magnetic field strength for sensor_2
  int angle2 = read_5048_angle(address2)/45.5;    delay(10); // reads the ANGLE value for sensor_2 - converts from 14 Bit to an integer in degrees.
////{ These two blocks of code "digitize" the magnet readings into numbers from -30 to +30 and with 0 being an error, +1-30 representing digits with measureable 
//// strength in the magnetic field (We are confident the sensor is reading an actual magnet) while -1-30 represents a digitized angle reading where we are NOT
//// condifent that the magnet is an actual magnet...
        if (mag1 == 0)                      {mag_digit_1 = 0;}
   else if (angle1 < 6 || angle1 > 354)     {mag_digit_1 = 1;}
   else                                     {mag_digit_1 = int(angle1 + 18)/12;}

        if (mag2 == 0)                      {mag_digit_2 = 0;}
   else if(angle2 < 6 || angle2 > 354)      {mag_digit_2 = 1;}  
   else                                     {mag_digit_2 = int(angle2 + 18)/12;}

  if(mag1 >= 255) {signed_angle1 = angle1*(-1); mag_digit_1 = mag_digit_1*(-1);}  else{signed_angle1 = angle1;}
  if(mag2 >= 255) {signed_angle2 = angle2*(-1); mag_digit_2 = mag_digit_2*(-1);}  else{signed_angle2 = angle2;}
  if(mag1 == 0 || mag2 == 0) {mag1 = 0; mag2 = 0; magnetic_neighbors[face] = 0; error_flag = 1;}
  
     else if((mag1 < 255 && mag1 > 0) && (mag2 < 255 && mag2 > 0)) // If precense of both BOTH magnets are detected, magnetic_neighbors[face] entry holds a 2
          {
            magnetic_neighbors[face] = 2;
          }
     else if(mag1 < 255 || mag2 < 255) 
            magnetic_neighbors[face] = 1; // if ONLY one of 
     else 
          {
            magnetic_neighbors[face] = 0;
          }
 /// UPDATE Global arrays - indexed by (face*2-2) and (face*2-1)   
  mag___values_array[face*2-2]          = mag1;
  mag___values_array[face*2-1]          = mag2;
  mag_digit_array[face*2-2]             = mag_digit_1;
  mag_digit_array[face*2-1]             = mag_digit_2;
  angle_values_array[face*2-2]          = angle1;
  angle_values_array[face*2-1]          = angle2;
  signed_angle_values_array[face*2-2]   = signed_angle1;
  signed_angle_values_array[face*2-1]   = signed_angle2;
}


