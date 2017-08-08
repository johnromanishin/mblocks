// We want to implement classes for ease of reference as we construct and modify code.
// The classes we define here are the cube, the face.
#include "defines.h"
#include "Cube.h"
#include "sensation.h"
#include "Face.h"
#include <Wire.h> 
#include <ArduinoHardware.h>

Cube::Cube()
  : axFrameBuffer(ARRAY_SIZEOF(this->axFrameData), this->axFrameData),
    ayFrameBuffer(ARRAY_SIZEOF(this->ayFrameData), this->ayFrameData),
    azFrameBuffer(ARRAY_SIZEOF(this->azFrameData), this->azFrameData),
    gxFrameBuffer(ARRAY_SIZEOF(this->gxFrameData), this->gxFrameData),
    gyFrameBuffer(ARRAY_SIZEOF(this->gyFrameData), this->gyFrameData),
    gzFrameBuffer(ARRAY_SIZEOF(this->gzFrameData), this->gzFrameData),

    axCoreBuffer(ARRAY_SIZEOF(this->axCoreData), this->axCoreData),
    ayCoreBuffer(ARRAY_SIZEOF(this->ayCoreData), this->ayCoreData),
    azCoreBuffer(ARRAY_SIZEOF(this->azCoreData), this->azCoreData),
    gxCoreBuffer(ARRAY_SIZEOF(this->gxCoreData), this->gxCoreData),
    gyCoreBuffer(ARRAY_SIZEOF(this->gyCoreData), this->gyCoreData),
    gzCoreBuffer(ARRAY_SIZEOF(this->gzCoreData), this->gzCoreData)
    
{ 
  for(int i = 0; i < ARRAY_SIZEOF(this->faces); i++)
  {
     this->faces[i].setIOExpanderAddress(this->faceExpanderAddresses[i]);
  }
}

bool Cube::updateFrameMPU()
{
  bool succeed = false; // set a boolean
  int16_t ax, ay, az, Tmp, gx, gy, gz;
  this->wakeMPU(this->frameMPUaddress);
  Wire.beginTransmission(this->frameMPUaddress);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  //succeed = Wire.requestFrom(this->frameMPUaddress, 14, 1); // 7 reads of 2 bytes
  Wire.requestFrom(this->frameMPUaddress, 14, 1); // 7 reads of 2 bytes
  ax = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  ay = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  az = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  gx = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  gy = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  gz = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  this->axFrameBuffer.push(ax);
  this->ayFrameBuffer.push(ay);
  this->azFrameBuffer.push(az);
  this->gxFrameBuffer.push(gx);
  this->gyFrameBuffer.push(gy);
  this->gzFrameBuffer.push(gz);
  return succeed;
}

bool Cube::updateCoreMPU()
{
  bool succeed = false; // set a boolean 
  int16_t ax, ay, az, Tmp, gx, gy, gz;
  this->wakeMPU(this->coreMPUaddress);
  Wire.beginTransmission(this->coreMPUaddress);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  //succeed = Wire.requestFrom(this->coreMPUaddress, 14, 1); // 7 reads of 2 bytes
  Wire.requestFrom(this->coreMPUaddress, 14, 1); // 7 reads of 2 bytes
  ax = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  ay = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  az = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  gx = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  gy = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  gz = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  this->axCoreBuffer.push(ax);
  this->ayCoreBuffer.push(ay);
  this->azCoreBuffer.push(az);
  this->gxCoreBuffer.push(gx);
  this->gyCoreBuffer.push(gy);
  this->gzCoreBuffer.push(gz);
  return succeed;
}


bool Cube::wakeMPU(int i2cAddress)
// Wakes the MPU_6050 accelerometer
{
  Wire.beginTransmission(i2cAddress);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void Cube::goToSleep()
// Turns the entire cube off by printing "sleep" to the slave board
{
  while(1)
  {
    Serial.println("sleep");delay(1000);
  }
}

//bool Cube::determineForwardFace()
//{
////  int face = this->;
////  
////  if      (face == 2 && plane == 1234  ||  face == 6 && plane == 1536)                                 {forward_face = 1;}
////  else if (face == 3 && plane == 1234  ||  face == 5 && plane == 2546)                                 {forward_face = 2;}
////  else if (face == 4 && plane == 1234  ||  face == 5 && plane == 1536)                                 {forward_face = 3;}
////  else if (face == 1 && plane == 1234  ||  face == 6 && plane == 2546)                                 {forward_face = 4;}
////  else if (face == 4 && plane == 2546  ||  face == 1 && plane == 1536)                                 {forward_face = 5;}
////  else if (face == 2 && plane == 2546  ||  face == 3 && plane == 1536)                                 {forward_face = 6;}
////  else if((face == 5 && plane == 1234) || (face == 1 && plane == 2546) || (face == 2 && plane == 1536)){forward_face = 10;}
////  else if((face == 6 && plane == 1234) || (face == 3 && plane == 2546) || (face == 4 && plane == 1536)){forward_face = 11;}
////  else{return(-1);}
////  return(forward_face);
//}


int Cube::lightFace(int face, bool r = true, bool g = false, bool b = true)
{
  /*
   * Lights up a particular face with the color r | g | b
   */
this->clearRGB(); // Sets all RGB bits to HIGH / aka off

switch (face){
    case 0:
      if(r){this->faces[0].setPinLow(this->faces[0].r_0);this->faces[0].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[0].setPinLow(this->faces[0].g_0);this->faces[0].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[0].setPinLow(this->faces[0].b_0);this->faces[0].setPinLow(this->faces[0].b_1);}
      
      if(r){this->faces[1].setPinLow(this->faces[0].r_0);this->faces[1].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[1].setPinLow(this->faces[0].g_0);this->faces[1].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[1].setPinLow(this->faces[0].b_0);this->faces[1].setPinLow(this->faces[0].b_1);}
      break;
    case 1:
      if(r){this->faces[1].setPinLow(this->faces[0].r_0);this->faces[1].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[1].setPinLow(this->faces[0].g_0);this->faces[1].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[1].setPinLow(this->faces[0].b_0);this->faces[1].setPinLow(this->faces[0].b_1);}

      if(r){this->faces[2].setPinLow(this->faces[0].r_0);this->faces[2].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[2].setPinLow(this->faces[0].g_0);this->faces[2].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[2].setPinLow(this->faces[0].b_0);this->faces[2].setPinLow(this->faces[0].b_1);}
      break;
      
    case 2:
      if(r){this->faces[2].setPinLow(this->faces[0].r_0);this->faces[2].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[2].setPinLow(this->faces[0].g_0);this->faces[2].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[2].setPinLow(this->faces[0].b_0);this->faces[2].setPinLow(this->faces[0].b_1);}

      if(r){this->faces[3].setPinLow(this->faces[0].r_0);this->faces[3].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[3].setPinLow(this->faces[0].g_0);this->faces[3].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[3].setPinLow(this->faces[0].b_0);this->faces[3].setPinLow(this->faces[0].b_1);}
      break;
      
    case 3:
      if(r){this->faces[3].setPinLow(this->faces[0].r_0);this->faces[3].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[3].setPinLow(this->faces[0].g_0);this->faces[3].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[3].setPinLow(this->faces[0].b_0);this->faces[3].setPinLow(this->faces[0].b_1);}

      if(r){this->faces[0].setPinLow(this->faces[0].r_0);this->faces[0].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[0].setPinLow(this->faces[0].g_0);this->faces[0].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[0].setPinLow(this->faces[0].b_0);this->faces[0].setPinLow(this->faces[0].b_1);}
      break;
      
    case 4:
      if(r){this->faces[0].setPinLow(this->faces[0].r_1);this->faces[1].setPinLow(this->faces[0].r_1);
            this->faces[2].setPinLow(this->faces[0].r_0);this->faces[3].setPinLow(this->faces[0].r_1);}
      
      if(g){this->faces[0].setPinLow(this->faces[0].g_1);this->faces[1].setPinLow(this->faces[0].g_1);
            this->faces[2].setPinLow(this->faces[0].g_0);this->faces[3].setPinLow(this->faces[0].g_1);}
      
      if(b){this->faces[0].setPinLow(this->faces[0].b_1);this->faces[1].setPinLow(this->faces[0].b_1);
            this->faces[2].setPinLow(this->faces[0].b_0);this->faces[3].setPinLow(this->faces[0].b_1);}
      break;
      
    case 5:
    if(r)
    {
      this->faces[0].setPinLow(this->faces[0].r_0);this->faces[1].setPinLow(this->faces[0].r_0);
      this->faces[2].setPinLow(this->faces[0].r_1);this->faces[3].setPinLow(this->faces[0].r_0);
    }
    if(g)
    {
      this->faces[0].setPinLow(this->faces[0].g_0);this->faces[1].setPinLow(this->faces[0].g_0);
      this->faces[2].setPinLow(this->faces[0].g_1);this->faces[3].setPinLow(this->faces[0].g_0);
    }
    if(b)
    {
      this->faces[0].setPinLow(this->faces[0].b_0);this->faces[1].setPinLow(this->faces[0].b_0);
      this->faces[2].setPinLow(this->faces[0].b_1);this->faces[3].setPinLow(this->faces[0].b_0);
    }
      break;
    }
  for(int i = 0; i< 4; i++)
  {
    this->faces[i].updateIOExpander();
  }
}
    

bool Cube::determineTopFace(int threshold = 12000)
{
  /**  
   *  Each MPU returns value from -16000 to +16000 if cube is stationary
   *  Since the Accelerometer lines up with X Y and Z axes we just check if any of the axes
   *  This function returns an integer representing
   *  the face which is point upwards. Returns -1 if results are inconclusive
   *  c.axFrameBuffer.access(0)
   */
  this->updateFrameMPU();
  int sum = (abs(this->axFrameBuffer.access(0))+abs(this->ayFrameBuffer.access(0))+abs(this->azFrameBuffer.access(0)));
  
       if ((this->azFrameBuffer.access(0) < (-threshold)) && sum < 25000)  { this->topFace = 0; }
  else if ((this->azFrameBuffer.access(0) > (threshold)) && sum < 25000)   { this->topFace = 2; }
  else if ((this->axFrameBuffer.access(0) < (-threshold)) && sum < 25000)  { this->topFace = 5; }
  else if ((this->axFrameBuffer.access(0) > (threshold)) && sum < 25000)   { this->topFace = 4; }
  else if ((this->ayFrameBuffer.access(0) < (-threshold)) && sum < 25000)  { this->topFace = 1; }
  else if ((this->ayFrameBuffer.access(0) > (threshold)) && sum < 25000)   { this->topFace = 3; }
  else                                                                     { this->topFace = -1;}
  return (this->topFace);
  
}

int Cube::returnTopFace()
{
  this->determineTopFace();
  return (this->topFace);
}

void Cube::clearRGB()
/*
 * Switches all of the bits to HIGH (or off) for all 8 corner RGBLED's
 * 
 */
{
  for(int i = 0; i< 4; i++)
    {
      this->faces[i].setPinHigh(this->faces[0].r_0);
      this->faces[i].setPinHigh(this->faces[0].r_1);
      this->faces[i].setPinHigh(this->faces[0].g_0);
      this->faces[i].setPinHigh(this->faces[0].g_1);
      this->faces[i].setPinHigh(this->faces[0].b_0);
      this->faces[i].setPinHigh(this->faces[0].b_1);
    }
}

bool Cube::determineCurrentPlane()
{
  /*
   * 
   */
  this->currentPlane = 0123;
  
}

int Cube::returnCurrentPlane()
{
  /*
   * Current plane is the orientation of the cube with respect to the frame
   * Either 0123, 1435
   */
  this->determineCurrentPlane();
  return(this->currentPlane);
}

bool Cube::determineForwardFace() // FUN3 // plane should be either int 1234, 1536 2546 or -1
{
  /*
   * plane 0321 /0
   * plane 0425 /1
   * plane 1534 /4
   */
  this->determineTopFace(); // Figure out which face is on top
  this->determineCurrentPlane();  // determine which plane the actuator is in
  int face = this->returnTopFace(); // 
  int plane = this->returnCurrentPlane();
  
       if( (face == 1 && plane == 0321)  ||  (face == 5 && plane == 0425) )                               {this->forwardFace = 0;}
  else if( (face == 2 && plane == 0321)  ||  (face == 4 && plane == 1534) )                               {this->forwardFace = 1;}
  else if( (face == 3 && plane == 0321)  ||  (face == 4 && plane == 0425) )                               {this->forwardFace = 2;}
  else if( (face == 0 && plane == 0321)  ||  (face == 5 && plane == 1534) )                               {this->forwardFace = 3;}
  else if( (face == 3 && plane == 1534)  ||  (face == 0 && plane == 0425) )                               {this->forwardFace = 4;}
  else if( (face == 1 && plane == 1534)  ||  (face == 2 && plane == 0425) )                               {this->forwardFace = 5;}
  
  else if( (face == 4 && plane == 0321) || (face == 0 && plane == 1534) || (face == 1 && plane == 0425) ) {this->forwardFace = -1;}
  else if( (face == 5 && plane == 0321) || (face == 2 && plane == 1534) || (face == 3 && plane == 0425) ) {this->forwardFace = -1;}
  else                                                                                                    {return(false);}
  
  return(true);
}

int Cube::returnForwardFace()
/*
 * Returns the face which is forward
 */
{
  return(this->forwardFace);
}
