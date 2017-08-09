#ifndef CUBE_H
#define CUBE_H

#include "defines.h"
#include "CBuff.h"
#include "Face.h"
#include <ArduinoHardware.h>

class Cube
/*
 * Cube class is the object for the cube itself
 */
{
  private:
      // General information
    int cubeID = 1;
    int batteryVoltage = 0;
    int currentPlane; // The plane (out of three) that the central actuator is in - 0
    int coreAngle;    // current angle of the core 
    int topFace;      // face this is point upwards, as determined by the accelerometer
    int forwardFace;  // face that is pointing forwards
    int reverseFace;  // face that is pointing backwards
    
      // i2c Addresses
    const int frameIMUaddress = 0x69;
    const int coreIMUaddress  = 0x68;
    const int coreMagnetSensorAddress = 0x40;//
    const int faceExpanderAddresses[6] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25};
        
      // Data storage spaces
    int axCoreData[32];
    int ayCoreData[32];
    int azCoreData[32];
    int gxCoreData[32];
    int gyCoreData[32];
    int gzCoreData[32];

    int axFrameData[32];
    int ayFrameData[32];
    int azFrameData[32];
    int gxFrameData[32];
    int gyFrameData[32];
    int gzFrameData[32];

    int coreMagnetAngleData[32];
    int coreMagnetStrengthData[32];

      // Internal functions
    bool CornerRGB(int face, bool top, bool r, bool g, bool b); // Only for Version 0;

  public:
      // Public Variables
    long shutDownTime = (60000*10); // time until board goes to sleep
    
      // Functions involving SENSORS
    bool updateSensors(); // Updates almost everything on the cube...
    bool updateFaces(); // Updates all of the face sensors on all faces
    bool updateBothIMUs(); // updates BOTH IMU's
    bool updateFrameIMU();
    bool updateCoreIMU();
    bool updateCoreMagnetSensor();
    bool wakeIMU(int i2cAddress);
    
    // Functions involving LED's
    bool clearRGB(); // Turns off all LED's on the cube DUAL VERSIONS
    bool lightFace(int face, bool r = true, bool g = false, bool  b = true); //DUAL VERSIONS
    bool lightCube(bool r = true, bool g = true, bool b = false); // lights entire cube, defaults to yellow
    
    // Functions 
    bool determineIfLatticeConnected();  
    
    bool determineCurrentPlane();       // updates variable this->CurrentPlane
    int  returnCurrentPlane();
    
    bool determineForwardFace();        // updates variable this->ForwardFace
    int returnForwardFace();
    
    bool determineTopFace(int threshold = 13000);             // updates variable this->UpFace
    int returnTopFace();
    
    void shutDown();                   // Turns off the entire cube
    //
    long cubeMAC = ESP.getChipId(); 
    
    //
    CircularBuffer<int> axCoreBuffer;
    CircularBuffer<int> ayCoreBuffer;
    CircularBuffer<int> azCoreBuffer;
    CircularBuffer<int> gxCoreBuffer;
    CircularBuffer<int> gyCoreBuffer;
    CircularBuffer<int> gzCoreBuffer;

    CircularBuffer<int> axFrameBuffer;
    CircularBuffer<int> ayFrameBuffer;
    CircularBuffer<int> azFrameBuffer;
    CircularBuffer<int> gxFrameBuffer;
    CircularBuffer<int> gyFrameBuffer;
    CircularBuffer<int> gzFrameBuffer;

    CircularBuffer<int> coreMagnetAngleBuffer;
    CircularBuffer<int> coreMagnetStrengthBuffer;
    //
    Face faces[6];
    Cube();
};

#endif
