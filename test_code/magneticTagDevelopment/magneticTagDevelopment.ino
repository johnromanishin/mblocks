#define Switch 12
#define LED 13
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix1 = Adafruit_7segment();
Adafruit_7segment matrix2 = Adafruit_7segment();

///////////////////////////////////////////////////////////
/////////////// FACE + ORIENTATION BIT ////////////////////
  int face1A = 29; // Magnet centerpoint for face 1 magnet position a
  int face1B = 28;
  int face2A = 27;
  int face2B = 26;
  int face3A = 25;
  int face3B = 24;
  int face4A = 23;
  int face4B = 22; 
  int face5A = 21;
  int face5B = 20;
  int face6A = 19;
  int face6B = 18; 
  
  int passiveID = 90;
  int passiveA  = 180;
  int passiveB  = 0;

  int tag1 = 6;
  int tag2 = 5;


///////////////////
String tagType = "nothing";
int    tagID   = 0;
int    tagFace = 0;
int    tagAngle = 0;

int mode_switch = 0;
int mag1 = 0x43;
int mag2 = 0x42;
int angle1 = 0;
int angle2 = 0;
int agc1   = 0;
int agc2   = 0;
int magDigit1 = 0;
int magDigit2 = 0;

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
  
        if (agc1 == 0 || agc1 == 255)       {magDigit1 = 0;}
   else if (angle1 < 6 || angle1 > 354)     {magDigit1 = 1;}
   else                                     {magDigit1 = int(angle1 + 18)/12;}

        if (agc2 == 0 || agc2 == 255)       {magDigit2 = 0;}
   else if(angle2 < 6 || angle2 > 354)      {magDigit2 = 1;}  
   else                                     {magDigit2 = int(angle2 + 18)/12;}

  analyzeTag(magDigit1, magDigit2, angle1, angle2, agc1, agc2);
  
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
    
    if(agc1 < 255 && agc1 > 0)
      {
      matrix1.blinkRate(0);
      matrix1.print(magDigit1);
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
      matrix2.print(magDigit2);
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
}

String analyzeTag(int magDigit1, int magDigit2, int angle1, int angle2, int agc1, int agc2)
{
  String abc = "NothingBurger...";
/*
 * CHECK IF TAG REPRESENTS A MODULE
 */
  if((magDigit1 >= 17 && magDigit1 <= 29) &&  // Means magdigit1 is a faceID
     (magDigit2 >= 1 && magDigit2 <= 17))     // Means magdifit2 stores an ID # 
    {
      if(magDigit1 % 2 ==0){Serial.println("Found an actual Cube, **1** ");delay(1000);}
      else                 {Serial.println("Found an actual Cube, **2** ");delay(1000);}
    }
    
  if((magDigit2 >= 17 && magDigit2 <= 29) &&  // Means magdigit1 is a faceID
     (magDigit1 >= 1 && magDigit1 <= 17))     // Means magdifit2 stores an ID # 
    {
      if(magDigit2 % 2 ==0){Serial.println("Found an actual Cube, **3** ");delay(1000);}
      else                 {Serial.println("Found an actual Cube, **4** ");delay(1000);}
    }

/*
 * CHECK IF TAG REPRESENTS A PASSIVE MODULE
 */
 
       if((magDigit1 == 15 || magDigit1 == 16 || magDigit1 == 17  // Means magdigit1 is a faceID
        || magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2 ) &&
          (magDigit2 == 8  || magDigit2 == 9  || magDigit2 == 10))     // Means magdifit2 stores an ID # 
          {
              if(magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2)
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **3** WOOO!");delay(1000);
                  }
              else 
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **4** WOOO!");delay(1000);
                  }
          }
       if((magDigit2 == 15 || magDigit2 == 16 || magDigit2 == 17  // Means magdigit1 is a faceID
        || magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2 ) &&
          (magDigit1 == 8  || magDigit1 == 9  || magDigit1 == 10))     // Means magdifit2 stores an ID # 
          {
              if(magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2)
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **1** WOOO!");delay(1000);
                  }
              else 
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **2** WOOO!");delay(1000);
                  }
          }

/*
 * CHECK IF TAG REPRESENTS A COMMAND TAG
 */
  if((magDigit1 == magDigit2) &&  // Means magdigit1 is a faceID
     (magDigit1 != 17 && magDigit2 != 17) &&
     (magDigit1 != 30 && magDigit2 != 30)
     )     // Means magdifit2 stores an ID # 
    {
      if(magDigit1 == 25)
        {Serial.println("Found an COMMAND TAG... Go to SLEEP! SLEEPY TIME ! WOO MUCH SLEEP ");delay(1000);}
    }

    
  String tagType = "nothing";
  int    tagID   = 0;
  int    tagFace = 0;
  int    tagAngle = 0;
  
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

