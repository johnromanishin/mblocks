// We want to implement classes for ease of reference as we construct and modify code.
// The classes we define here are the cube, the face, and the sensor.
#include "defines.h"
#include "classDefinitions.h"
#include "sensation.h"
#include <Wire.h> 
#include <ArduinoHardware.h>

Face::Face()
  : ambientBuffer(ARRAY_SIZEOF(this->ambientData), this->ambientData)
{}


bool Face::updateAmbient()
{
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
}

int Face::returnAmbientValue(int index)
{
  return(this->ambientBuffer.access(index));
}



//
//class Sensor
//{
//  public:
//  private:
//};
//

