// We want to implement classes for ease of reference as we construct and modify code.
// The classes we define here are the cube, the face, and the sensor.
#include "defines.h"
#include "classDefinitions.h"
#include "sensation.h"
#include <Wire.h> 
#include <ArduinoHardware.h>

Cube::Cube()
{
  
  for(int i = 0; i < ARRAY_SIZEOF(this->faces); i++)
  {
     this->faces[i].setIOExpanderAddress(this->faceExpanderAddresses[i]);
  }
}

Face::Face()
  : ambientBuffer(ARRAY_SIZEOF(this->ambientData), this->ambientData),
    magnetBuffer_A(ARRAY_SIZEOF(this->magnetData_A), this->magnetData_A),
    magnetBuffer_B(ARRAY_SIZEOF(this->magnetData_B), this->magnetData_B)
{
  this->faceVersion = 1;
  this->IOExpanderState[0] = (this->IOExpanderState[1] = byte(0xff));
}

Face::Face(int IOExpanderAddress, int faceVersion = 1)
  : ambientBuffer(ARRAY_SIZEOF(this->ambientData), this->ambientData),
    magnetBuffer_A(ARRAY_SIZEOF(this->magnetData_A), this->magnetData_A),
    magnetBuffer_B(ARRAY_SIZEOF(this->magnetData_B), this->magnetData_B)
{
  this->IOExpanderAddress = IOExpanderAddress;
  this->faceVersion = faceVersion;
  this->IOExpanderState[0] = (this->IOExpanderState[1] = byte(0xff));
}

bool Face::updateIOExpander()
{
  Wire.beginTransmission(this->IOExpanderAddress);
  Wire.write(this->IOExpanderState[0]);
  Wire.write(this->IOExpanderState[1]);
  Wire.endTransmission();
}

bool Face::updateAmbient()
{
  activateLightSensor(this->ambientSensorAddress);
  delay(20);
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
}

int Face::returnAmbientValue(int index)
{
  return(this->ambientBuffer.access(index));
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

bool Face::turnOnLedA()
{
  this->IOExpanderState[0] &= ~(1 << FB_LEDA);
  return this->updateIOExpander();
}

bool Face::turnOffLedA()
{
  this->IOExpanderState[0] |= (1 << FB_LEDA);
  return this->updateIOExpander();
}

bool Face::enableSensors()
{
  this->IOExpanderState[0] &= ~(1 << FB_EN1);
  return this->updateIOExpander();
}

bool Face::disableSensors()
{
  this->IOExpanderState[0] |= (1 << FB_EN1);
  return this->updateIOExpander();
}

//
//class Sensor
//{
//  public:
//  private:
//};
//

