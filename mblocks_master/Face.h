#ifndef FACE_H
#define FACE_H

#include "Defines.h"
#include "CBuff.h"
#include "Cube.h"
#include <Arduino.h>

class Face
{ 
  private:
    // Cube attributes
    byte IOExpanderState[2];
    int versionOffset = 31; // Adapts the i2c address for faceVersion == 0;
    const int ambientSensorAddress = 0x39; // i2c address for ambient light sensor on face version == 1;
    int faceMagnetAddresses[2];
    
    void getMagnetEncoderAddresses(int* target)
    {
      switch(faceVersion)
      {
        case 1:
          target[0] = 0x43; target[1] = 0x42;
          break;
          
        default:
          target[0] = 0x42; target[1] = 0x43;
          break;
      }
    }
    
      // Data storage spaces
    int ambientData[64];
    CircularBuffer<int> ambientBuffer;

      // Magnetic data Buffers
    int magnetAngleData_A[10];
    int magnetStrengthData_A[10];
    CircularBuffer<int> magnetAngleBuffer_A;
    CircularBuffer<int> magnetStrengthBuffer_A;
    int magnetAngleData_B[10];
    int magnetStrengthData_B[10];
    CircularBuffer<int> magnetAngleBuffer_B;
    CircularBuffer<int> magnetStrengthBuffer_B;

      // Neighbor Information Buffers
    int neighborIDData[10];
    int neighborInfoData[10];
    CircularBuffer<int> neighborIDBuffer;
    CircularBuffer<int> neighborInfoBuffer;
    
  public:
      // Constructors
    Face();   
    Face(int expanderAddress);
    
      // Public Variables
    const int r_0 = 13; // Pins on PCF7585 IO expander on each face.
    const int g_0 = 11; //...
    const int b_0 = 12;
    const int r_1 = 9;
    const int g_1 = 8;
    const int b_1 = 10;
    const int led_A = 7;    // Second byte of pins
    const int led_B = 5;
    const int led_C = 15;
    const int led_D = 2;
    const int EN1 = 3;
    const int EN2 = 4; // ... Pins on PCF7585 IO expander on each face.
    
    int IOExpanderAddress;
    
      // Utility Commands
    bool updateIOExpander();
    bool setPinLow(int pin);
    bool setPinHigh(int pin);
    void setIOExpanderAddress(int a) {this->IOExpanderAddress = a;}

      //LED Related Commands
    bool turnOnFaceLEDs(bool LED_A = true, bool LED_B = true, bool LED_C = true, bool LED_D = true);  //DUAL VERSIONS
    bool turnOffFaceLEDs(); // DUAL Versions

      // Sensor Related Commands
    bool enableSensors();   //DUAL VERSIONS
    bool disableSensors();  //DUAL VERSIONS
    bool updateAmbient();   //DUAL VERSIONS
    bool updateMagneticBarcode(); // Reads both magnet sensors,
    bool updateFace(); // Enables sensors, Updates ambient values, updates magnetic sensors, neighbors...
    
      // Return from Circular Buffer Commands
    int returnMagnetAngle_A(int index);
    int returnMagnetAngle_B(int index);
    int returnMagnetStrength_A(int index);
    int returnMagnetStrength_B(int index); 
    int returnAmbientValue(int index);
    int returnNeighborID(int index);
    int returnNeighborInfo(int index);
};

#endif
