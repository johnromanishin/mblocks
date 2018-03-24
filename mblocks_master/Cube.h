#ifndef CUBE_H
#define CUBE_H

#include "Defines.h"
#include "CBuff.h"
#include "Face.h"
#include <Arduino.h>

//typedef enum PlaneEnum {PLANE0123, PLANE0425, PLANE1453, PLANENONE, PLANEMOVING, PLANEERROR} PlaneEnum;

class Cube
/*
 * Cube class is the object for the cube itself
 */
{
  private:
    // General information
    int topFace;      // face this is point upwards, as determined by the accelerometer
    int forwardFace;  // face that is pointing forwards
    int reverseFace;  // face that is pointing backwards

    bool planeBitLocked = false;
      // i2c Addresses
    const int frameIMUaddress = 0x69;
    const int coreIMUaddress  = 0x68;
    const int faceExpanderAddresses[6] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25};

      // Data storage spaces

    Behavior behaviorBufferData[10];   // This buffer stores the history of the behaviors
    PlaneEnum currentPlaneBufferData[10]; // this buffer stores the history of which plane we are in
    bool moveSuccessBufferData[10];       // this buffer stores a history of moves and if they worked or not
    int topFaceBufferData[10];

    int axCoreData[10];
    int ayCoreData[10];
    int azCoreData[10];
    int gxCoreData[10];
    int gyCoreData[10];
    int gzCoreData[10];

    int axFrameData[10];
    int ayFrameData[10];
    int azFrameData[10];
    int gxFrameData[10];
    int gyFrameData[10];
    int gzFrameData[10];

      // Internal functions
    //bool CornerRGB(int face, bool top, bool r, bool g, bool b); // Only for Version 0;
    bool determineForwardFace();        // updates variable this->ForwardFace
    bool determineTopFace(int threshold = 12500);             // updates variable this->UpFace

  public:
      // Public Variables
    int cubeID = 0;

    // Functions
    void updateCubeID(int idNUM);
    void disconnectI2C();
    void reconnectI2C();
    void resetI2C();
    void blinkParasiteLED(int blinkTime = 100);
    int wifiDelayWithMotionDetection(int delayTime);
    int shutDownTime = (60000*10); // time until board goes to sleep

      // Update Functions involving SENSORS
//************* MOST IMPORTANT FUNCTION ****************************
    bool update(); // Updates almost everything on the cube...
//*******************************************************************
    int numberOfNeighbors(int index = 0, bool lightFace = true);
    int numberOfNeighborsCheckNow(); //quickly checks the magnetic field sensors to see if tehre are neighbors
    int whichFaceHasNeighbor(int index = 0);
    
    bool updateBothIMUs(); // updates BOTH IMU's
    bool updateFrameIMU();
    bool updateCoreIMU();
    bool wakeIMU(int i2cAddress);
    
    int returnXthBrightestFace(int index, bool ExcludeTop);
    int returnSumOfAmbient(); // returns the sum of all of the light sensors
    bool areFacesOpposite(int face1, int face2);
    
        // Functions involving motion
    bool moveIASimple(Motion* motion);
    bool roll(bool forwardReverse, int rpm = 6000);
    bool moveBLDCACCEL(bool forwardReverse, int current, int lengthOfTime);
    
        // Related to the state itself
    bool processState();
    String returnCurrentPlane_STRING();
    int returnForwardFace();
    int returnReverseFace();
    int returnTopFace(int index = 0);
    int returnBottomFace();
    bool isFaceNeitherTopNorBottom(int face);
    String debugAccelerometers();
    bool isValidPlane();
    bool anythingOnSerial(int waitTime);
    bool printString(String stringToPrint, int waitTime = 500);
    
    // Functions involving PLane Changing
    PlaneEnum findPlaneStatus(bool reset = true);
    bool setCorePlaneSimple(PlaneEnum targetCorePlane);
    bool goToPlaneParallel(int faceExclude);
    bool goToPlaneIncludingFaces(int face1, int face2);
    bool isPlaneParallel(int faceExclude);
    void lightPlaneRing(PlaneEnum corePlane);    
    bool isPlaneInPlaneOfFaces(int face1, int face2);
    
      // Functions involving LED's
    bool clearRGB(); // Turns off all LED's on the cube DUAL VERSIONS
    bool lightCorner(int corner, Color* inputColor);
    bool lightFace(int face, Color* inputColor); //DUAL VERSIONS
    bool lightCube(Color* inputColor); // lights entire cube
    bool blockingBlink(Color* inputColor, int howManyTimes = 6, int waitTime = 100);
    void setFaceLEDsAtEdge(int, int); // 
    void superSpecialBlink(Color* inputColor, int delayTime);
    void blinkRingAll(int delayLength = 50, int numberOfTimes = 1);
    
    // Misc. Useful Functions
    void shutDown();                   // Turns off the entire cube
    long cubeMAC = ESP.getChipId();
    //
    //CircularBuffer<Behavior> behaviorBuffer;
    CircularBuffer<PlaneEnum> currentPlaneBuffer;
    CircularBuffer<bool> moveSuccessBuffer;
    CircularBuffer<int> topFaceBuffer;

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

int sortList(int*, int, int);
int oppositeFace(int face);
static int32_t vector_distance_squared(const int32_t* a, const int32_t* b);
static void apply_3x3_mult(const int32_t* R, const int32_t* V, int32_t* target);
void wifiDelay(int delayTime);

#endif
