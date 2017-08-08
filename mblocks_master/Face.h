#ifndef FACE_H
#define FACE_H

#include "defines.h"
#include "CBuff.h"
#include "Cube.h"
#include <ArduinoHardware.h>


class Face
{ 
  private:
    // Cube attributes
    byte IOExpanderState[2];
    int faceVersion;
    const int ambientSensorAddress = 0x39;
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
    // Pins on PCF7585 IO expander on each face.
    const int r_0 = 13;
    const int g_0 = 11;
    const int b_0 = 12;
    const int r_1 = 9;
    const int g_1 = 8;
    const int b_1 = 10;
    
    const int led_A = 7;
    const int led_B = 5;
    const int led_C = 15;
    const int led_D = 2;
    const int EN1 = 3;
    const int EN2 = 4;
    //
    int IOExpanderAddress;
    Face();
    Face(int expanderAddress, int version);
    bool clearRGB();
    bool enableSensors();
    bool disableSensors();
    bool updateAmbient();
    bool updateIOExpander();
    bool setPinLow(int pin);
    bool setPinHigh(int pin);
    int returnAmbientValue(int index);

    void setIOExpanderAddress(int a) {this->IOExpanderAddress = a;}
};

#endif
