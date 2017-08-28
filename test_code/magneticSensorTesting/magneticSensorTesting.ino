//define ADDRESS 0x43
//#define ADDRESS1 0x42
//define ADDRESS 0x40
// I2C device found at address 0x70   - segment display

#define Switch 12
#define LED 13
#define Awesome_push_switch 2
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
Adafruit_7segment matrix1 = Adafruit_7segment();
Adafruit_7segment matrix2 = Adafruit_7segment();
///////////////////////////////////////////////////////////
/////////////// FACE + ORIENTATION BIT ////////////////////
  int eps = 6;   // range around the centerpoint which still qualifys
  
  int v1a = 348; // Magnet centerpoint for face 1 magnet position a
  int v1b = 336;
  int v2a = 324;
  int v2b = 312;
  int v3a = 300;
  int v3b = 288;
  int v4a = 276;
  int v4b = 264; 
  int v5a = 252;
  int v5b = 240;
  int v6a = 228;
  int v6b = 216; 
///////////////////////////////////////////////////////////
/////////////////////CUBE ID NUMBER////////////////////////
  int ID_eps = 12;
  
  int cubeID_1  = 6;
  int cubeID_2  = 30;
  int cubeID_3  = 54;
  int cubeID_4  = 78;
  int cubeID_5  = 102;
  int cubeID_6  = 126;
  int cubeID_7  = 150;
  int cubeID_8  = 174;
  int cubeID_9  = 198;
///////////////////////////////////////////////////////////
//////////////////////PASSIVE CUBES////////////////////////
  int passive_ID = 90;
  int passive_A  = 180;
  int passive_B  = 0;
///////////////////////////////////////////////////////////
//////////////////////SPECIAL COMMANDS/////////////////////
  int cmd_red       = 30;
  int cmd_blue      = 50;
  int cmd_green     = 70;
  int cmd_purple    = 90;
  int cmd_yellow    = 110;
  int cmd_teal      = 130;
  int cmd_move      = 150;
  int cmd_s1        = 170;
  int cmd_s2        = 190;
  int cmd_s3        = 330;
  int cmd_s4        = 310;
  int cmd_s5        = 290;
  int cmd_s6        = 270;
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

int address1 = 0x43;
int address2 = 0x42;
int address3 = 0x41;
int address4 = 0x40;
int mode_switch = 0;
float AMS5048_scaling_factor = 45.5111;

void setup()
{
  
  Wire.begin(2,14);
  Serial.begin(115200);
  pinMode(LED,    OUTPUT);
  pinMode(Switch, INPUT);
  pinMode(Awesome_push_switch, INPUT);
  delay(50);
  digitalWrite(Switch, HIGH);
  matrix1.begin(0x70);
  matrix2.begin(0x71);
}



 void loop()
{
mode_switch = digitalRead(Switch);
//if(mode_switch < 0) // JUST SHOW NUMBERS
if(1)
  {
  if((read_intensity(address4) < 255 && read_intensity(address4) > 0) || (read_intensity(address4) < 255 && read_intensity(address4) > 0)
  || (read_intensity(address4) < 255 && read_intensity(address4) > 0) || (read_intensity(address4) < 255 && read_intensity(address4) > 0)
  )

  delay(100);
  Serial.println("stillalive");
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW); 
  
  
  if(read_intensity(address4) < 255 && read_intensity(address4) > 0)
  {
  matrix1.blinkRate(0);
  matrix1.print(round((read_angle(address4)*10)/AMS5048_scaling_factor));
  matrix1.writeDisplay();
  }
  else
  {
   matrix1.blinkRate(1);
   matrix1.print(round(read_angle(address4)/AMS5048_scaling_factor));
   matrix1.writeDisplay();
  }

  if(read_intensity(address2) < 255 && read_intensity(address2) > 0)
  {
  matrix2.blinkRate(0);
  matrix2.print(round(read_angle(address4)/AMS5048_scaling_factor));
  matrix2.writeDisplay();
  }
  else
  {
   matrix2.blinkRate(1);
   matrix2.print(round(read_angle(address4)/AMS5048_scaling_factor));
   matrix2.writeDisplay();
  }
  delay(50);
  }

  
else // CUBE NUMBER AND FACE NUMBER
{
  int reading_1 = 0;
  int read_id = 0;
  int read_face = 0;
  int reading_2 = 0;
  int orientation = 0;
  if(
   (read_intensity(address1) < 255 && read_intensity(address1) > 0) || (read_intensity(address2) < 255 && read_intensity(address2) > 0)
  || (read_intensity(address2) < 255 && read_intensity(address2) > 0) || (read_intensity(address4) < 255 && read_intensity(address4) > 0)
  )
  {
//  Serial.print(read_angle(address1)/AMS5048_scaling_factor);Serial.print("    ");Serial.print(read_intensity(address1));Serial.print("    ");
//  Serial.print(read_angle(address2)/AMS5048_scaling_factor);Serial.print("    ");Serial.print(read_intensity(address2));Serial.print("    ");
//  Serial.print(read_angle(address3)/AMS5048_scaling_factor);Serial.print("    ");Serial.print(read_intensity(address3));Serial.print("    ");
//  Serial.print(read_angle(address4)/AMS5048_scaling_factor);Serial.print("    ");Serial.println(read_intensity(address4));
  
  reading_1 = round(read_angle(address1)/AMS5048_scaling_factor);
  reading_2 = round(read_angle(address2)/AMS5048_scaling_factor);
 ////////////////////////// Determine if Cube is present - and if so its ID#
if((reading_1 >= 0 && reading_1 < 15) || (reading_2 >= 0 && reading_2 < 15) || (reading_1 > 355 && reading_1 < 361) || (reading_2 > 355 && reading_2 < 361)) {read_id = 1;}

 ////////////////////////// DETERMINE FACE AND ORIENTATION
  // FACE # 1
       if(reading_1 >= v1a - eps && reading_1 <= v1a + eps) {read_face = 1; orientation = 1;}
  else if(reading_2 >= v1a - eps && reading_2 <= v1a + eps) {read_face = 1; orientation = 2;} 
  else if(reading_1 >= v1b - eps && reading_1 <= v1b + eps) {read_face = 1; orientation = 3;}
  else if(reading_2 >= v1b - eps && reading_2 <= v1b + eps) {read_face = 1; orientation = 4;} 
  // FACE # 2
  else if(reading_1 >= v2a - eps && reading_1 <= v2a + eps) {read_face = 2; orientation = 1;}
  else if(reading_2 >= v2a - eps && reading_2 <= v2a + eps) {read_face = 2; orientation = 2;} 
  else if(reading_1 >= v2b - eps && reading_1 <= v2b + eps) {read_face = 2; orientation = 3;}
  else if(reading_2 >= v2b - eps && reading_2 <= v2b + eps) {read_face = 2; orientation = 4;} 
  // FACE # 3
  else if(reading_1 >= v3a - eps && reading_1 <= v3a + eps) {read_face = 3; orientation = 1;}
  else if(reading_2 >= v3a - eps && reading_2 <= v3a + eps) {read_face = 3; orientation = 2;}
  else if(reading_1 >= v3b - eps && reading_1 <= v3b + eps) {read_face = 3; orientation = 3;}
  else if(reading_2 >= v3b - eps && reading_2 <= v3b + eps) {read_face = 3; orientation = 4;} 
  // FACE # 4
  else if(reading_1 >= v4a - eps && reading_1 <= v4a + eps) {read_face = 4; orientation = 1;}
  else if(reading_2 >= v4a - eps && reading_2 <= v4a + eps) {read_face = 4; orientation = 2;} 
  else if(reading_1 >= v4b - eps && reading_1 <= v4b + eps) {read_face = 4; orientation = 3;}
  else if(reading_2 >= v4b - eps && reading_2 <= v4b + eps) {read_face = 4; orientation = 4;} 
  // FACE # 5
  else if(reading_1 >= v5a - eps && reading_1 <= v5a + eps) {read_face = 5; orientation = 1;} 
  else if(reading_2 >= v5a - eps && reading_2 <= v5a + eps) {read_face = 5; orientation = 2;}
  else if(reading_1 >= v5b - eps && reading_1 <= v5b + eps) {read_face = 5; orientation = 3;} 
  else if(reading_2 >= v5b - eps && reading_2 <= v5b + eps) {read_face = 5; orientation = 4;} 
  // FACE # 6
  else if(reading_1 >= v6a - eps && reading_1 <= v6a + eps) {read_face = 6; orientation = 1;}
  else if(reading_2 >= v6a - eps && reading_2 <= v6a + eps) {read_face = 6; orientation = 2;} 
  else if(reading_1 >= v6b - eps && reading_1 <= v6b + eps) {read_face = 6; orientation = 3;}
  else if(reading_2 >= v6b - eps && reading_2 <= v6b + eps) {read_face = 6; orientation = 4;} 
  
  if(read_intensity(address1) < 255 && read_intensity(address1) > 0)
  {
  matrix1.blinkRate(0);
  matrix1.print(read_id);
  matrix1.writeDisplay();
  }
  else
  {
   matrix1.blinkRate(1);
   matrix1.print(0);
   matrix1.writeDisplay();
  }

  if(read_intensity(address2) < 255 && read_intensity(address2) > 0)
  {
  matrix2.blinkRate(0);
  //matrix2.print(round(read_angle(address2)/AMS5048_scaling_factor));
  matrix2.print(read_face);
  matrix2.writeDisplay();
  }
  else
  {
   matrix2.blinkRate(1);
   //matrix2.print(round(read_angle(address2)/AMS5048_scaling_factor));
   matrix2.print(0);
   matrix2.writeDisplay();
  }
  delay(50);
  }
  else
  {
  matrix1.print(0);
  matrix1.writeDisplay();
  matrix2.print(0);
  matrix2.writeDisplay();
  }
  }
}

int read_intensity(int address)
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
}


int read_angle(int address)
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

