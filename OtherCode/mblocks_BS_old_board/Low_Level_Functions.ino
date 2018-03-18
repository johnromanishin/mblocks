bool Sensor_Update(bool magnet, bool light, bool state)
{
  for(int face = 1; face < 7; face++)
  {
    if(magnet)
    {
    delay(10);
    }

    if(light)
    {
    delay(10);
    }

    if(state)
    {
    delay(10);
    }
  }
}

bool turn_on_sensors(int face)
{
  delay(10);
}

void shift_and_update_ambient_sensors()
{
   for(int face = 1; face < FACES; face++)
   {
      for(int j = ARRAY_LENGTH_LONG-1; j > 0; j--){ambient_values[face][j] = ambient_values[face][j-1];}
      ambient_values[face][0] = read_ambient(face); 
   }
}

String check_for_patterns_gyros()
{
int low = 1200;
int high = 5000;
String color = "default";
///////////////////////////////////////THESE represent rolling about 3 axis...
     if(motion_sum_log[1] > 15000 && motion_sum_log[8] > 15000 && motion_sum_log[17] > 15000)                                           {color = "angry";cmd = "chill";}
     if(GyX_log[0] > low && GyX_log[0] < high && GyX_log[3] > low && GyX_log[3] < high && GyX_log[8] > low && GyX_log[8] < high)        {color = "y"; y_counter = 4;}
else if(GyX_log[0] < -low && GyX_log[0] > -high && GyX_log[3] < -low && GyX_log[3] > -high &&GyX_log[8] < -low && GyX_log[8] > -high)   {color = "p"; p_counter = 4;}
else if(GyY_log[0] > low && GyY_log[0] < high && GyY_log[3] > low&& GyY_log[3] < high &&GyY_log[8] > low && GyY_log[8] < high)          {color = "r"; r_counter = 4;}
else if(GyY_log[0] < -low && GyY_log[0] > -high && GyY_log[3] < -low && GyY_log[3] > -high &&GyY_log[8] < -low && GyY_log[8] > -high)   {color = "b"; b_counter = 4;}
else if(GyZ_log[0] > low && GyZ_log[0] < high && GyZ_log[3] > low && GyZ_log[3] < high &&GyZ_log[8] > low && GyZ_log[8] < high)         {color = "t"; t_counter = 4;}
else if(GyZ_log[0] < -low && GyZ_log[0] > -high && GyZ_log[3] < -low && GyZ_log[3] > -high &&GyZ_log[8] < -low && GyZ_log[8] > -high)   {color = "g"; g_counter = 4;}
return(color);
}

void shift_and_update_IMU()
{
  for(int j = ARRAY_LENGTH_LONG;  j > 0; j--){motion_sum_log[j] = motion_sum_log[j-1];}  motion_sum_log[0] = read_gyro_and_accel(MPU_parasite); 
  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){GyX_log[j] = GyX_log[j-1];}  GyX_log[0] = GyX;
  for(int a = ARRAY_LENGTH_SHORT; a > 0; a--){GyY_log[a] = GyY_log[a-1];}  GyY_log[0] = GyY;
  for(int b = ARRAY_LENGTH_SHORT; b > 0; b--){GyZ_log[b] = GyZ_log[b-1];}  GyZ_log[0] = GyZ;
  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){AcX_log[j] = AcX_log[j-1];}  AcX_log[0] = AcX;
  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){AcY_log[j] = AcY_log[j-1];}  AcY_log[0] = AcY;
  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){AcZ_log[j] = AcZ_log[j-1];}  AcZ_log[0] = AcZ;
}


void write_face(int face)
{
if     (face == 1) {write_expander(0x20, F1B1, F1B2);}
else if(face == 2) {write_expander(0x21, F2B1, F2B2);}
else if(face == 3) {write_expander(0x22, F3B1, F3B2);}
else if(face == 4) {write_expander(0x23, F4B1, F4B2);}
else if(face == 5) {write_expander(0x24, F5B1, F5B2);}
else if(face == 6) {write_expander(0x25, F6B1, F6B2);}
}

void write_expander(int address, byte b1, byte b2)
{
  Wire.beginTransmission(address);
  Wire.write(b1);
  Wire.write(b2);
  Wire.endTransmission();
}

void reset_expander(int address)
{
  Wire.beginTransmission(address);
  Wire.write(byte(B11111111));
  Wire.write(byte(B11111111));
  Wire.endTransmission();
}

int read_expander(int address)
{
  Wire.beginTransmission(0x27);
  Wire.endTransmission();
  Wire.requestFrom(address, 2);
  int reading = 0;
  if (Wire.available())
  {
    reading = Wire.read();
    reading = reading << 2;
    reading |= Wire.read();
  }
  return reading;
}


int read_ambient()
{
  activate_light_sensor();
  delay(120);
  int reading = 0;
  Wire.beginTransmission(light_address); 
  Wire.write(byte(0x8C)); // this is the register where the Ambient values are stored
  Wire.endTransmission();
  Wire.requestFrom(light_address, 2);
  if (2 <= Wire.available()) //ambientLight  = twiBuf[0] << 2;
  {
    reading = Wire.read();
    reading |= Wire.read()<<8;
  }
  Serial.println(reading);
  return reading;
}

void activate_light_sensor()
{
  Wire.beginTransmission(light_address); 
  Wire.write(byte(0x80)); // 1000 0000 - Selects command register
  Wire.write(byte(0x03)); // 0000 0010 - Activates device
  Wire.endTransmission();
  delay(3);
  Wire.beginTransmission(light_address); 
  Wire.write(byte(0x81));
  Wire.write(byte(0x11)); // Sets integration time to 100 ms ... // 00010XX sets gain to 16x
  Wire.endTransmission();
}

void activate_sensors_no_light(int face)
{
if     (face == 1) {F1B1 = sensors_active; write_face(face);}
else if(face == 2) {F2B1 = sensors_active_with_light; write_face(face);}
else if(face == 3) {F3B1 = sensors_active_with_light; write_face(face);}
else if(face == 4) {F4B1 = sensors_active_with_light; write_face(face);}
else if(face == 5) {F5B1 = sensors_active_with_light; write_face(face);}
else if(face == 6) {F6B1 = sensors_active_with_light; write_face(face);}
else{}
}

void activate_sensors(int face)
{
if     (face == 1) {F1B1 = sensors_active_with_light; write_face(face);}
else if(face == 2) {F2B1 = sensors_active_with_light; write_face(face);}
else if(face == 3) {F3B1 = sensors_active_with_light; write_face(face);}
else if(face == 4) {F4B1 = sensors_active_with_light; write_face(face);}
else if(face == 5) {F5B1 = sensors_active_with_light; write_face(face);}
else if(face == 6) {F6B1 = sensors_active_with_light; write_face(face);}
else{}
}

void deactivate_sensors(int face)
{
if     (face == 1) {F1B1 = all_high; write_face(face);}
else if(face == 2) {F2B1 = all_high; write_face(face);}
else if(face == 3) {F3B1 = all_high; write_face(face);}
else if(face == 4) {F4B1 = all_high; write_face(face);}
else if(face == 5) {F5B1 = all_high; write_face(face);}
else if(face == 6) {F6B1 = all_high; write_face(face);}
else{}
}

void turn_off_face(int face)
{
if     (face == 1) {F1B2 = all_high; F2B2 = all_high; write_all_faces();} 
else if(face == 2) {F2B2 = all_high; F3B2 = all_high; write_all_faces();}
else if(face == 3) {F3B2 = all_high; F4B2 = all_high; write_all_faces();}
else if(face == 4) {F4B2 = all_high; F1B2 = all_high; write_all_faces();}
else if(face == 5) {}
else if(face == 6) {}
}

void light_up_purple(int face)
{
if     (face == 1) {F1B2 = purple; F2B2 = purple; F3B2 = all_high; F4B2 = all_high; write_all_faces();} 
else if(face == 2) {F2B2 = purple; F3B2 = purple; F1B2 = all_high; F4B2 = all_high; write_all_faces();}
else if(face == 3) {F3B2 = purple; F4B2 = purple; F1B2 = all_high; F2B2 = all_high; write_all_faces();}
else if(face == 4) {F4B2 = purple; F1B2 = purple; F2B2 = all_high; F3B2 = all_high; write_all_faces();}
else if(face == 5) {}
else if(face == 6) {}
}

////////////////////////////FUNCTIONS LIST//////
// FUN1   bool  face_board_sleep(int face)
// FUN2   int   is_i2c_connected()
// FUN3   int   which_face_is_forward(int plane)
// FUN4   int   which_face_is_reverse()
// FUN5   int   which_face_is_down()
// FUN6   int   opposite_face(int face)
// FUN7   void  turn_off_esp()
// FUN8   void  go_to_sleep()
// FUN9   int   which_plane_slow()
// FUN10  int   which_plane_fast()
// FUN11  int   which_orientation_is_central()
// FUN12  int   which_face_is_up(int threshold)
////////////////////// LED RELATED FUNCTIONS //////////
// FUN13  void  taste_the_rainbow()
// FUN14  int   display_ring_purple(int which_plane)
// FUN15  bool  IRLED_OFF()
// FUN16  bool  IRLED(int face, bool led1, bool led2, bool led3, bool led4)
// FUN17  bool  rgb(int face, bool top, bool r, bool g, bool b)
// FUN18  int   face_rgb(int face, int r, int g, int b, int lights_off)
// FUN19  int   rgbr()
// FUN20  int   rgbg()
// FUN21  int   rgbp()
// FUN22  int   rgbb()
// FUN23  int   rgby()
// FUN24  int   irall()
// FUN25  int   rgbt()
// FUN26  int   rgbw()
// FUN27  void  blink_teal()
// FUN28  void  blink_purple()
// FUN29  void  blink_red()
// FUN30  void  blink_yellow()
// FUN31  void  blink_blue()
// FUN32  void  blink_ir()
// FUN33  void  blink_ir_face()
// FUN34  void  blink_white()
// FUN35  void  blink_green()
// FUN36  int   blink_green_old(int times)
// FUN37  int   blink_yellow_old(int times)
// FUN38  int   blink_purple_old(int times)
// FUN39  int   blink_blue_old(int times)
// FUN40  int   blink_red_old(int times)
// FUN41  int   blink_teal_old(int times)
// FUN42  int   blink_white_old(int times)
// FUN43  int   blink_IR_old(int times)
// FUN44  int   blink_rainbow_old(int times)
// FUN45  int   rgboff() 
// FUN46  int   blink_red_length(int len)
////////////////////// SENSOR RELATED //////////
// FUN47  int   get_battery_voltage()
// FUN48  int   read_accel_central(){ 
// FUN49  void  wake_accel()
// FUN50  long  read_accel()
// FUN51  long  read_gyro_and_accel()
// FUN52  long  read_gyro()
// FUN53  void  fbrxen(bool state)                        // Turns on Ambient light sensor
// FUN54  int   read_5048_agc(int address)
// FUN55  int   read_5048_angle(int address)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// I2C COMMANDS!!!///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUN1
bool face_board_sleep(int face)
{
  if (face < 1 || face > 6){return(false);}
  digitalWrite(Switch, HIGH);
  int i2c1 = 0x50; // FB_REGISTER_ADDR_RX_FLUSH  
  int i2c2 = 0x01;
  Wire.beginTransmission(face);
  Wire.write(byte(i2c1));
  Wire.write(byte(i2c2));
  Wire.endTransmission();
  return(true);
} 

// FUN2
int is_i2c_connected() 
//
{ 
digitalWrite(Switch, HIGH);
Wire.beginTransmission(0x69);
int error = Wire.endTransmission();
return(error);
}

// FUN3
int which_face_is_forward(int plane) // FUN3 // plane should be either int 1234, 1536 2546 or -1
{
  int face = which_face_is_up(12000);
  if      (face == 2 && plane == 1234  ||  face == 6 && plane == 1536)                                 {forward_face = 1;}
  else if (face == 3 && plane == 1234  ||  face == 5 && plane == 2546)                                 {forward_face = 2;}
  else if (face == 4 && plane == 1234  ||  face == 5 && plane == 1536)                                 {forward_face = 3;}
  else if (face == 1 && plane == 1234  ||  face == 6 && plane == 2546)                                 {forward_face = 4;}
  else if (face == 4 && plane == 2546  ||  face == 1 && plane == 1536)                                 {forward_face = 5;}
  else if (face == 2 && plane == 2546  ||  face == 3 && plane == 1536)                                 {forward_face = 6;}
  else if((face == 5 && plane == 1234) || (face == 1 && plane == 2546) || (face == 2 && plane == 1536)){forward_face = 10;}
  else if((face == 6 && plane == 1234) || (face == 3 && plane == 2546) || (face == 4 && plane == 1536)){forward_face = 11;}
  else{return(-1);}
  return(forward_face);
}

// FUN4
int which_face_is_reverse()
{
  return(opposite_face(which_face_is_forward(which_plane_fast())));
}

// FUN5
int which_face_is_down()
{
  return(opposite_face(which_face_is_up(12000)));
}

// FUN6
int opposite_face(int face)
{
  if(face == 1){return(3);}
  else if(face == 2){return(4);}
  else if(face == 3){return(1);}
  else if(face == 4){return(2);}
  else if(face == 5){return(6);}
  else if(face == 6){return(5);}
  else{return(-1);}
}

// FUN7
void turn_off_esp()
{
for(int i = 0; i < 4;i++){Serial.println("espoff");delay(400);}// This should turn the lights completely off... Suicide basically
}

// FUN8
void go_to_sleep()
{
digitalWrite(Switch, HIGH);
delay(100);
fbrxen(false);
blink_blue_old(6); // This just flashes the lights blue 5 times
int test_time = millis();
while(Serial.available() && (millis() - test_time) < 300){Serial.read();} // empty serial buffer just in case...
delay(100);
digitalWrite(Switch, LOW);
delay(100);
for(int i = 0; i < 2;i++){Serial.println("fbsleep");delay(300);Serial.println("stillalive");delay(100);}// Tell the NRF51422 to sleep the face-boards 3 seperate times.
digitalWrite(Switch, LOW);
for(int i = 0; i < 2;i++){Serial.println("fbsleep");delay(300);Serial.println("stillalive");delay(100);}// Tell the NRF51422 to sleep the face-boards 3 seperate times.
delay(200);
for(int i = 0; i < 4;i++){Serial.println("sleep");delay(400);Serial.println("stillalive");delay(100);}// This should turn the lights completely off... Suicide basically
cmd = "chill";
}

// FUN9
int which_plane_slow()
{
  int f = which_face_is_up(12000); //
  int c = which_orientation_is_central();
  if(f == -1){delay(100); f = which_face_is_up(11000);} // if cube is moving... wait, try again with lower threshold
  //
       if(f == c && f != -1 && c != -1)                                                                                          {which_plane_am_i_in = 1234;}
  else if((f == 5 && c == 2)||(f == 4 && c == 3)||(f == 3 && c == 6)||(f == 2 && c == 1)||(f == 1 && c == 5)||(f == 6 && c == 4)){which_plane_am_i_in = 2546;}
  else if((f == 5 && c == 1)||(f == 3 && c == 4)||(f == 6 && c == 3)||(f == 1 && c == 2)||(f == 4 && c == 6)||(f == 2 && c == 5)){which_plane_am_i_in = 1536;}
  else{which_plane_am_i_in = -1;}
  return(which_plane_am_i_in);
}

// FUN10

int which_plane_fast()
{
  int threshold = 1700;
  int value_1234 = 9000;
  int value_2546 = 12000;
  int value_1536 = 1000;
  int magnet_value = read_5048_angle(address4);
       if(magnet_value > (value_1234 - threshold) && magnet_value < (value_1234 + threshold))                            {which_plane_am_i_in = 1234;}
  else if(magnet_value > (value_2546 - threshold) && magnet_value < (value_2546 + threshold))                            {which_plane_am_i_in = 2546;}
  else if((magnet_value > (value_1536 - threshold) && magnet_value < (value_1536 + threshold)) || magnet_value > 15900)  {which_plane_am_i_in = 1536;}
  else{which_plane_am_i_in = -1;}
  delay(4);
  return(which_plane_am_i_in);
}

// FUN11
int which_orientation_is_central()
{
//AcX_c, AcY_c, AcZ_c // Central IMU values
  int central_orientation = 0;
  int eps = 2800;
  int thr = 11000; // this is a value low eneough to be less than sqrt(16000) 
  int zer = 0; // Anything below this value is considered to be zero
  int z_t = 16000; // this value is meant to catch anything
  if     (abs(AcX_c - (-thr)) < eps && abs(AcY_c - (thr)) < eps  && abs(AcZ_c - (zer)) < eps) {central_orientation = 4;}
  else if(abs(AcX_c - (thr)) < eps  && abs(AcY_c - (thr)) < eps  && abs(AcZ_c - (zer)) < eps) {central_orientation = 1;}
  else if(abs(AcX_c - (thr)) < eps  && abs(AcY_c - (-thr)) < eps && abs(AcZ_c - (zer)) < eps) {central_orientation = 2;}
  else if(abs(AcX_c - (-thr)) < eps && abs(AcY_c - (-thr)) < eps && abs(AcZ_c - (zer)) < eps) {central_orientation = 3;}
  else if(abs(AcX_c - (-zer)) < eps && abs(AcY_c - (zer)) < eps  && abs(AcZ_c - (z_t)) < eps) {central_orientation = 5;}
  else if(abs(AcX_c - (-zer)) < eps && abs(AcY_c - (zer)) < eps  && abs(AcZ_c - (-z_t)) < eps){central_orientation = 6;}
  else                                                                                        {central_orientation = -1;} // cannot determine orientation... something is wrong
  return(central_orientation);
}

//FUN12
int which_face_is_up(int threshold)
{
  // threshold should be around 12000
  // This function returns an integer representing
  //the face which is point upwards. Returns -1 if results are inconclusive
  digitalWrite(Switch, HIGH);
  face_that_is_up = -1;
  read_accel(MPU_frame); // returns AcX, AcY AcZ values representing the acceleration
  int sum = (abs(AcX)+abs(AcY)+abs(AcZ));
  if(AcZ < -threshold && sum < 25000)        { face_that_is_up = 1; }
  else if (AcZ > threshold && sum < 25000)   { face_that_is_up = 3; }
  else if (AcX < -threshold && sum < 25000)  { face_that_is_up = 6; }
  else if (AcX > threshold && sum < 25000)   { face_that_is_up = 5; }
  else if (AcY < -threshold && sum < 25000)  { face_that_is_up = 2; }
  else if (AcY > threshold && sum < 25000)   { face_that_is_up = 4; }
  return(face_that_is_up);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// LED COMMANDS!!!///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FUN13
void taste_the_rainbow()
{
  //Serial.println("Tring to taste...");
  for(int i = 1; i<5; i++)
  {
  int rr = random(0,2);
  int gg = random(0,2);
  int bb = random(0,2);
  if(rr == 0 && gg == 0 && bb == 0)
  {rr = 1; bb = 1;}
  rgb(i, random(0,2), rr, gg, bb);
  delay(10+random(1,20));
  }
}

// FUN14
int display_ring_purple(int which_plane)
{
  int del = 200;
  if     (which_plane == 1234){face_rgb(1,1,0,1,1); delay(del); face_rgb(2,1,0,1,1); delay(del); face_rgb(3,1,0,1,1); delay(del); face_rgb(4,1,0,1,1); delay(del);rgboff();}
  else if(which_plane == 1536){face_rgb(1,1,0,1,1); delay(del); face_rgb(5,1,0,1,1); delay(del); face_rgb(3,1,0,1,1); delay(del); face_rgb(6,1,0,1,1); delay(del);rgboff();}
  else if(which_plane == 2546){face_rgb(2,1,0,1,1); delay(del); face_rgb(5,1,0,1,1); delay(del); face_rgb(4,1,0,1,1); delay(del); face_rgb(6,1,0,1,1); delay(del);rgboff();}
  else if(which_plane == 52)  {blink_green_old(3);}
  else if(which_plane == 21)  {blink_blue_old(3);}
  else if(which_plane == 15)  {blink_red_old(3);}
  else{rgboff();}
}

// FUN15
bool IRLED_OFF()
{
  for(int i = 1;i<7;i++){
    IRLED(i,0,0,0,0);
  }
}

// FUN16
bool IRLED(int face, bool led1, bool led2, bool led3, bool led4) 
{
  if (face < 1){return(false);}
  digitalWrite(Switch, HIGH);
  int i2c1 = 0x20; // Get number
  int i2c2;
  if (face > 6) {return(false);}
  i2c2 = led1 ? 0x01 : 0x00;
  i2c2 |= led2 ? 0x02 : 0x00;
  i2c2 |= led3 ? 0x04 : 0x00;
  i2c2 |= led4 ? 0x08 : 0x00;
  Wire.beginTransmission(face);
  Wire.write(byte(i2c1));
  Wire.write(byte(i2c2));
  Wire.endTransmission();
  return(true);
}

// FUN17
bool rgb(int face, bool top, bool r, bool g, bool b)
{
  //fbrxen();
  digitalWrite(Switch, HIGH);
  int i2c1 = 0x00;
  int i2c2 = 0x00;
  i2c1 = top ? 0x00 : 0x01;
  i2c2 |= r ? 0x01 : 0x00;
  i2c2 |= g ? 0x02 : 0x00;
  i2c2 |= b ? 0x04 : 0x00;
  if (face > 6) {return(false);}
  Wire.beginTransmission(face);
  Wire.write(byte(i2c1)); //Register for TOP LED
  Wire.write(byte(i2c2)); // 
  Wire.endTransmission();
  //fbrxoff();
}

// FUN18
int face_rgb(int face, int r, int g, int b, int lights_off)
{
  if(face < 1 || face > 6){return(-1);}
  if(face_that_was_last_lit_up != face){rgboff();}
  digitalWrite(Switch, HIGH);
  face_that_was_last_lit_up = face;
  switch (face)
  {
    case 1: rgb(1,1,r,g,b); rgb(1,0,r,g,b); rgb(2,0,r,g,b); rgb(2,1,r,g,b); break;
    case 2: rgb(2,1,r,g,b); rgb(2,0,r,g,b); rgb(3,0,r,g,b); rgb(3,1,r,g,b); break;
    case 3: rgb(3,1,r,g,b); rgb(3,0,r,g,b); rgb(4,0,r,g,b); rgb(4,1,r,g,b); break;
    case 4: rgb(4,1,r,g,b); rgb(4,0,r,g,b); rgb(1,0,r,g,b); rgb(1,1,r,g,b); break;
    case 5: for(int i=1;i<=4;i++) {rgb(i,1,r,g,b);}                         break;
    case 6: for(int i=1;i<=4;i++) {rgb(i,0,r,g,b);}                         break;
  }
}

// FUN19
int rgbr()
{
for(int i=1;i<=4;i++){
rgb(i,1,1,0,0);
rgb(i,0,1,0,0);}
}

// FUN20
int rgbg()
{
for(int i=1;i<=4;i++){
rgb(i,1,0,1,0);
rgb(i,0,0,1,0);}
}

// FUN21
int rgbp()
{
for(int i=1;i<=4;i++){
rgb(i,1,1,0,1);
rgb(i,0,1,0,1);}
}

// FUN22
int rgbb()
{
for(int i=1;i<=4;i++){
rgb(i,1,0,0,1);
rgb(i,0,0,0,1);}
}

// FUN23
int rgby()
{
for(int i=1;i<=4;i++){
rgb(i,1,1,1,0);
rgb(i,0,1,1,0);}
}

// FUN24
int irall()
{
for(int face=1;face<7;face++){IRLED(face,1,1,1,1);}
}

// FUN25
int rgbt()
{
for(int i=1;i<=4;i++){
rgb(i,1,0,1,1);
rgb(i,0,0,1,1);}
}

// FUN26
int rgbw()
{
for(int i=1;i<=4;i++)
  {
  rgb(i,1,1,1,1);
  rgb(i,0,1,1,1);
  }
}

// FUN27
void blink_teal()
{
  if(t_counter < 1){return;} // 
  else
    {
      if(t_counter % 4 == 0 && t_counter > 0)        {rgbt();}
      else if((t_counter+2) % 4 == 0 && t_counter > 0)  {rgboff();}
      else                          {}
    }
    t_counter = t_counter - 1;
}


// FUN28
void blink_purple()
{
  if(p_counter < 1){return;} // 
  else
    {
      if(p_counter % 4 == 0 && p_counter > 0)        {rgbp();}
      else if((p_counter+2) % 4 == 0 && p_counter > 0)  {rgboff();}
      else                          {}
    }
    p_counter = p_counter - 1;
}

// FUN29
void blink_red()
{
  if(r_counter < 1){return;} // 
  else
    {
      if(r_counter % 4 == 0 && r_counter > 0)        {rgbr();}
      else if((r_counter+2) % 4 == 0 && r_counter > 0)  {rgboff();}
      else                          {}
    }
    r_counter = r_counter - 1;
}

// FUN30
void blink_yellow()
{
  if(y_counter < 1){return;} // 
  else
    {
      if(y_counter % 4 == 0 && y_counter > 0)        {rgby();}
      else if((y_counter+2) % 4 == 0 && y_counter > 0)  {rgboff();}
      else                          {}
    }
    y_counter = y_counter - 1;
}

// FUN31
void blink_blue()
{
  if(b_counter < 1){return;} // 
  else
    {
      if(b_counter % 4 == 0 && b_counter > 0)        {rgbb();}
      else if((b_counter+2) % 4 == 0 && b_counter > 0)  {rgboff();}
      else                          {}
    }
    b_counter = b_counter - 1;
}

void blink_ir()
{
  if(ir_counter[0] < 1){return;} // 
  else
    {
      if(ir_counter[0] % 4 == 0 && ir_counter[0] > 0)        {irall();}
      else if((ir_counter[0]+2) % 4 == 0 && ir_counter[0] > 0)  {IRLED_OFF();}
      else                          {}
    }
    ir_counter[0] = ir_counter[0] - 1;
}
// FUN32

void blink_ir_face()
{
  for(int i = 1; i <7; i++)
  {
  if(ir_counter[i] < 1){delayMicroseconds(5);} // 
  else
    {
      if(ir_counter[i] % 4 == 0 && ir_counter[i] > 0)        {IRLED(i,1,1,1,1);}
      else if((ir_counter[i]+2) % 4 == 0 && ir_counter[i] > 0)  {IRLED_OFF();}
      else                          {}
    }
    ir_counter[i] = ir_counter[i] - 1;
  }
}
// FUN33


void blink_white()
{
  if(w_counter < 1){return;} // 
  else
    {
      if(w_counter % 4 == 0 && w_counter > 0)        {rgbw();}
      else if((w_counter+2) % 4 == 0 && w_counter > 0)  {rgboff();}
      else                          {}
    }
    w_counter = w_counter - 1;
}
// FUN34


void blink_green()
{
  if(g_counter < 1){return;} // 
  else
    {
      if(g_counter % 4 == 0 && g_counter > 0)        {rgbg();}
      else if((g_counter+2) % 4 == 0 && g_counter > 0)  {rgboff();}
      else                          {}
    }
    g_counter = g_counter - 1;
}
// FUN35


int blink_green_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbg();delay(100);rgboff();delay(75);} 
}
// FUN36


int blink_yellow_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgby();delay(100);rgboff();delay(75);} 
}
// FUN37


int blink_purple_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbp();delay(100);rgboff();delay(75);} 
}
// FUN38


int blink_blue_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbb();delay(100);rgboff();delay(75);} 
}
// FUN39


int blink_red_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbr();delay(100);rgboff();delay(75);} 
}
// FUN40


int blink_teal_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbr();delay(100);rgboff();delay(75);} 
}
// FUN41


int blink_white_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbw();delay(800);rgboff();delay(750);} 
}
// FUN42


int blink_IR_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){irall();delay(100);IRLED_OFF();delay(750);} 
}
// FUN43


int blink_rainbow_old(int times)
{
for(int i = 0; i <times;i++)
{Serial.println("stillalive");rgby();delay(130);rgbr();delay(130);rgbp();Serial.println("stillalive");delay(130);rgbb();delay(130);rgbt();delay(130);rgbg();delay(130);Serial.println("stillalive");} 
}
// FUN44


int rgboff() 
{
  digitalWrite(Switch, HIGH);
  for(int tb = 0;tb<=1;tb++)
  {
  for(int num = 1;num<=4;num++)
    {
      Wire.beginTransmission(num);// Face Board # 4 I2C address
      Wire.write(byte(tb));//Register for TOP LED
      Wire.write(byte(0x00));// 
      Wire.endTransmission();
    }
  }
}
// FUN45

int get_battery_voltage()
  {
    long begin_function = millis();
    while(Serial.available()){Serial.read();} // empty serial buffer just in case...
    delay(3);
    Serial.println("vbat");
    delay(23);
    char prev_char = ' ';
    String temp_string = "";
    int battery_counter = 1;
    while (Serial.available() > 0 && (millis()-begin_function) < 60) // while there are things in the serial buffer...
    {
        char c = Serial.read();
        if(c == ' ' && prev_char == ':')
          {
              for(int i = 0; i < 4; i++)
                  {
                  char a = Serial.read();
                  delayMicroseconds(250);
                  if(isDigit(a)){temp_string += a;}
                  } 
              if(battery_counter == 1){vbat[1] = temp_string.toInt();}
              else if(battery_counter == 2){vbat[2] = temp_string.toInt();}
              else if(battery_counter == 3){vbat[3] = temp_string.toInt();}
              else if(battery_counter == 4){vbat[4] = temp_string.toInt();}
              battery_counter++;
              temp_string = ""; 
          }
        delayMicroseconds(200);
        prev_char = c;
    }
  vbat[0] = (vbat[1]+vbat[2]+vbat[3]+vbat[4])/4;
  return (vbat[0]);
}
//FUN47

int get_vin()
{
  long begin_function = millis();
  while(Serial.available()){Serial.read();} // empty serial buffer just in case...
  delay(5);
  Serial.println("vin");
  delay(23);
  char prev_char      = ' ';
  String temp_string  = "";
  int temp_vin        = 0;
  while (Serial.available() > 0 && (millis()-begin_function) < 60) // while there are things in the serial buffer...
  {
        char c = Serial.read();
        if(c == ' ' && prev_char == ':')
          {
              for(int i = 0; i < 4; i++)
                  {
                  char a = Serial.read();
                  delayMicroseconds(250);
                  if(isDigit(a)){temp_string += a;}
                  } 
              temp_vin = temp_string.toInt();
        
          }
        delayMicroseconds(250);
        prev_char = c;
  }
  return(temp_vin);
//  for debugging:
//  Serial.print("This part of this took: ");
//  Serial.println(millis() - begin_function);
}//FUN47

int read_ambient(int face)//(int face)
{
  if(FACE_VERSION == 0)
  {
  int reading = 0;
  digitalWrite(Switch, HIGH);
  if(face > 6 || face < 1) {return(-1);}
  Wire.beginTransmission(face); 
  Wire.write(byte(0x10)); // this is the register where the Ambient values are stored
  Wire.endTransmission();
  Wire.requestFrom(face, 2);
  if (2 <= Wire.available()) //ambientLight  = twiBuf[0] << 2;
  {
    reading =  Wire.read()<<1;     //  ambientLight |= twiBuf[1] >> 6;
    reading |=  Wire.read()>>7;
    return reading;
  }
  else{return(-1);}
  }
  else if(FACE_VERSION == 1)
  {
  delay(1);
  }
  
}

void wake_accel(int address)
{
  Wire.beginTransmission(address);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
} //FUN49

long read_accel(int address)
{
  wake_accel(address);
  Wire.beginTransmission(address);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(address,6,1);
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
//  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
//  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
//  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
//  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  return(abs(AcX)+abs(AcY)+abs(AcZ));
} ////FUN50

long read_gyro_and_accel(int address)
{
  wake_accel(address);
  Wire.beginTransmission(address);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(address,14,1);
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  return((abs(GyX)+abs(GyY)+abs(GyZ)+abs(AcX) + abs(AcY) + abs(AcZ))/10);
} ///FUN51

long read_gyro(int address)
{
  wake_accel(address);
  Wire.beginTransmission(address);
  Wire.write(0x41);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(address,8,1);
//  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
//  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
//  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  return(abs(GyX)+abs(GyY)+abs(GyZ));
} //FUN52

void fbrxen(bool state) // Turns on Ambient light sensor

{
  if(FACE_VERSION == 1){return;}
  if(DEBUG){Serial.println("Beginning FBRXEN");}
  digitalWrite(Switch, HIGH); // Takecontrol of I2C BUS
  delay(1);
  for (int face = 1; face < 7; face++) 
  {
    Wire.beginTransmission(face);
    Wire.write(byte(0x43));
    if(state) {Wire.write(byte(0x01));}
    else      {Wire.write(byte(0x00));}
    Wire.endTransmission(); 
  }
} //FUN53

int read_5048_agc(int address)
// AGC is the "strength" of the magnet returned as an 8-bit number, 255 = magnet field is too weak, 0 = very strong magnetic field.
{
  Wire.beginTransmission(address);
  Wire.write(0xFA);
  Wire.endTransmission();
  Wire.requestFrom(address, 2);

  int reading = 0;
  if (2 <= Wire.available())
  {
    reading = Wire.read();
    reading = reading << 6;
    reading |= Wire.read();
  }
  return reading;
} ////FUN54

int read_5048_angle(int address)
// Returns the angle of the measured magnet as a 14 bit number
{
  Wire.beginTransmission(address);
  Wire.write(byte(255));
  Wire.endTransmission();
  Wire.requestFrom(address, 2);

  int reading = 0;
  if (2 <= Wire.available())
  {
    reading = Wire.read();
    reading = reading << 6;
    reading |= Wire.read();
  }
  return reading;
}

void reset_main_loop_variables() // resets color variables, cmd, "which_demo", etc...
{
  most_recent_traverse_result = 0;      // resets this 
  attempts_traverse = 0;               // resets some random variables
  demo = "nothing";               // also reset this...
  cmd = "chill";
  rgboff(); // turn off Lights
  move_counter = 0; 
  light_tracking_delay_counter = 0;
}







#define Switch                              12
#define LED                                 13
#define FACES 7
#define ARRAY_LENGTH_LONG                   22
#define ARRAY_LENGTH_SHORT                  10
#include <Wire.h>
#include "Adafruit_VL53L0X.h"
#include <ESP8266WiFi.h>

// FACES
// Face 1 = 0x20
//
//

/// <P7> <P6> <P5> <P4> <P3> <P2> <P1> <P0> |||  <P17> <P16> <P15> <P14> <P13> <P12> <P11> <P10>
/// LED1 NC   LEDB EN2  EN1  LEDA NC   NC        LEDC   NC   R1    B1    G1    B2    R2    G2
///           
/// Purple                                         1      1    0   0   1   0   0   1                                                                   
/////// Ports on PCF5275
// P0 |
// P1 | 
// P2 | LED D
// P3 | EN 1 // inverted
// P4 | EN 2
// P5 | LED B
// P6 |
// P7 | LED A

// P10 | G2
// P11 | R2
// P12 | B2
// P13 | G1
// P14 | B1
// P15 | R1
// P16 | X_SHUT
// P17 | LED C


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////VARIABLES FROM MAIN PROGRAM///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////VARIABLES FROM MAIN PROGRAM///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int measured_distance = 0;
int time_addition = 0;
// Light Sensor Notes...

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

//digitalWrite(LED,HIGH);
//delay(500);
//Serial.println(ESP.getChipId(), HEX);
//digitalWrite(LED,LOW);

//void print_i2c()
//{
//byte error, address;
//int nDevices;
// 
//  Serial.println("Scanning...");
// 
//  nDevices = 0;
//  for(address = 1; address < 127; address++ )
//  {
//    // The i2c_scanner uses the return value of
//    // the Write.endTransmisstion to see if
//    // a device did acknowledge to the address.
//    Wire.beginTransmission(address);
//    error = Wire.endTransmission();
// 
//    if (error == 0)
//    {
//      Serial.print("I2C device found at address 0x");
//      if (address<16)
//        Serial.print("0");
//      Serial.print(address,HEX);
//      Serial.println("  !");
// 
//      nDevices++;
//    }
//    else if (error==4)
//    {
//      Serial.print("Unknow error at address 0x");
//      if (address<16)
//        Serial.print("0");
//      Serial.println(address,HEX);
//    }    
//  }
//  if (nDevices == 0)
//    Serial.println("No I2C devices found\n");
//  else
//    Serial.println("done\n");
// 
//  delay(1000);           // wait 5 seconds for next scan
//}

int measure_distance() 
{
  VL53L0X_RangingMeasurementData_t measure;
    
  //Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) 
  {
  measured_distance = measure.RangeMilliMeter;
  time_addition = measured_distance/10;
  return(measured_distance);
  }
}

void enable_VL53()
{
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
}
void read_VL53()
{
  VL53L0X_RangingMeasurementData_t measure;
  
Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }
    
  delay(100);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////NEW FUNCTIONS FROM MAIN PROGRAM///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void write_all_faces()
{
  for(int face = 1; face < 7; face ++)
  {
  write_face(face);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////FUNCTIONSFROM MAIN PROGRAM///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void check_5048_frame()
  {
  mag4 = read_5048_agc(address4);             delay(3);
  angle4 = read_5048_angle(address4)/45.5;    delay(10);
  if(mag4 >= 255) {signed_angle4 = angle4*(-1);}  else{signed_angle4 = angle4;}
  }



//void loop()
//{ 
//  //for(int i = 0; i< 5; i++)
//  {
//  int i = 0;
//  write_expander_special(expander_address+i);
//  Serial.println("-----------------------");
//  Serial.print("FACE: ");Serial.println(i+1);
//  Serial.println("-----------------------");
//  delay(200);
//  //enable_VL53();
//  //for(int i = 0; i < 4; i++)
//  //{
//    //read_VL53();
//  //}
//  Serial.print("Ambient: ");Serial.println(read_ambient());
//  delay(100);
//  Serial.print("MAGNET1: ");Serial.println(read_5048_angle(0x42));
//  Serial.print("MAGNET2: ");Serial.println(read_5048_angle(0x43));
//  print_i2c();
//  delay(200);
//  write_expander_high(expander_address+i);
//  //measure_distance();
//  //if (measured_distance > 500)   {write_expander_high(expander_address); Serial.println("HEY");}
//  //else                           {police(expander_address); Serial.println("YO");}
//  // put your main code here, to run repeatedly:
//  //delay(50);
//}
//}
