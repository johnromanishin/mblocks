// We want to implement classes for ease of reference as we construct and modify code.
// The classes we define here are the cube, the face.
#include "Defines.h"
#include "Cube.h"
#include "Sensation.h"
#include "Face.h"
#include <Wire.h> 

Face::Face()
  : ambientBuffer(ARRAY_SIZEOF(this->ambientData), this->ambientData),
    magnetAngleBuffer_A(ARRAY_SIZEOF(this->magnetAngleData_A), this->magnetAngleData_A),
    magnetStrengthBuffer_A(ARRAY_SIZEOF(this->magnetStrengthData_A), this->magnetStrengthData_A),
    
    magnetAngleBuffer_B(ARRAY_SIZEOF(this->magnetAngleData_A), this->magnetAngleData_A),
    magnetStrengthBuffer_B(ARRAY_SIZEOF(this->magnetStrengthData_B), this->magnetStrengthData_B),
    
    neighborIDBuffer(ARRAY_SIZEOF(this->neighborIDData), this->neighborIDData),
    neighborInfoBuffer(ARRAY_SIZEOF(this->neighborInfoData), this->neighborInfoData)
    
{
//  this->faceVersion = faceVersion;
  this->IOExpanderState[0] = (this->IOExpanderState[1] = byte(0xff)); // sets bytes of IO expander to be all 1's
}

//Face::Face(int IOExpanderAddress);//, int faceVersion = 1)
//  : ambientBuffer(ARRAY_SIZEOF(this->ambientData), this->ambientData),
//    magnetBuffer_A(ARRAY_SIZEOF(this->magnetData_A), this->magnetData_A),
//    magnetBuffer_B(ARRAY_SIZEOF(this->magnetData_B), this->magnetData_B),
//    
//    neighborIDBuffer(ARRAY_SIZEOF(this->neighborIDData), this->neighborIDData),
//    neighborFaceBuffer(ARRAY_SIZEOF(this->neighborFaceData), this->neighborFaceData),
//    neighborAttributeBuffer(ARRAY_SIZEOF(this->neighborAttributeData), this->neighborAttributeData)
//{
//  this->IOExpanderAddress = IOExpanderAddress;
//  //this->faceVersion = faceVersion;
//  this->IOExpanderState[0] = (this->IOExpanderState[1] = byte(0xff));
//}

bool Face::updateFace()
{
  bool success = 
      (this->enableSensors() 
    && this->turnOnFaceLEDs(1,0,0,0)
    && this->updateAmbient()
    && this->updateMagneticBarcode()
    && this->turnOffFaceLEDs()
    && this->disableSensors()); 
  return(success);
}


bool Face::updateIOExpander()
{
  bool success;
  Wire.beginTransmission(this->IOExpanderAddress);
  Wire.write(this->IOExpanderState[0]);
  Wire.write(this->IOExpanderState[1]);
  if(Wire.endTransmission() == 0){return(true);}
  else                           {return(false);}
}

bool Face::updateAmbient()
{
  /*
   * This function reads the ambient sensor on this instance of "Face" 
   * There is the standard version (When faceVersion == 1), and a
   * legacy version (when faceVerstion ==0);
   */
  if(faceVersion == 0) // Alternate method for Old Face Version
  {
    int final_reading = 0;
    int lightSensorGain = 10;
    for(int i = 0; i < 3; i++)
    {
      int reading = 0;
      delay(6);
      Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset); 
      Wire.write(byte(0x10)); // this is the register where the ambient values are stored
      Wire.endTransmission();
      Wire.requestFrom((this->IOExpanderAddress - this->versionOffset), 2);
      if (2 <= Wire.available()) //ambientLight  = twiBuf[0] << 2;
      {
        reading =  Wire.read()<<1;     //  ambientLight |= twiBuf[1] >> 6;
        reading |=  Wire.read()>>7;
      }
      else  {return(false);}
      final_reading += reading*lightSensorGain;
    }
    this->ambientBuffer.push(final_reading);
    return(true);
  }
  
  //// Code runs for Regular faceVersion
  activateLightSensor(this->ambientSensorAddress);
  delay(15);
  int reading = 0;
  Wire.beginTransmission(this->ambientSensorAddress); 
  Wire.write(byte(0x8C)); // this is the register where the Ambient values are stored
  Wire.endTransmission();
  Wire.requestFrom(this->ambientSensorAddress, 2);
  if (2 <= Wire.available()) 
  {
    reading = Wire.read();
    reading |= Wire.read()<<8;
  }
  this->ambientBuffer.push(reading);
  return(true);
}

int Face::returnAmbientValue(int index)
{
  return(this->ambientBuffer.access(index));
}

int Face::returnMagnetStrength_A(int index)
{
  return(this->magnetStrengthBuffer_A.access(index));
}

int Face::returnMagnetAngle_A(int index)
{
  return(this->magnetAngleBuffer_A.access(index));
}

int Face::returnMagnetStrength_B(int index)
{
  return(this->magnetStrengthBuffer_B.access(index));
}

int Face::returnMagnetAngle_B(int index)
{
  return(this->magnetAngleBuffer_B.access(index));
}

/**       V 
 *   0111 1010
 * & 1111 0111  <--- how do we generate this one?
 * ------------
 *   0111 0010
 *   
 *   (1 << 3) = 
 *   0000 1000  then we bitwise NOT
 *   1111 0111  And this is what we need to bitwise AND
 */ 

bool Face::setPinLow(int pin)
{
  if((pin < 8) && (pin >= 0))
  {
    this->IOExpanderState[0] &= ~(1 << pin);
  }
  else if ((pin > 7) && (pin < 16))
  {
    this->IOExpanderState[1] &= ~(1 << (pin - 8));
  }
}

bool Face::setPinHigh(int pin)
{
    if(pin < 8 && pin >= 0)
  {
    this->IOExpanderState[0] |= (1 << pin);
  }
  else if (pin > 7 && pin < 16)
  {
    this->IOExpanderState[1] |= (1 << (pin - 8));
  }
  
  //return this->updateIOExpander();
}

//***
bool Face::updateMagneticBarcode()
{
    this->getMagnetEncoderAddresses(this->faceMagnetAddresses);
    int angle_A  = readMagnetSensorAngle(faceMagnetAddresses[0])/45.5;
    int mag_A    = readMagnetSensorFieldStrength(faceMagnetAddresses[0]);//(this->getMagnetEncoderAddresses(this->*faceMagnetAddresses))[0]);
    int angle_B  = readMagnetSensorAngle(faceMagnetAddresses[1]);//(this->getMagnetEncoderAddresses(this->*faceMagnetAddresses)[1]))/45.5;
    int mag_B    = readMagnetSensorFieldStrength(faceMagnetAddresses[1]);//(this->getMagnetEncoderAddresses(this->*faceMagnetAddresses))[1]);
    if((mag_A == 0) || (mag_B == 0)) {return(false);}

    //if(
    this->magnetAngleBuffer_A.push(angle_A);
    this->magnetStrengthBuffer_A.push(mag_A);
   
    this->magnetAngleBuffer_B.push(angle_B);
    this->magnetStrengthBuffer_B.push(mag_B);
   
//        if (mag_A == 0)                     {mag_digit_1 = 0;}
//   else if (angle_A < 6 || angle_A > 354)   {mag_digit_1 = 1;}
//   else                                     {mag_digit_1 = int(angle1 + 18)/12;}
//
//  
//        if (mag2 == 0)                      {mag_digit_2 = 0;}
//   else if(angle2 < 6 || angle2 > 354)      {mag_digit_2 = 1;}  
//   else                                     {mag_digit_2 = int(angle2 + 18)/12;}
//   
  return(true);
}


bool Face::enableSensors()
{
    /*
   * 
   */
  if(faceVersion == 0) // Alternate method for Old Face Version
  {
    Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset); // this goes from 0x20 to 0x01 addresses
    Wire.write(byte(0x43)); // access FBRXEN register
    Wire.write(byte(0x01)); // sets FBRXEN register to ACTIVE
    Wire.endTransmission(); 
    this->turnOnFaceLEDs(1,0,1,0);
  return(true);
  }
  
  //// Code runs for Regular faceVersion
  this->IOExpanderState[0] &= ~(1 << FB_EN1);
  return this->updateIOExpander();
}

bool Face::turnOffFaceLEDs()
{
 /* Turns on the four white LED's on each face
   * (bool LED_A = true, bool LED_B = true, bool LED_C = true, bool LED_D = true);
   */
   if(faceVersion == 0) // Alternate method for Old Face Version
  {   
      Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset);
      Wire.write(byte(0x20)); // register address for IRled's
      Wire.write(byte(0x00));
      Wire.endTransmission();
      return(true);
  }
  this->setPinHigh(this->led_A);
  this->setPinHigh(this->led_B);
  this->setPinHigh(this->led_C);
  this->setPinHigh(this->led_D);
  return(this->updateIOExpander());
}

bool Face::turnOnFaceLEDs(bool LED_A, bool LED_B, bool LED_C, bool LED_D)
{
  /* Turns on the four white LED's on each face
   * (bool LED_A = true, bool LED_B = true, bool LED_C = true, bool LED_D = true);
   */
   if(faceVersion == 0) // Alternate method for Old Face Version
   {   
      int dataByte;
      dataByte  = LED_A ? 0x01 : 0x00;
      dataByte |= LED_B ? 0x02 : 0x00;
      dataByte |= LED_C ? 0x04 : 0x00;
      dataByte |= LED_D ? 0x08 : 0x00;
      Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset);
      Wire.write(byte(0x20)); // register address for IRled's
      Wire.write(byte(dataByte));
      Wire.endTransmission();
      return(true);
  }
  if(LED_A) {this->setPinLow(this->led_A);}
  if(LED_B) {this->setPinLow(this->led_B);}
  if(LED_C) {this->setPinLow(this->led_C);}
  if(LED_D) {this->setPinLow(this->led_D);}
  return this->updateIOExpander();
}

bool Face::disableSensors()
{
   /*
   * 
   */
  if(faceVersion == 0) // Alternate method for Old Face Version
  {
    Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset); // this goes from 0x20 to 0x01 addresses
    Wire.write(byte(0x43)); // access FBRXEN register
    Wire.write(byte(0x00)); // sets FBRXEN register to ACTIVE
    Wire.endTransmission(); 
    this->turnOffFaceLEDs();
    return(true);
  }
  
  //// Code runs for Regular faceVersion
  
  this->IOExpanderState[0] |= (1 << FB_EN1);
  return(this->updateIOExpander());
}

/*

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

void process_5048()
{
    {
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
}

*/
