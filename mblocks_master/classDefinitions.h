// We want to implement classes for ease of reference as we construct and modify code.
// The classes we define here are the cube, the face, and the sensor.
#include "defines.h"
#include "CBuff.h"
#include <ArduinoHardware.h>

class Cube
{
  private:
    int cubeID = 1;
    long cubeMAC = 0;//esp.getchipID();
    const int frameMPU = 0x68;
    const int coreMPU  = 0x69;
  public:
};

class Face
{ 
  private:
    int faceNumber;
    const int IOexpanderAddress = 0x21;
    byte I0expanderByte_0 = byte(11111111);
    byte I0expanderByte_1 = byte(11111111);
    const int ambientSensorAddress = 0x39;
    int faceVersion = 1;
    int magnetEncoderAddress_A = 0x43;
    int magnetEncoderAddress_B = 0x44;
    int ambientData[64];
    int magnetData_A[64];
    int magnetData_B[64];
    CircularBuffer<int> ambientBuffer;
    
  public:
    Face();
    bool updateAmbient();
    int returnAmbientValue(int index);
};



//
//class Sensor
//{
//  public:
//  private:
//};
//

