#ifndef CUBE_H
#define CUBE_H

#include "defines.h"
#include "CBuff.h"
#include "Face.h"
#include <ArduinoHardware.h>

class Cube
/*
 * Cube class is the object for the cube itself
 * 
 */
{
  private:
    // General information
    int cubeID = 1;
    // long cubeMAC = ESP.getChipId();  //esp.getchipID();
    // ESP.getChipId();
    //long msg_id   = system_get_chip_id();

    int batteryVoltage = 0;
    
    // Status Variables
    int currentPlane; // int either 
    int topFace;      // face this is point upwards, as determined by the accelerometer
    int forwardFace;
    int reverseFace;
    
    // Sensor Addresses
    const int frameMPUaddress = 0x69;
    const int coreMPUaddress  = 0x68;
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
    
  public:
    bool updateFrameMPU();
    bool updateCoreMPU();
    bool wakeMPU(int i2cAddress);
    
    //     
    void clearRGB();
    int lightFace(int face, bool r, bool g, bool  b);
    
    bool determineIfLatticeConnected();  
    
    bool determineCurrentPlane();       // updates variable this->CurrentPlane
    int  returnCurrentPlane();
    
    bool determineForwardFace();        // updates variable this->ForwardFace
    int returnForwardFace();
    
    bool determineTopFace(int threshold);             // updates variable this->UpFace
    int returnTopFace();
    
    void goToSleep();                   // Turns off the entire cube
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
    //
    Face faces[6];
    Cube();
};

#endif
