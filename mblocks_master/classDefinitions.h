#ifndef CLASSDEFINITIONS_H
#define CLASSDEFINITIONS_H
// We want to implement classes for ease of reference as we construct and modify code.
// The classes we define here are the cube, the face, and the sensor.
#include "defines.h"
#include "CBuff.h"
#include <Arduino.h>

class Face
{ 
  private:
    // Cube attributes
    byte IOExpanderState[2];
    const int ambientSensorAddress = 0x39;
    int faceVersion;
    void getMagnetEncoderAddresses(int* target)
    {
      switch(this->faceVersion)
      {
        case 1:
          target[0] = 0x43; target[1] = 0x42;
          break;
          
        default:
          target[0] = 0x43; target[1] = 0x42;
          break;
      }
    }
    
    // Data storage spaces
    int ambientData[64];
    int magnetData_A[64];
    int magnetData_B[64];
    CircularBuffer<int> ambientBuffer;
    CircularBuffer<int> magnetBuffer_A;
    CircularBuffer<int> magnetBuffer_B;
    
  public:
      int IOExpanderAddress;
    Face();
    Face(int expanderAddress, int version);
    bool enableSensors();
    bool disableSensors();
    bool updateAmbient();
    bool updateIOExpander();
    bool turnOnLedA();
    bool turnOffLedA();
    int returnAmbientValue(int index);

    void setIOExpanderAddress(int a) {this->IOExpanderAddress = a;}
};


class Cube
{
  private:
    // General information
    int cubeID = 1;
    long cubeMAC = 0;//esp.getchipID();
    int batteryVoltage = 0;
    
    // Status Variables
    int currentPlane;
    int topFace;
    int forwardFace;
    int reverseFace;
    
    // Sensor Addresses
    const int frameMPU = 0x69;
    const int coreMPU  = 0x68;
    const int faceExpanderAddresses[6] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25};

    
  public:
    Face faces[6];

    Cube();
};

//
//class Sensor
//{
//  public:
//  private:
//};
//
#endif
