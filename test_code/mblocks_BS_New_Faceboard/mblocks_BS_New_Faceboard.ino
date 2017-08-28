#define PRINT_THINGS                        0
#define Switch                              12
#define LED                                 13
#define FACES 7
#define ARRAY_LENGTH_LONG                   22
#define ARRAY_LENGTH_SHORT                  10
#include <Wire.h>
//#include "Adafruit_VL53L0X.h"
//#include <ESP8266WiFi.h>
#include <ArduinoHardware.h>

//************************************************************
//************************************************************
#include <painlessMesh.h>

#define   LED             2       // GPIO number of connected LED, ON ESP-12 IS GPIO2
#define   BLINK_PERIOD    3000000 // microseconds until cycle repeat
#define   BLINK_DURATION  100000  // microseconds LED is on for
#define   MESH_SSID       "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

painlessMesh  mesh;
bool calc_delay = false;
SimpleList<uint32_t> nodes;
void sendMessage() ; // Prototype

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage ); // start with a one second interval

//************************************************************
//************************************************************

// FACES
// Face 1 = 0x20
//
//
/// <P7> <P6> <P5> <P4> <P3> <P2> <P1> <P0> |||  <P17> <P16> <P15> <P14> <P13> <P12> <P11> <P10>
/// LED1 NC   LEDB EN2  EN1  LEDA NC   NC        LEDC   NC   R1    B1    G1    B2    R2    G2
///           
/// Purple                                           1      1    0   0   1   0   0   1                                                                   
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

int MPU_frame = 0x69;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ, AcX_c, AcY_c, AcZ_c; // Variables from all of the accelerometer/ gyros

/// <P7> <P6> <P5> <P4> <P3> <P2> <P1> <P0> |||  <P17> <P16> <P15> <P14> <P13> <P12> <P11> <P10>
/// LED1 NC   LEDB EN2  EN1  LEDA NC   NC        LEDC   NC   R1    B1    G1    B2    R2    G2

// Special Bytes
byte all_high                  = B11111111;
byte all_low                   = B00000000;
byte sensors_active            = B11110111;
byte purple                    = B11001001;
byte green                     = B11110110;
byte blue                      = B11101011;
byte red                       = B11011101;
byte yellow                    = B11010100;
byte sensors_active_with_light = B01010011;
byte last_led_on               = B01111111;



////Holding Bytes/////
byte F1B1 = B11111111;
byte F1B2 = B11111111;
byte F2B1 = B11111111;
byte F2B2 = B11111111;
byte F3B1 = B11111111;
byte F3B2 = B11111111;
byte F4B1 = B11111111;
byte F4B2 = B11111111;
byte F5B1 = B11111111;
byte F5B2 = B11111111;
byte F6B1 = B11111111;
byte F6B2 = B11111111;
//////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////VARIABLES FROM MAIN PROGRAM///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int mag___values_array[12] =        {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
int angle_values_array[12] =        {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
int mag_digit_array[12] =           {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
int signed_angle_values_array[12] = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}; // adds negatice numbers,to show 
                                                                                    //values which are lot within the mag. Field Strength threshold.
int magnetic_neighbors[FACES] =                        {0, 0, 0, 0, 0, 0, 0};
int magnetic_neighbors_most_recent[FACES] =            {0, 0, 0, 0, 0, 0, 0};

int mag_digit_1 = 0;
int mag_digit_2 = 0;

int angle1;
int angle2;
int angle4;

int signed_angle1;
int signed_angle2;
int signed_angle4;

int mag1 = 0;
int mag2 = 0;
int mag4 = 0;
int address1 = 0x42;
int address2 = 0x43;
int address4 = 0x40; // Angle of Central Axis measurement
long motion_sum_log[ARRAY_LENGTH_LONG+1] = {};

long IDnumber = ESP.getChipId();
long msg_id   = system_get_chip_id();
long timer_counter = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////VARIABLES FROM MAIN PROGRAM///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int measured_distance = 0;
int time_addition = 0;
// Light Sensor Notes...
byte light_address = 0x39;
//
//  Data0_reg         = 12,
//  Data1_reg         = 14

// PCA8754 Expander Notes...
//byte expander_address = 0x20;
//
// int write_expander
// int write_expander_off
// p7 p6...

void setup() 
{
    Wire.begin(2,14);
    Serial.begin(115200);
    pinMode(Switch, OUTPUT);
    digitalWrite(Switch, LOW);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    Serial.println("stillalive");
    delay(1000);
    if(get_vin() > 3400){turn_off_esp();delay(100);Serial.println("Putting esp to sleep");}
    delay(500);
    if(get_vin() > 3400){turn_off_esp();delay(100);Serial.println("Putting esp to sleep");}
    Serial.println("espprogram");
    delay(10);
    Serial.println("espprogram");
    delay(10);
    Serial.println("espprogram");
    digitalWrite(Switch, HIGH);
    delay(2000);
    Serial.print("ID NUMBER IS: ");Serial.println(IDnumber);
    delay(500);
    //Serial.println("sma retract 8000");
    delay(400);
    digitalWrite(LED, LOW);
    //for(int i = 0; i < 60; i++){check_5048_frame(); delay(400);}
    
    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
    mesh.onNodeDelayReceived(&delayReceivedCallback);
    mesh.scheduler.addTask( taskSendMessage );
    taskSendMessage.enable() ;
    randomSeed(analogRead(A0));        
}


void loop()
{
//loop_counter++;
//if(loop_counter%10 == 0){reset_faces_power();}
//blink_and_shout();
//shift_and_update_IMU();
all_faces_everything_on();
delay(100);
Serial.print("Infrared: ");Serial.println(read_infrared());
Serial.print("AMBIENT : ");Serial.println(read_ambient());
delay(1000);
all_faces_everything_off();
delay(1000);
//Serial.print("Infrared: ");Serial.println(read_infrared());
//Serial.print("AMBIENT : ");Serial.println(read_ambient());
//delay(1000);
//all_faces_everything_on();
//
//if(millis() > timer_counter)
//  {
//    
//    String msg_new = "";
//    msg_new = (("Cube ID: ") + String(msg_id) + " Sensor Magnitude: " +String(read_5048_agc(address4)) + "   Angle: " + String(read_5048_angle(address4)/45.5) + "  ");
//    timer_counter += 1000;
//    mesh.sendBroadcast(msg_new);
//  }
//mesh.update();
}

//for(int face = 1; face < 7; face++)
//  {
//  activate_sensors(face);
//  //Serial.print("Reading sensors on face: ");Serial.println(face);
//  delay(10);
//  process_5048(face);
//  delay(5);
//  //read_ambient();
//  delay(100);
//  deactivate_sensors(face);
//  delay(10);
//  deactivate_sensors(face);
//  }


void shift_and_update_IMU()
{
  long temp1 = read_gyro_and_accel(0x68);
  delay(10);
  long temp2 = read_gyro_and_accel(0x68);
  delay(10);
  long temp3 = read_gyro_and_accel(0x68);
  for(int j = ARRAY_LENGTH_LONG;  j > 0; j--){motion_sum_log[j] = motion_sum_log[j-1];}  motion_sum_log[0] = (temp1+temp2+temp3)/3; 
  if(PRINT_THINGS){Serial.print("motion_sum_log[0]: "); Serial.println(motion_sum_log[0]);}
  if(PRINT_THINGS){Serial.print("motion_sum_log[1]: "); Serial.println(motion_sum_log[1]);}
  if(PRINT_THINGS){Serial.print("motion_sum_log[2]: "); Serial.println(motion_sum_log[2]);}
  if(motion_sum_log[0] > 10000 && motion_sum_log[1] > 10000 && motion_sum_log[2] > 10000)
  {
    Serial.println("STARTING SLEEP SEQUENCE");
    turn_blue();
    delay(250);
    all_faces_everything_off();
    delay(250);
    turn_blue();
    delay(250);
    all_faces_everything_off();
    delay(250);
    if(read_gyro_and_accel(0x69) > 5000)
    {blink_blue_twice_fast();Serial.println("stillalive");delay(1000);Serial.println("stillalive");
    delay(1000);Serial.println("stillalive"); blink_blue_twice_fast(); delay(1000); 
    for(int i = 0; i < 3; i++){Serial.println("sleep");delay(200);}}
  }
}

void blink_blue_twice_fast()
{
    turn_blue();
    delay(100);
    all_faces_everything_off();
    delay(100);
    turn_blue();
    delay(100);
    all_faces_everything_off();
    delay(100);
}

void turn_color(byte color)
{
    F1B2 = color;
    F2B2 = color;
    F3B2 = color;
    F4B2 = color;
    for(int face = 1; face < 5; face++)
  {
    write_face(face);
  }

}
void turn_blue()
{
    F1B2 = blue;
    F2B2 = blue;
    F3B2 = blue;
    F4B2 = blue;
    F5B2 = blue;
    F6B2 = blue;
  for(int face = 1; face < 7; face++)
  {

    write_face(face);
  }
}

void reset_faces_power()
{
  Serial.println("stillalive");
  digitalWrite(Switch, LOW);
  delay(300);
  digitalWrite(Switch, HIGH);
// delay(10);
//  turn_color(green);
//  delay(60);
//  turn_color(blue);
//  delay(60);
//  turn_color(red);
//  delay(60);
//  turn_color(purple);
//  delay(60);
  all_faces_everything_off();
}

void all_faces_everything_on()
{   
    F1B1 = all_low;
    F1B1 = all_low;
    F1B1 = all_low;
    F1B1 = all_low;
    F1B1 = all_low;
    F1B1 = all_low;
    
    F1B2 = all_low;
    F2B2 = all_low;
    F3B2 = all_low;
    F4B2 = all_low;
    F5B2 = all_low;
    F6B2 = all_low;
  for(int face = 1; face < 7; face++)
  {
    write_face(face);
  }
}

void all_faces_everything_off()
{
    F1B1 = all_high;
    F1B1 = all_high;
    F1B1 = all_high;
    F1B1 = all_high;
    F1B1 = all_high;
    F1B1 = all_high;
    
    F1B2 = all_high;
    F2B2 = all_high;
    F3B2 = all_high;
    F4B2 = all_high;
    F5B2 = all_high;
    F6B2 = all_high;
  for(int face = 1; face < 7; face++)
  {
    write_face(face);
  }
}

void blink_and_shout()
{
    Serial.println("stillalive");
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW); 
    delay(50);
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW); 
    delay(50); 
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

int read_infrared()
{
  activate_light_sensor();
  delay(15);
  int reading = 0;
  Wire.beginTransmission(byte(light_address)); 
  Wire.write(byte(0x8E)); // this is the register where the Ambient values are stored
  Wire.endTransmission();
  Wire.requestFrom(byte(light_address), 2);
  if (2 <= Wire.available()) //ambientLight  = twiBuf[0] << 2;
  {
    reading = Wire.read();
    reading |= Wire.read()<<8;
  }
  //Serial.println(reading);
  return reading;
}

int read_ambient()
{
  activate_light_sensor();
  delay(15);
  int reading = 0;
  Wire.beginTransmission(byte(light_address)); 
  Wire.write(byte(0x8C)); // this is the register where the Ambient values are stored
  Wire.endTransmission();
  Wire.requestFrom(byte(light_address), 2);
  if (2 <= Wire.available()) //ambientLight  = twiBuf[0] << 2;
  {
    reading = Wire.read();
    reading |= Wire.read()<<8;
  }
  //Serial.println(reading);
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
  Wire.write(byte(0x10)); // Sets integration time to 100 ms ... // 00010XX sets gain to 16x
  Wire.endTransmission();
}

//
//int measure_distance() 
//{
//  VL53L0X_RangingMeasurementData_t measure;
//    
//  //Serial.print("Reading a measurement... ");
//  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
//
//  if (measure.RangeStatus != 4) 
//  {
//  measured_distance = measure.RangeMilliMeter;
//  time_addition = measured_distance/10;
//  return(measured_distance);
//  }
//}

//void enable_VL53()
//{
//  if (!lox.begin()) {
//    Serial.println(F("Failed to boot VL53L0X"));
//    while(1);
//  }
//}
//
//void read_VL53()
//{
//  VL53L0X_RangingMeasurementData_t measure;
//  
//Serial.print("Reading a measurement... ");
//  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
//
//  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
//    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
//  } else {
//    Serial.println(" out of range ");
//  }
//    
//  delay(100);
//}

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

void light_up_red(int face)
{
if     (face == 1) {F1B2 = red; F2B2 = red; F3B2 = all_high; F4B2 = all_high; write_all_faces();} 
else if(face == 2) {F2B2 = red; F3B2 = red; F1B2 = all_high; F4B2 = all_high; write_all_faces();}
else if(face == 3) {F3B2 = red; F4B2 = red; F1B2 = all_high; F2B2 = all_high; write_all_faces();}
else if(face == 4) {F4B2 = red; F1B2 = red; F2B2 = all_high; F3B2 = all_high; write_all_faces();}
else if(face == 5) {}
else if(face == 6) {}
}

void light_up_blue(int face)
{
if     (face == 1) {F1B2 = blue; F2B2 = blue; F3B2 = all_high; F4B2 = all_high; write_all_faces();} 
else if(face == 2) {F2B2 = blue; F3B2 = blue; F1B2 = all_high; F4B2 = all_high; write_all_faces();}
else if(face == 3) {F3B2 = blue; F4B2 = blue; F1B2 = all_high; F2B2 = all_high; write_all_faces();}
else if(face == 4) {F4B2 = blue; F1B2 = blue; F2B2 = all_high; F3B2 = all_high; write_all_faces();}
else if(face == 5) {}
else if(face == 6) {}
}

void light_up_green(int face)
{
if     (face == 1) {F1B2 = green; F2B2 = green; F3B2 = all_high; F4B2 = all_high; write_all_faces();} 
else if(face == 2) {F2B2 = green; F3B2 = green; F1B2 = all_high; F4B2 = all_high; write_all_faces();}
else if(face == 3) {F3B2 = green; F4B2 = green; F1B2 = all_high; F2B2 = all_high; write_all_faces();}
else if(face == 4) {F4B2 = green; F1B2 = green; F2B2 = all_high; F3B2 = all_high; write_all_faces();}
else if(face == 5) {}
else if(face == 6) {}
}

void light_up_yellow(int face)
{
if     (face == 1) {F1B2 = yellow; F2B2 = yellow; F3B2 = all_high; F4B2 = all_high; write_all_faces();} 
else if(face == 2) {F2B2 = yellow; F3B2 = yellow; F1B2 = all_high; F4B2 = all_high; write_all_faces();}
else if(face == 3) {F3B2 = yellow; F4B2 = yellow; F1B2 = all_high; F2B2 = all_high; write_all_faces();}
else if(face == 4) {F4B2 = yellow; F1B2 = yellow; F2B2 = all_high; F3B2 = all_high; write_all_faces();}
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////FUNCTIONSFROM MAIN PROGRAM///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void process_5048(int face)
{
   //Serial.print("Processing sensors on face: ");Serial.println(face);
   check_5048b(face);// angle1 angle2 mag1 mag2 signed_angle1...2... are now updated for face [face]
      // Produces the following:
      // mag_digit_1 = 0; -+ 1 to 30 or 0 if no face
      // mag_digit_2 = 0; -+ 1 to 30 or 0 if no face
      // magnetic_neighbors[face] = 0 (No Neighbors)  | 1 one sensor indicates neighbor or | 2 Both sensors show Neighbors
      //mag1 = read_5048_agc(address1);             delay(3);
      //angle1 = read_5048_angle(address1)/45.5;    delay(10);
      //mag2 = read_5048_agc(address2);             delay(3);
      //angle2 = read_5048_angle(address2)/45.5;    delay(10);
        if((mag1 > 0 && mag1 < 254) || (mag2 > 0 && mag2 < 254))
          {
            Serial.print("face: "); Serial.println(face);
            Serial.print("magnetic_neighbors[face]: "); Serial.println(magnetic_neighbors[face]);
            Serial.print("mag_digit_1: "); Serial.print(mag_digit_1); Serial.print("    ");  Serial.print("ANGLE1: "); 
            Serial.print(angle1);  Serial.print("    ");  Serial.print("MAGNITUDE1: "); Serial.println(mag1); 
            Serial.print("mag_digit_2: "); Serial.print(mag_digit_2); Serial.print("    ");  Serial.print("ANGLE2: "); 
            Serial.print(angle2);  Serial.print("    ");  Serial.print("MAGNITUDE1: "); Serial.println(mag2);    
          }
          
           if(magnetic_neighbors_most_recent[face] > 1 && magnetic_neighbors[face] == 0)                    {turn_off_face(face);} // turns off LED's if no cted
      else if(magnetic_neighbors[face] > 1 && (face == 1 || face == 2 || face == 4) && angle1 > 72 && angle1 < 108 && angle2 > 162 && angle2 < 198)   
      {Serial.println("ia f 6000 3000 15 e 10");delay(1500);Serial.println("stillalive");delay(1300);Serial.println("stillalive");}//cmd = "forward_traverse";} 
      else if(magnetic_neighbors[face] > 1 && face == 3 && angle2 < 108 && angle2 > 72 && (angle1 > 343 || angle1 < 12))                              
      {Serial.println("ia f 6000 3000 15 e 10");delay(1500);Serial.println("stillalive");delay(1300);Serial.println("stillalive");}

      else if(magnetic_neighbors[face] > 1 && (face == 1 || face == 2 || face == 4) && angle2 > 72 && angle2 < 108 && angle1 > 162 && angle1 < 198)   
      {Serial.println("ia r 6000 2500 15 e 10");delay(1500);Serial.println("stillalive");delay(1300);Serial.println("stillalive");}//cmd = "forward_traverse";} 
      else if(magnetic_neighbors[face] > 1 && face == 3 && angle1 < 108 && angle1 > 72 && (angle2 > 343 || angle2 < 12))                              
      {Serial.println("ia r 6000 2500 15 e 10");delay(1500);Serial.println("stillalive");delay(1300);Serial.println("stillalive");}

      
      //
      
      else if(magnetic_neighbors[face] > 1 && angle1 > 250 && angle1 < 270 && angle2 > 80 && angle2 < 100)  {light_up_green(face);} // Purple
      else if(magnetic_neighbors[face] > 1 && angle2 > 250 && angle2 < 270 && angle1 > 80 && angle1 < 100)  {light_up_red(face);}
      else if(magnetic_neighbors[face] > 1 && angle1 > 330 && angle1 < 350 && angle2 > 170 && angle2 < 190) {light_up_yellow(face);}
      else if(magnetic_neighbors[face] > 1 && angle2 > 330 && angle2 < 350 && angle1 > 170 && angle1 < 190) {light_up_blue(face);}

      // NEED TO WORK ON THIS
      
      else if(magnetic_neighbors[face] > 1 && angle1 > 300 && angle2 > 300)                                 {light_up_purple(face);} // yellow
      else if(magnetic_neighbors[face] > 1)                                                                 {light_up_purple(face);} //cmd = "rain";} // yellow
      magnetic_neighbors_most_recent[face] = magnetic_neighbors[face]; // shift the time value by one step
      delay(100);    
}

void check_5048_frame()
  {
  mag4 = read_5048_agc(address4);             delay(3);
  angle4 = read_5048_angle(address4)/45.5;    delay(10);
  //Serial.print("Magnitude: ");Serial.println(mag4);
  //Serial.print("Angle: ");    Serial.println(angle4);
  //if(mag4 >= 255) {signed_angle4 = angle4*(-1);}  else{signed_angle4 = angle4;}
  }

void check_5048b(int face) // This function updates a bunch of variables for the magnetic connections
{
//  activate_sensors(face);
 // delay(20);
 // IRLED(face,1,0,1,0);                        delay(10); // Turns on the power to the 5048B sensors on face _face_
  delay(5);
  mag1 = read_5048_agc(address2);             delay(3);  // reads the value of the magnetic field strength for sensor_1
  angle1 = read_5048_angle(address2)/45.5;    delay(10); // reads the ANGLE value for sensor_1 - converts from 14 Bit to an integer in degrees.
  mag2 = read_5048_agc(address1);             delay(3);  // reads the value of the magnetic field strength for sensor_2
  angle2 = read_5048_angle(address1)/45.5;    delay(10); // reads the ANGLE value for sensor_2 - converts from 14 Bit to an integer in degrees.


////{ These two blocks of code "digitize" the magnet readings into numbers from -30 to +30 and with 0 being an error, +1-30 representing digits with measureable 
//// strength in the magnetic field (We are confident we the sensor is reading an actual magnet) while -1-30 represents a digitized angle reading where we are NOT
//// condifent that the magnet is an actual magnet...
        if (mag1 == 0)                      {mag_digit_1 = 0;}
   else if (angle1 < 6 || angle1 > 354)     {mag_digit_1 = 1;}
   else                                     {mag_digit_1 = int(angle1 + 18)/12;}

        if (mag2 == 0)                      {mag_digit_2 = 0;}
   else if(angle2 < 6 || angle2 > 354)      {mag_digit_2 = 1;}  
   else                                     {mag_digit_2 = int(angle2 + 18)/12;}
////} 

  if(mag1 >= 255) {signed_angle1 = angle1*(-1); mag_digit_1 = mag_digit_1*(-1);}  else{signed_angle1 = angle1;}
  if(mag2 >= 255) {signed_angle2 = angle2*(-1); mag_digit_2 = mag_digit_2*(-1);}  else{signed_angle2 = angle2;}
  if(mag1 == 0 || mag2 == 0) {mag1 = 0; mag2 = 0; magnetic_neighbors[face] = 0;}
  
     else if((mag1 < 255 && mag1 > 0) && (mag2 < 255 && mag2 > 0)) // If precense of both BOTH magnets are detected, magnetic_neighbors[face] entry holds a 2
          {
            //Serial.print("Connected to Something!");  
            //Serial.print("ANGLES: "); Serial.print(angle1); Serial.print("    "); Serial.print(angle2); Serial.println("    ");
            magnetic_neighbors[face] = 2;
          }
     else if(mag1 < 255 || mag2 < 255) 
            magnetic_neighbors[face] = 1; // if ONLY one of 
     else 
          {
            magnetic_neighbors[face] = 0;
          }
          
  mag___values_array[face*2-2]          = mag1;
  mag___values_array[face*2-1]          = mag2;
  mag_digit_array[face*2-2]             = mag_digit_1;
  mag_digit_array[face*2-1]             = mag_digit_2;
  angle_values_array[face*2-2]          = angle1;
  angle_values_array[face*2-1]          = angle2;
  signed_angle_values_array[face*2-2]   = signed_angle1;
  signed_angle_values_array[face*2-1]   = signed_angle2;
}

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
}  //FUN55





///////
////////
////////


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
//  Serial.println("   ");
//  Serial.print("AcX: ");Serial.println(AcX);
//  Serial.print("AcY: ");Serial.println(AcY);
//  Serial.print("AcZ: ");Serial.println(AcZ);
//  Serial.print("Tmp: ");Serial.println(Tmp);
//  Serial.print("AcX: ");Serial.println(GyX);
//  Serial.print("AcY: ");Serial.println(GyY);
//  Serial.print("AcZ: ");Serial.println(GyZ);
  long sum_of_gyros =  (abs(GyX)+abs(GyY)+abs(GyZ));
  float sum_of_accel_sqrt = (sqrt(AcX*AcX+AcY*AcY+AcZ*AcZ)-16000);
  if(PRINT_THINGS){Serial.print("sum_of_Gyros: "); Serial.println(sum_of_gyros);}
  if(PRINT_THINGS){Serial.print("sum_of_accel_sqrt: "); Serial.println(sum_of_accel_sqrt);}
  if(PRINT_THINGS){Serial.print("Ratio of gyro to accel"); Serial.println(sum_of_gyros/sum_of_accel_sqrt);}
  return(sum_of_gyros);
} 

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

void turn_off_esp()
{
Serial.println("stillalive");
for(int i = 0; i < 4;i++){Serial.println("espoff");delay(400);}// This should turn the lights completely off... Suicide basically
}


