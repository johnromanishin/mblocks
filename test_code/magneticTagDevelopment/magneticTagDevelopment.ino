#define Switch 12
#define LED 13
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix1 = Adafruit_7segment();
Adafruit_7segment matrix2 = Adafruit_7segment();

///////////////////////////////////////////////////////////
/////////////// FACE + ORIENTATION BIT ////////////////////
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
  
  int passive_ID = 90;
  int passive_A  = 180;
  int passive_B  = 0;

///////////////////
int mode_switch = 0;

int mag1 = 0x42;
int mag2 = 0x43;
int angle1 = 0;
int angle2 = 0;
int agc1   = 0;
int agc2   = 0;

float AMS5048_scaling_factor = 45.5111;

void setup()
{
  
  Wire.begin(2,14);
  Serial.begin(115200);
  pinMode(LED,    OUTPUT);
  pinMode(Switch, INPUT);
  delay(50);
  matrix1.begin(0x71);
  matrix2.begin(0x70);
}

 void loop()
{
  mode_switch = digitalRead(Switch);
  delay(20);
  agc1   = read_intensity(mag1);
  delay(20);
  angle1 = round(read_angle(mag1)/AMS5048_scaling_factor);
  delay(20);
  agc2   = read_intensity(mag2);
  delay(20);
  angle2 = round(read_angle(mag2)/AMS5048_scaling_factor);
  delay(20);
  if(mode_switch > 0) // JUST SHOW NUMBERS
    {

    if(agc1 < 255 && agc1 > 0)
      {
      matrix1.blinkRate(0);
      matrix1.print(angle1);
      matrix1.writeDisplay();
      }
    else
      {
      matrix1.blinkRate(1);
      matrix1.print(angle1);
      matrix1.writeDisplay();
      }

    if(agc2 < 255 && agc2 > 0)
      {
      matrix2.blinkRate(0);
      matrix2.print(angle2);
      matrix2.writeDisplay();
      }
    else
      {
      matrix2.blinkRate(1);
      matrix2.print(angle2);
      matrix2.writeDisplay();
      }
    delay(50);
    }

  
  else // CUBE NUMBER AND FACE NUMBER
  {

//            if (mag1 == 0)                      {mag_digit_1 = 0;}
//   else if (angle1 < 6 || angle1 > 354)     {mag_digit_1 = 1;}
//   else                                     {mag_digit_1 = int(angle1 + 18)/12;}
//
//        if (mag2 == 0)                      {mag_digit_2 = 0;}
//   else if(angle2 < 6 || angle2 > 354)      {mag_digit_2 = 1;}  
//   else                                     {mag_digit_2 = int(angle2 + 18)/12;}
//   
  }
}
/*
  int reading_1 = 0;
  int read_id = 0;
  int read_face = 0;
  int reading_2 = 0;
  int orientation = 0;
  Serial.println("BRO");
  if(
     (read_intensity(mag1) < 255 && read_intensity(mag1) > 0) 
  || (read_intensity(mag2) < 255 && read_intensity(mag2) > 0)
    )
  {
    
 ////////////////////////// Determine if Cube is present - and if so its ID#
if((reading_1 >= 0 && reading_1 < 15) || (reading_2 >= 0 && reading_2 < 15) || (reading_1 > 355 && reading_1 < 361) 
|| (reading_2 > 355 && reading_2 < 361)) {read_id = 1;}

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
*/

int read_intensity(int address)
// AGC is the "strength" of the magnet returned as an 8-bit number, 
// 255 = magnet field is too weak, 0 = very strong magnetic field.
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

