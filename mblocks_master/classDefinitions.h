// We want to implement classes for ease of reference as we construct and modify code.
// The classes we define here are the cube, the face, and the sensor.
#include "defines.h"
#include "CircularBuffer.h"
#include <ArduinoHardware.h>

class Cube
{
  private:
  public:
};

class Face
{ 
  private:
    const int IOexpanderAddress = 0x21;
    const int ambientSensorAddress = 0x39;
    int ambientData[128];
    CircularBuffer<int> ambientBuffer;
    
  public:
    Face();
    void updateAmbient();
    int returnAmbientValue(int index);
};



//
//class Sensor
//{
//  public:
//  private:
//};
//

