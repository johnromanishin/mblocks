#ifndef FACE_H
#define FACE_H

#include "Defines.h"
#include "CBuff.h"
#include "Cube.h"
#include <Arduino.h>

//typedef enum TagCommand
//{
//  TAGCOMMAND_NONE,
//  TAGCOMMAND_SLEEP,
//  TAGCOMMAND_PURPLE,
//  TAGCOMMAND_27,
//  TAGCOMMAND_25,
//  TAGCOMMAND_23,
//  TAGCOMMAND_21,
//  TAGCOMMAND_19,
//} TagCommand;
//
//typedef enum TagType
//{
//  TAGTYPE_NOTHING,
//  TAGTYPE_INVALID,
//  TAGTYPE_REGULAR_CUBE,
//  TAGTYPE_PASSIVE_CUBE,
//  TAGTYPE_COMMAND
//} TagType;

//typedef struct Tag
//{
//  TagType type;   //
//  int angle ; // Either -1, 0, 1, 2 or 3 - corresponding to 90 deg angle
//  int id; // ID or message code attached to tag
//  int face; // face number (0,1,2,3,4,5) associated with a cube
//  int strength; // Validitity of the tag, basically just agc1+agc2
//  TagCommand command; // Text of command or behavior to go to... if it exists
//} Tag;

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
    int reflectivityData[8];
    CircularBuffer<int> ambientBuffer;
    CircularBuffer<int> reflectivityBuffer;

      // Magnetic data Buffers
    int magnetAngleData_A[10];
    int magnetStrengthData_A[10];
    int magnetAngleData_B[10];
    int magnetStrengthData_B[10];
    
    CircularBuffer<int> magnetAngleBuffer_B;
    CircularBuffer<int> magnetStrengthBuffer_B;
    CircularBuffer<int> magnetAngleBuffer_A;
    CircularBuffer<int> magnetStrengthBuffer_A;
    
      // Neighbor Information Buffers
    int neighborData[8];
    TagType     neighborTypeData[10];
    TagCommand  neighborCommandData[10];
    int neighborIDData[10];
    int neighborFaceData[10];
    int neighborAngleData[10];
    bool neighborPresenceData[10];
    int neighborLightDigitData[10];
    
      // Circular Buffers for Magnetic Tag Variables
    CircularBuffer<TagType>     neighborTypeBuffer;
    CircularBuffer<TagCommand>  neighborCommandBuffer;
    CircularBuffer<int>         neighborIDBuffer;
    CircularBuffer<int>         neighborFaceBuffer;
    CircularBuffer<int>         neighborAngleBuffer;
    CircularBuffer<bool>        neighborPresenceBuffer;
    CircularBuffer<int>         neighborLightDigitBuffer;
    
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
    bool turnOnFaceLEDs(bool LED_A = true, bool LED_B = true, bool LED_C = true, bool LED_D = true); 
    bool turnOffFaceLEDs();
    void blinkRing(int delayLength = 100, int numberOfTimes = 6);

      // Sensor Related Commands
    bool enableSensors();   
    bool disableSensors();  
    bool updateAmbient(bool activate = true);   
    bool readAmbient(bool activate = true);
    //bool updateReflectivity();
    bool updateMagneticBarcode(); // Reads both magnet sensors,
    bool isThereNeighbor(); // true == yes! // false == NO
    bool updateFace(int lightDigit, bool checkForLightYo, bool blinkLEDs = true); // Enables sensors, Updates ambient values, updates magnetic sensors, neighbors...
      // Sending simple messages - length of blink = digit... 100ms == "1" 200ms == "2" ... etc.
    int checkForMessage(int lightDigit, int waitTime);
    void blinkOutMessage(int digit); 

      // Magnetic Tag Related Commands
    bool processTag();
    
      // Return from Circular Buffer Commands
    int returnMagnetAngle_A(int index);
    int returnMagnetAngle_B(int index);
    int returnMagnetStrength_A(int index);
    int returnMagnetStrength_B(int index); 
    
    int returnAmbientValue(int index);
    void forceUpdateAmbientValue(int value); // used to simplify sorting...
    int returnReflectivityValue(int index);
    //
    bool returnNeighborPresence(int index);
    int returnNeighborID(int index);
    int returnNeighborAngle(int index);
    int returnNeighborFace(int index);
    TagType returnNeighborType(int index);
    TagCommand returnNeighborCommand(int index);
    int returnNeighborLightDigit(int index);
};

void activateLightSensor(int address);
int readMagnetSensorAngle(int i2cAddress);
int readMagnetSensorFieldStrength(int i2cAddress);
int magnetSensorRead(int i2cAddress, byte dataRegisterAddress);

typedef struct Tag
{
  TagType type;   
  /* 0 = not a valid tag
   * 1 = Regular Cube Attached
   * 2 = Passive Cube Attached
   * 3 = COMMAND tag
   */ 
  int angle ; // Either -1, 0, 1, 2 or 3 - corresponding to 90 deg angle
  int id; // ID or message code attached to tag
  int face; // face number (0,1,2,3,4,5) associated with a cube
  int strength; // Validitity of the tag, basically just agc1+agc2
  TagCommand command; // Text of command or behavior to go to... if it exists
} Tag;

void analyzeTag(int angle1, int agc1, int angle2, int agc2, Tag*);
int returnFaceNumber(int magDigit);

#endif
