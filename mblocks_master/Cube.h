#ifndef CUBE_H
#define CUBE_H

#include "Defines.h"
#include "CBuff.h"
#include "Face.h"
#include "Z_ArrowMap.h"
#include "SerialDecoder.h"
#include <Arduino.h>

//typedef enum PlaneEnum {PLANE0123, PLANE0425, PLANE1453, PLANENONE, PLANEMOVING, PLANEERROR} PlaneEnum;

class Cube
/*
 * Cube class is the object for the cube itself
 */
{
  private:
    // General information
    int batteryVoltage = 0;
    PlaneEnum currentPlane; // The plane (out of three) that the central actuator is in - 0
    int topFace;      // face this is point upwards, as determined by the accelerometer
    int forwardFace;  // face that is pointing forwards
    int reverseFace;  // face that is pointing backwards

      // i2c Addresses
    const int frameIMUaddress = 0x69;
    const int coreIMUaddress  = 0x68;
    const int faceExpanderAddresses[6] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25};

      // Data storage spaces

    long faceSensorUpdateTimeData[10]; // This buffer stores millis() for when c->updateSensors is called
    Behavior behaviorBufferData[32];   // This buffer stores the history of the behaviors
    PlaneEnum currentPlaneBufferData[20]; // this buffer stores the history of which plane we are in
    bool moveSuccessBufferData[20];       // this buffer stores a history of moves and if they worked or not
    int moveShakingBufferData[20];          // this buffer stores the sum of the IMU for recent moves.
    int topFaceBufferData[10];

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

      // Internal functions
    //bool CornerRGB(int face, bool top, bool r, bool g, bool b); // Only for Version 0;
    bool determineForwardFace();        // updates variable this->ForwardFace
    bool determineTopFace(int threshold = 12500);             // updates variable this->UpFace

  public:
      // Public Variables
    int cubeID = 1;

    // Functions
    void updateCubeID(int idNUM);
    void disconnectI2C();
    void reconnectI2C();
    void resetI2C();
    void blinkParasiteLED(int blinkTime = 100);
    int wifiDelayWithMotionDetection(int delayTime);
    int shutDownTime = (60000*5); // time until board goes to sleep

      // Update Functions involving SENSORS
    bool updateSensors(bool ShouldIcheckForLightMessages = true); // Updates almost everything on the cube...
    int numberOfNeighbors(int index = 0, bool lightFace = true);
    int numberOfCubeNeighbors(int index = 0);
    bool updateFaces(bool checkForLight = true); // Updates all of the face sensors on all faces
    bool updateBothIMUs(); // updates BOTH IMU's
    bool updateFrameIMU();
    bool updateCoreIMU();
    bool wakeIMU(int i2cAddress);
    int returnXthBrightestFace(int index, bool ExcludeTop);
    int returnSumOfAmbient(); // returns the sum of all of the light sensors

        // Functions involving motion
    bool moveIASimple(Motion* motion);
    bool roll(int forwardReverse, SerialDecoderBuffer* buf, int rpm = 6000, String ALTERNATE = "nothing");

        // Related to the state itself
    bool processState();
    PlaneEnum returnCurrentPlane();
    String returnCurrentPlane_STRING();
    int returnForwardFace();
    int returnReverseFace();
    int returnTopFace(int index = 0);
    int returnBottomFace();
    bool isFaceNeitherTopNorBottom(int face);

    // Functions involving PLane Changing
    PlaneEnum findPlaneStatus(bool reset = true);
    bool setCorePlaneSimple(PlaneEnum targetCorePlane);
    //bool setCorePlane(PlaneEnum targetCorePlane, SerialDecoderBuffer* buf, int attemptTime = 6000); 
    bool goToPlaneParallel(int faceExclude);
    bool goToPlaneIncludingFaces(int face1, int face2, SerialDecoderBuffer* buf);
    

      // Functions involving LED's
    bool clearRGB(); // Turns off all LED's on the cube DUAL VERSIONS
    void lightsOff();
    bool lightFace(int face, Color* inputColor); //DUAL VERSIONS
    bool lightCube(Color* inputColor); // lights entire cube, defaults to yellow
    bool blockingBlink(Color* inputColor, int howManyTimes = 6, int waitTime = 100);
    void setFaceLEDsAtEdge(int, int); // 
    void blinkRainbow(int delayTime = 250);
    void blinkSpecial(int delayTime = 250);
    void blinkAmerica(int delayTime = 250);

    // Misc. Useful Functions
    bool determineIfLatticeConnected();
    void printOutDebugInformation();
    void shutDown();                   // Turns off the entire cub
    //
    long cubeMAC = ESP.getChipId();
    //
    CircularBuffer<long> faceSensorUpdateTimeBuffer;
    CircularBuffer<Behavior> behaviorBuffer;
    CircularBuffer<PlaneEnum> currentPlaneBuffer;
    CircularBuffer<bool> moveSuccessBuffer;
    CircularBuffer<int> moveShakingBuffer;
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

    ArrowMap arrowMap;
    //
    Face faces[6];
    Cube();
};

int sortList(int*, int, int);
int oppositeFace(int face);
static int32_t vector_distance_squared(const int32_t* a, const int32_t* b);
static void apply_3x3_mult(const int32_t* R, const int32_t* V, int32_t* target);
void wifiDelay(int delayTime);
bool waitForSerialResponse(SerialResponse response, int timeOutTime, SerialDecoderBuffer* buf);

#endif
