// We want to implement classes for ease of reference as we construct and modify code.
// Includes
#include "Defines.h"
#include "Behavior.h"
#include "Cube.h"
#include "SerialDecoder.h"
#include "Face.h"
#include "Z_ArrowMap.h"
#include <Wire.h> 
#include <stdint.h>

Cube::Cube()
  :    
    faceSensorUpdateTimeBuffer(ARRAY_SIZEOF(this->faceSensorUpdateTimeData),  this->faceSensorUpdateTimeData),
    behaviorBuffer(ARRAY_SIZEOF(this->behaviorBufferData),                    this->behaviorBufferData),
    currentPlaneBuffer(ARRAY_SIZEOF(this->currentPlaneBufferData),            this->currentPlaneBufferData),
    
    axFrameBuffer(ARRAY_SIZEOF(this->axFrameData), this->axFrameData),
    ayFrameBuffer(ARRAY_SIZEOF(this->ayFrameData), this->ayFrameData),
    azFrameBuffer(ARRAY_SIZEOF(this->azFrameData), this->azFrameData),
    gxFrameBuffer(ARRAY_SIZEOF(this->gxFrameData), this->gxFrameData),
    gyFrameBuffer(ARRAY_SIZEOF(this->gyFrameData), this->gyFrameData),
    gzFrameBuffer(ARRAY_SIZEOF(this->gzFrameData), this->gzFrameData),

    axCoreBuffer(ARRAY_SIZEOF(this->axCoreData), this->axCoreData),
    ayCoreBuffer(ARRAY_SIZEOF(this->ayCoreData), this->ayCoreData),
    azCoreBuffer(ARRAY_SIZEOF(this->azCoreData), this->azCoreData),
    gxCoreBuffer(ARRAY_SIZEOF(this->gxCoreData), this->gxCoreData),
    gyCoreBuffer(ARRAY_SIZEOF(this->gyCoreData), this->gyCoreData),
    gzCoreBuffer(ARRAY_SIZEOF(this->gzCoreData), this->gzCoreData)

//    coreMagnetAngleBuffer(ARRAY_SIZEOF(this->coreMagnetAngleData), this->coreMagnetAngleData),
//    coreMagnetStrengthBuffer(ARRAY_SIZEOF(this->coreMagnetStrengthData), this->coreMagnetStrengthData)
    
{ 
  for(int i = 0; i < ARRAY_SIZEOF(this->faces); i++)
  {
     this->faces[i].setIOExpanderAddress(this->faceExpanderAddresses[i]);
  }
}

////////////////////////////////////////////////////////////////////////////////////
//////////////////////COMMONLY USED FUNCTIONS///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

// In later sections, super long...
// bool Cube::lightCube(bool r, bool g, bool b)

bool Cube::moveIASimple(Motion* motion)
{
  delay(1500);
  this->blockingBlink(&green,2);
  delay(2000);
  this->lightsOff();
  delay(250);
  String iaString = "ia " + String(motion->for_rev)+ " " + String(motion->rpm) + " " + String(motion->current) + " " + String(motion->brakeTime) + " e 10";
  Serial.println(iaString);
  delay((motion->timeout+4500));
  return(true);
}

bool Cube::MoveIA(Motion* motion, SerialDecoderBuffer* buf)
{
  int attempts = 1;
  this->blockingBlink(&purple,2);
  delay(2000);
  this->lightsOff();
  delay(250);
  while(attempts) // we subtract 1 from "numberOfAttempts" each time around
  {
    String iaString = "ia " + String(motion->for_rev)+ " " + String(motion->rpm) + " " + String(motion->current) + " " + String(motion->brakeTime);
    Serial.println(iaString);
    delay(motion->timeout);
    //while((millis() - beginTime) < motion->timeout)
      //{
        //delay(100);
        //delay(1);
        //SerialResponse resp = pushNewChar(buf);
//        if(resp == RESPONSE_BLDC_STABLE)
//        { 
//          break;
//        }
      //}
    attempts--;
  }
  this->reconnectI2C();
  return(true);
}

void Cube::disconnectI2C()
{
  digitalWrite(Switch, LOW); 
}
void Cube::reconnectI2C()
{
  digitalWrite(Switch, HIGH); 
}

void Cube::resetI2C()
{
  this->disconnectI2C();
  wifiDelay(150);
  this->reconnectI2C();
  wifiDelay(25);
  this->blockingBlink(&teal,1,100);
  wifiDelay(10);
}
void Cube::blinkParasiteLED(int blinkTime)
{
  digitalWrite(LED, HIGH);
  wifiDelay(blinkTime);
  digitalWrite(LED, LOW);
}
bool Cube::updateSensors()
{
  /*
   * This functions updates all of the sensor buffers on each cube
   * It also refreshes variables like this->topFace/ forwardFace/ ...
   */
  this->processState();// -- this deals with anything involving IMUs 
  this->updateFaces(); // -- this checks all of the specific sensors on each face
  return(true);
}

bool Cube::processState()
{
  bool succeed = false;
  if(this->updateBothIMUs())
  {
    delay(1);
    succeed = true;
  }

  else
  {
    this->disconnectI2C();
    wifiDelay(100);
    this->reconnectI2C();
    wifiDelay(100);
    succeed = this->updateBothIMUs();
  }
  
  this->determineTopFace();
  this->determineForwardFace();
  return(succeed);
}

bool Cube::blockingBlink(Color* inputColor, int howManyTimes, int waitTime)
{

  for(int times = 0; times < howManyTimes; times++)
  {
    this->lightCube(inputColor);
    delay(waitTime);
    this->lightCube(&off);
    delay(waitTime);
  }
}

bool Cube::updateFaces()
{
for(int i = 0; i< FACES; i++)
    {
      delay(3);
      this->faces[i].updateFace();  // updateFace updates light and Magnetic sensors  
      delay(3);
    }
}

void Cube::shutDown()
// Turns the entire cube off by printing "sleep" to the slave board
{
  while(1)
  {
    Serial.println("sleep");delay(1000);
  }
}   

int Cube::returnTopFace()
{
  return (this->topFace);
}

int Cube::returnBottomFace()
{
  return(oppositeFace(this->topFace));
}

PlaneEnum Cube::returnCurrentPlane()
{
  /*
   * Current plane is the orientation of the cube with respect to the frame
   * Either 0123, 1435
   */
  return(this->currentPlane);
}

int Cube::returnForwardFace()
/*
 * Returns the face which is "forward"
 */
{
  return(this->forwardFace);
}

int Cube::returnReverseFace()
{
  return(oppositeFace(this->returnForwardFace()));
}

void wifiDelay(int delayTime)
{
  int millisNow = millis();
  while((millis() - millisNow) < delayTime)
  {
    delay(2);
    mesh.update();
  }
}

/*
 * Delays for specified ammount of time, and returns an integer 
 * representing the average rotational motion of the core over the time
 */
int Cube::wifiDelayWithMotionDetection(int delayTime) //**WIP
{
  int motionSum = 0;
  int updateCount = 0;
  long millisNow = millis();
  while((millis() - millisNow) < delayTime)
  {
    mesh.update();
    if(this->updateCoreIMU())
    {
      updateCount++;
      motionSum += (abs(this->gxCoreBuffer.access(0)) + 
                    abs(this->gyCoreBuffer.access(0)) + 
                    abs(this->gzCoreBuffer.access(0)));     
    }
    mesh.update();    
    delay(10);
  }
  if(motionSum == 0 || updateCount == 0)
  {
    return(0); // to make sure we don't have a divide by zero error...
  }
  return(motionSum/(updateCount));
}
////////////////////////////////////////////////////////////////////////////////////
//////////////////////REGARDING PLANE CHANGING//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

bool Cube::goToPlaneIncludingFaces(int face1, int face2, SerialDecoderBuffer* buf)
/*
 * 
 */
{
  bool result = false;
  
  //result = this->setCorePlane(returnPlane(face1, face2), buf, 8000);
  result = this->setCorePlaneSimple(returnPlane(face1, face2));
  return(result);
}

bool Cube::goToPlaneParallel(int faceExclude, SerialDecoderBuffer* buf)
{
  bool simple = true;
  bool result = false;
  if(faceExclude == 4 || faceExclude == 5)
  {
    if(simple)
      result = this->setCorePlaneSimple(PLANE0123);
    else
      delay(1);
      //result = this->setCorePlane(PLANE0123, buf, 8000);
  }
  else if(faceExclude == 1 || faceExclude == 3)
  {
    if(simple)
      result = this->setCorePlaneSimple(PLANE0425);
    else
      delay(1);
      //result = this->setCorePlane(PLANE0425, buf, 8000);
  }
  else if(faceExclude == 0 || faceExclude == 2)
  {
    if(simple)
      result = this->setCorePlaneSimple(PLANE1453);
    else
      delay(1);
      //result = this->setCorePlane(PLANE1453, buf, 8000);
  }
  return(result);
}

bool Cube::setCorePlaneSimple(PlaneEnum targetCorePlane)
{   
  if((targetCorePlane == PLANENONE)  ||
     (targetCorePlane == PLANEERROR) || 
     (targetCorePlane == PLANEMOVING)) // this protects the inputs
  {
    return(false);
  }
  if(this->findPlaneStatus(false) == targetCorePlane)
  {
    return(true);
  }
  //
  this->blockingBlink(&teal, 3);
  this->lightsOff();
  delay(1000);
  Serial.println("sma retractcurrent 1000");
  delay(250);
  bool succeed = false;
  int beginTime = millis();
  Serial.println("sma retract 8000");
  delay(1000);
  while((this->findPlaneStatus(false) != targetCorePlane) && ((millis()-beginTime) < 7000))
  {
    String bldcaccelString = "bldcaccel f " + String(GlobalPlaneAccel) + " 700";
    Serial.println(bldcaccelString);
    delay(800);
    if(this->findPlaneStatus(false) == targetCorePlane)
    {
      Serial.println("bldcstop");
      delay(1000);
    }
    else
    {
      Serial.println("bldcstop b");
    }
    delay(1600);
  }
  
  while((millis()-beginTime) < 8000)
  {
    wifiDelay(100);
  }
  
  if(this->findPlaneStatus(false) == targetCorePlane)
  {
    this->blockingBlink(&green, 2);
    return(true);
  }
  /*
   * Nothing checked out... So the default is to return false
   */
  this->blockingBlink(&red, 2);
  return(false);
}


#define ROOT2OVER2Q15_16 46341
#define ONE_Q15_16 65536
#define Q15_16_TO_DOUBLE(x) (((double)x) / (65536.0))
static PlaneEnum planeEnumMap[] = {PLANE0123, PLANE0425, PLANE1453};
 /**
 * 0, 120, -120
 */
static const int32_t rotationMatricies[3][3][3] =
{
    {{                0,                0,       ONE_Q15_16},
     {-ROOT2OVER2Q15_16, ROOT2OVER2Q15_16,                0},
     {-ROOT2OVER2Q15_16,-ROOT2OVER2Q15_16,                0}},

    {{ ROOT2OVER2Q15_16, ROOT2OVER2Q15_16,                0},
     {                0,                0,      -ONE_Q15_16},
     {-ROOT2OVER2Q15_16, ROOT2OVER2Q15_16,                0}},

    {{ ROOT2OVER2Q15_16,-ROOT2OVER2Q15_16,                0},
     {-ROOT2OVER2Q15_16,-ROOT2OVER2Q15_16,                0},
     {                0,                0,      -ONE_Q15_16}}
};

/**
 * This function uses the 
 * We expect raw, signed 14-bit accelerometer readings
 */
PlaneEnum Cube::findPlaneStatus(bool reset)
{
  PlaneEnum likelyStatus = PLANEERROR; 
  if(this->updateBothIMUs()) // try to update IMU...
  {
    delay(1);
  }
  else                       // if it fails, flip power switch
  {
    delay(10);
    if(reset)
      {
        this->resetI2C();
      }
    if(this->updateBothIMUs())// and try again
    {
      delay(1);
    }
    else
    {
      this->currentPlaneBuffer.push(likelyStatus);
      return(likelyStatus);   // if it fails on second try... exit
    }
  }
    const int validPlaneThreshold = 165; // This number is what determines if it is actually in a proper plane
    const int gyroMovingThreshold  = 1700; // This represents what "moving" is
    
    int32_t coreAccel[3] =   {this->axCoreBuffer.access(0),     
                              this->ayCoreBuffer.access(0),
                              this->azCoreBuffer.access(0)};
    int32_t frameAccel[3] =  {this->axFrameBuffer.access(0), 
                              this->ayFrameBuffer.access(0),
                              this->azFrameBuffer.access(0)};
    int32_t transformed[3][3];
    int32_t distance[3];
    
    int     centralGyro[3] =  {this->gxCoreBuffer.access(0),
                               this->gyCoreBuffer.access(0),
                               this->gzCoreBuffer.access(0)};
                               
    int     sumOfGyros = (abs(centralGyro[0]) + // add up three components of gyro
                          abs(centralGyro[1]) + // tells us how much "movement"
                          abs(centralGyro[2]));
    
    
    //test each of the rotation matricies.  Store all results for debug purposes.
    for(int i = 0; i < 3; i++)
    {
      apply_3x3_mult(&rotationMatricies[i][0][0], coreAccel, &transformed[i][0]);
      distance[i] = vector_distance_squared(&transformed[i][0], frameAccel);
    }
    
    int mindist = distance[0];
    int minidx = 0;
    for(int i = 1; i < 3; i++) // figure out which one is closest to the value
    {
      if(distance[i] < mindist)
      {
        minidx = i;
        mindist = distance[i];
      }
    }
    // At this point we should have a vector with the distance between the actual readings
    // and the perfect readings for each of the three planes... 
    // and we know the one with the lowest value: minidx
    if(sumOfGyros > gyroMovingThreshold)
    {
      this->currentPlane = PLANEMOVING;
      likelyStatus = PLANEMOVING;
    }
    else if((distance[minidx] < validPlaneThreshold) && 
            (sumOfGyros < gyroMovingThreshold))
    {
      this->currentPlane = planeEnumMap[minidx];
      likelyStatus = planeEnumMap[minidx];
    }
    else if((distance[minidx] > validPlaneThreshold))
    { 
      this->currentPlane = PLANENONE;
      likelyStatus = PLANENONE;
    }  
    else
    {
      this->currentPlane = PLANEERROR;
      likelyStatus = PLANEERROR;
    }
  this->currentPlaneBuffer.push(likelyStatus);
  return(likelyStatus);
}

/**
 * Multiplies two Q15.16 matricies.  R should be 3x3 and V should be should
 * be 1x3.
 */
static void apply_3x3_mult(const int32_t* R, const int32_t* V, int32_t* target)
{
    for(int i = 0; i < 3; i++)
    {
        target[i] = 0;
        for(int j = 0; j < 3; j++)
            target[i] += (V[j] * R[j + (3 * i)]) / (65536);
    }
}

static int32_t vector_distance_squared(const int32_t* a, const int32_t* b)
{
    int32_t accum = 0;
    for(int i = 0; i < 3; i++)
        accum += ((a[i] - b[i]) * (a[i] - b[i])) / (65536);

    return accum;
}
////////////////////////////////////////////////////////////////////////////////////
//////////////////////REGARDING MAGNETIC TAGS///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

/**
 * Given a primary face and a secondary face, this array gives led settings such that if the 
 * face leds on the "primary face" are set according to the given settings, they will be on 
 * the "side" of the secondary face.
 */
static const bool faceLEDCornerMapping[6][6][4] = 
{
  {{1,1,1,1}, {1,0,0,1}, {0,0,0,0}, {0,1,1,0}, {0,0,1,1}, {1,1,0,0}},
  {{0,1,1,0}, {1,1,1,1}, {1,0,0,1}, {0,0,0,0}, {0,0,1,1}, {1,1,0,0}},
  {{0,0,0,0}, {1,1,0,0}, {1,1,1,1}, {0,0,1,1}, {0,1,1,0}, {1,0,0,1}},
  {{1,0,0,1}, {0,0,0,0}, {0,1,1,0}, {1,1,1,1}, {0,0,1,1}, {1,1,0,0}},
  {{0,0,1,1}, {0,1,1,0}, {1,1,0,0}, {1,0,0,1}, {1,1,1,1}, {0,0,0,0}},
  {{1,0,0,1}, {1,1,0,0}, {0,1,1,0}, {0,0,1,1}, {0,0,0,0}, {1,1,1,1}}
};

void Cube::setFaceLEDsAtEdge(int primaryFace, int adjacentFace)
{
  this->faces[primaryFace].turnOnFaceLEDs(faceLEDCornerMapping[primaryFace][adjacentFace][0],
                                          faceLEDCornerMapping[primaryFace][adjacentFace][1],
                                          faceLEDCornerMapping[primaryFace][adjacentFace][2],
                                          faceLEDCornerMapping[primaryFace][adjacentFace][3]);
}

int Cube::numberOfNeighbors(int index, bool doIlightFace)
{
  int neighbors = 0;
  for(int face = 0; face < 6; face++)
    {
    if((this->faces[face].returnMagnetStrength_A(index) < 255) &&
       (this->faces[face].returnMagnetStrength_A(index) > 0)   && 
       (this->faces[face].returnMagnetStrength_B(index) < 255) &&
       (this->faces[face].returnMagnetStrength_B(index) > 0))
        {
          neighbors++;
          if(doIlightFace) 
            {
              this->lightFace(face,&green);
            }
        }
    }
    return(neighbors);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//////////////////////REGARDING LIGHT SENSORS///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

int Cube::returnXthBrightestFace(int X)
{
 /*
  * Returns the face number corresponding to 
  */
  if( (X < 0) || (X > FACES) ) {return(-1);}
  int tempArray[FACES];
  for(int i = 0; i < FACES; i++)
  {
    tempArray[i] = this->faces[i].returnAmbientValue(0);
  }
  return(sortList(tempArray, FACES, X));
}

int sortList(int* inputList, int listLength, int desiredIndex)
/*
 * returns the original index number representing where the items' new position
 * after the list inputList has been sorted 
 * e.g.
 * a[5] = [100,400,312,900,100]
 *         (0),(1),(2),(3),(4)      << returns this
 * sortList(a,5,0)   should return ---> (3) Since index posotion 3 holds the highest value
 * sortList(a,5,1)   should return ---> (1) Since index posotion 1 holds the second highest value
 */
{
  int swapsInLastCycle = 1;
  int listCopy[listLength]; // blank array to copy the input array into
  int indexList[listLength];
  
  for(int i = 0; i < listLength; i++) {indexList[i] = i;}
  for(int index = 0; index < listLength; index++) // copy items over to a new list
    {listCopy[index] = inputList[index];} 
  
  while(swapsInLastCycle != 0)
  {
    swapsInLastCycle = 0;
    for(int i = 0; i < (listLength-1); i++)
      {
        int temp = 0;
        int indexTemp = 0;
        if(listCopy[i] < listCopy[i+1]) // if this is true, we flip the values, and increment swaps
          {
            temp = listCopy[i]; 
            indexTemp = indexList[i];
          
            listCopy[i] = listCopy[i+1];
            indexList[i] = indexList[i+1];
          
            listCopy[i+1] = temp;
            indexList[i+1] = indexTemp;
          
            swapsInLastCycle++;
        }
      }
    }
  return(indexList[desiredIndex]);
} 
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//////////////////////Color and Sensor Related//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

bool Cube::lightFace(int face, Color* inputColor)
{
    /*
   * Lights up a particular face with the color r | g | b
   */
   bool r = inputColor->r;
   bool g = inputColor->g;
   bool b = inputColor->b;

  this->clearRGB(); // Sets all RGB bits to HIGH / aka off

  switch (face){
    case 0:
      if(r){this->faces[0].setPinLow(this->faces[0].r_0);this->faces[0].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[0].setPinLow(this->faces[0].g_0);this->faces[0].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[0].setPinLow(this->faces[0].b_0);this->faces[0].setPinLow(this->faces[0].b_1);}
      
      if(r){this->faces[1].setPinLow(this->faces[0].r_0);this->faces[1].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[1].setPinLow(this->faces[0].g_0);this->faces[1].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[1].setPinLow(this->faces[0].b_0);this->faces[1].setPinLow(this->faces[0].b_1);}
      break;
      
    case 1:
      if(r){this->faces[1].setPinLow(this->faces[0].r_0);this->faces[1].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[1].setPinLow(this->faces[0].g_0);this->faces[1].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[1].setPinLow(this->faces[0].b_0);this->faces[1].setPinLow(this->faces[0].b_1);}

      if(r){this->faces[2].setPinLow(this->faces[0].r_0);this->faces[2].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[2].setPinLow(this->faces[0].g_0);this->faces[2].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[2].setPinLow(this->faces[0].b_0);this->faces[2].setPinLow(this->faces[0].b_1);}
      break;
      
    case 2:
      if(r){this->faces[2].setPinLow(this->faces[0].r_0);this->faces[2].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[2].setPinLow(this->faces[0].g_0);this->faces[2].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[2].setPinLow(this->faces[0].b_0);this->faces[2].setPinLow(this->faces[0].b_1);}

      if(r){this->faces[3].setPinLow(this->faces[0].r_0);this->faces[3].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[3].setPinLow(this->faces[0].g_0);this->faces[3].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[3].setPinLow(this->faces[0].b_0);this->faces[3].setPinLow(this->faces[0].b_1);}
      break;
      
    case 3:
      if(r){this->faces[3].setPinLow(this->faces[0].r_0);this->faces[3].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[3].setPinLow(this->faces[0].g_0);this->faces[3].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[3].setPinLow(this->faces[0].b_0);this->faces[3].setPinLow(this->faces[0].b_1);}

      if(r){this->faces[0].setPinLow(this->faces[0].r_0);this->faces[0].setPinLow(this->faces[0].r_1);}
      if(g){this->faces[0].setPinLow(this->faces[0].g_0);this->faces[0].setPinLow(this->faces[0].g_1);}
      if(b){this->faces[0].setPinLow(this->faces[0].b_0);this->faces[0].setPinLow(this->faces[0].b_1);}
      break;
      
    case 4:
      if(r){this->faces[0].setPinLow(this->faces[0].r_1);this->faces[1].setPinLow(this->faces[0].r_1);
            this->faces[2].setPinLow(this->faces[0].r_0);this->faces[3].setPinLow(this->faces[0].r_1);}
      
      if(g){this->faces[0].setPinLow(this->faces[0].g_1);this->faces[1].setPinLow(this->faces[0].g_1);
            this->faces[2].setPinLow(this->faces[0].g_0);this->faces[3].setPinLow(this->faces[0].g_1);}
      
      if(b){this->faces[0].setPinLow(this->faces[0].b_1);this->faces[1].setPinLow(this->faces[0].b_1);
            this->faces[2].setPinLow(this->faces[0].b_0);this->faces[3].setPinLow(this->faces[0].b_1);}
      break;
      
    case 5:
       if(r)
      {
        this->faces[0].setPinLow(this->faces[0].r_0);this->faces[1].setPinLow(this->faces[0].r_0);
        this->faces[2].setPinLow(this->faces[0].r_1);this->faces[3].setPinLow(this->faces[0].r_0);
      }
        if(g)
      {
        this->faces[0].setPinLow(this->faces[0].g_0);this->faces[1].setPinLow(this->faces[0].g_0);
        this->faces[2].setPinLow(this->faces[0].g_1);this->faces[3].setPinLow(this->faces[0].g_0);
      }
        if(b)
      {
        this->faces[0].setPinLow(this->faces[0].b_0);this->faces[1].setPinLow(this->faces[0].b_0);
        this->faces[2].setPinLow(this->faces[0].b_1);this->faces[3].setPinLow(this->faces[0].b_0);
      }
        break;
     }
    
  for(int i = 0; i< 4; i++)
  {
    this->faces[i].updateIOExpander();
  }
  return(true);
}

//
bool Cube::updateFrameIMU()
{
  int error;
  int16_t ax, ay, az, Tmp, gx, gy, gz;
  this->wakeIMU(this->frameIMUaddress);
  Wire.beginTransmission(this->frameIMUaddress);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  error = Wire.endTransmission(false);
  //succeed = Wire.requestFrom(this->frameIMUaddress, 14, 1); // 7 reads of 2 bytes
  Wire.requestFrom(this->frameIMUaddress, 14, 1); // 7 reads of 2 bytes
  ax =  Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  ay =  Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  az =  Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  gx =  Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  gy =  Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  gz =  Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  this->axFrameBuffer.push(ax);
  this->ayFrameBuffer.push(ay);
  this->azFrameBuffer.push(az);
  this->gxFrameBuffer.push(gx);
  this->gyFrameBuffer.push(gy);
  this->gzFrameBuffer.push(gz);
  
  // This returns true if the i2c command was a success...
  if(error == 0)
    {
      return(true);
    }
  else
    {
      return(false);
    }
}

bool Cube::updateCoreIMU()
{
  int error; // integer to store results of wire.endtransmission(false)
  int16_t ax, ay, az, Tmp, gx, gy, gz;
  this->wakeIMU(this->coreIMUaddress);
  Wire.beginTransmission(this->coreIMUaddress);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  error = Wire.endTransmission(false);
  Wire.requestFrom(this->coreIMUaddress, 14, 1); // 7 reads of 2 bytes
  ax =  Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  ay =  Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  az =  Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  gx =  Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  gy =  Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  gz =  Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  this->axCoreBuffer.push(ax);
  this->ayCoreBuffer.push(ay);
  this->azCoreBuffer.push(az);
  this->gxCoreBuffer.push(gx);
  this->gyCoreBuffer.push(gy);
  this->gzCoreBuffer.push(gz);
  
  // This returns true if the i2c command was a success...
  if(error == 0)
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool Cube::determineTopFace(int threshold)
{
  /**  
   *  Each IMU returns value from -16000 to +16000 if cube is stationary
   *  Since the Accelerometer lines up with X Y and Z axes we just check if any of the axes
   *  This function returns an integer representing
   *  the face which is point upwards. Returns -1 if results are inconclusive
   *  c.axFrameBuffer.access(0)
   */
  int sum = ( abs(this->axFrameBuffer.access(0)) +
              abs(this->ayFrameBuffer.access(0)) + 
              abs(this->azFrameBuffer.access(0)));
  
       if ((this->azFrameBuffer.access(0) < (-threshold)) && sum < 25000)  
          { this->topFace = 0; }
  else if ((this->azFrameBuffer.access(0) > (threshold)) && sum < 25000)   
          { this->topFace = 2; }
  else if ((this->axFrameBuffer.access(0) < (-threshold)) && sum < 25000)  
          { this->topFace = 5; }
  else if ((this->axFrameBuffer.access(0) > (threshold)) && sum < 25000)   
          { this->topFace = 4; }
  else if ((this->ayFrameBuffer.access(0) < (-threshold)) && sum < 25000)  
          { this->topFace = 1; }
  else if ((this->ayFrameBuffer.access(0) > (threshold)) && sum < 25000)   
          { this->topFace = 3; }
  else                                                                     
          {this->topFace = -1;}
  return (this->topFace);
}

bool Cube::wakeIMU(int i2cAddress)
// Wakes the IMU_6050 accelerometer
{
  Wire.beginTransmission(i2cAddress);
  Wire.write(0x6B);  // PWR_MGupdateFaceSensorsMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

bool Cube::determineForwardFace() // FUN3 // plane should be either int 1234, 1536 2546 or -1
{
  /*
   * plane PLANE_0321 /0
   * plane PLANE_0425 /1
   * plane PLANE_1534 /4
   */
  int topFace = this->returnTopFace(); // 
  PlaneEnum plane = this->returnCurrentPlane();
  
       if( (topFace == 1 && plane == PLANE0123)  ||  (topFace == 5 && plane == PLANE0425) )   
        {
          this->forwardFace = 0;
        }
  else if( (topFace == 2 && plane == PLANE0123)  ||  (topFace == 4 && plane == PLANE1453) )    
        {
          this->forwardFace = 1;
        }
  else if( (topFace == 3 && plane == PLANE0123)  ||  (topFace == 4 && plane == PLANE0425) )
        {
          this->forwardFace = 2;
        }
  else if( (topFace == 0 && plane == PLANE0123)  ||  (topFace == 5 && plane == PLANE1453) )    
        {
          this->forwardFace = 3;
        }
  else if( (topFace == 3 && plane == PLANE1453)  ||  (topFace == 0 && plane == PLANE0425) )    
        {
          this->forwardFace = 4;
        }
  else if( (topFace == 1 && plane == PLANE1453)  ||  (topFace == 2 && plane == PLANE0425) )    
        {
          this->forwardFace = 5;
        }
  
  else if(  (topFace == 4 && plane == PLANE0123) || 
            (topFace == 0 && plane == PLANE1453) || 
            (topFace == 1 && plane == PLANE0425) ) 
        {
          this->forwardFace = -1;
        }
  else if(  (topFace == 5 && plane == PLANE0123) || 
            (topFace == 2 && plane == PLANE1453) || 
            (topFace == 3 && plane == PLANE0425) ) 
        {
          this->forwardFace = -1;
        }
  else                                                                                                    
        {
          return(false);
        }
  return(true);
}

bool Cube::updateBothIMUs()
{
  bool a = this->updateCoreIMU();
  delay(2);
  bool b = this->updateFrameIMU();
  return(a && b);
}

bool Cube::clearRGB()
/*
 * Switches all of the bits to HIGH (or off) for all 8 corner RGBLED's
 * 
 */
{
for(int i = 0; i< 4; i++)
  {
    this->faces[i].setPinHigh(this->faces[0].r_0);
    this->faces[i].setPinHigh(this->faces[0].r_1);
    this->faces[i].setPinHigh(this->faces[0].g_0);
    this->faces[i].setPinHigh(this->faces[0].g_1);
    this->faces[i].setPinHigh(this->faces[0].b_0);
    this->faces[i].setPinHigh(this->faces[0].b_1);
  }
}

void Cube::lightsOff()
{
   this->clearRGB();
   for(int i = 0; i < 4; i++)
  {
    this->faces[i].updateIOExpander();
  }
}

bool Cube::lightCube(Color* inputColor)
  /*
   * Lights up the cube to be a specific color 
   */
{  
   bool r = inputColor->r;
   bool g = inputColor->g;
   bool b = inputColor->b;
this->clearRGB(); // Sets all RGB bits to HIGH / aka off
for(int i = 0; i < 4; i++)
{
  if(r)
    {
      this->faces[i].setPinLow(this->faces[0].r_0);
      this->faces[i].setPinLow(this->faces[0].r_1);
    }
  else
    {
      this->faces[i].setPinHigh(this->faces[0].r_0);
      this->faces[i].setPinHigh(this->faces[0].r_1);
    }
    
  if(g)
    {
      this->faces[i].setPinLow(this->faces[0].g_0);
      this->faces[i].setPinLow(this->faces[0].g_1);
    }
  else
    {
      this->faces[i].setPinHigh(this->faces[0].g_0);
      this->faces[i].setPinHigh(this->faces[0].g_1);
    }
    
  if(b)
    {
      this->faces[i].setPinLow(this->faces[0].b_0);
      this->faces[i].setPinLow(this->faces[0].b_1);
    }
  else
    {
      this->faces[i].setPinHigh(this->faces[0].b_0);
      this->faces[i].setPinHigh(this->faces[0].b_1);
    }
  this->faces[i].updateIOExpander();
}
return(true);
}

int oppositeFace(int face)
{
  if(face == 0){return(2);}
  else if(face == 1){return(3);}
  else if(face == 2){return(0);}
  else if(face == 3){return(1);}
  else if(face == 4){return(5);}
  else if(face == 5){return(4);}
  else{return(-1);}
}
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////DEBUG THINGS/////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
bool waitForSerialResponse(SerialResponse response, int timeOutTime, SerialDecoderBuffer* buf)
{
   bool result = false;
   long startTime = millis();
   while(millis()-startTime < timeOutTime)
   {
    delay(2);
    SerialResponse resp = pushNewChar(buf);
    mesh.update();
    if(resp == response)
        { 
          result = true;
          break;
        }
   }
   return(result);
}

void Cube::printOutDebugInformation()
{
      Serial.print("Top Face: ");Serial.println(this->returnTopFace());
      Serial.print("Forward Face: ");Serial.println(this->returnForwardFace());
      Serial.print("Brightest Face: ");Serial.println(this->returnXthBrightestFace(0));
      Serial.println("2nd Brightest Face: ");Serial.println(this->returnXthBrightestFace(1));
}

void Cube::lightRainbow(int delayTime)
{
  this->lightCube(&purple);
  delay(delayTime);
  this->lightCube(&red);
  delay(delayTime);
  this->lightCube(&yellow);
  delay(delayTime);
  this->lightCube(&green);
  delay(delayTime);
  this->lightCube(&teal);
  delay(delayTime);
  this->lightCube(&blue);
  delay(delayTime);
  this->lightCube(&white);
  delay(delayTime);
  this->lightCube(&off);
  delay(delayTime);
}
