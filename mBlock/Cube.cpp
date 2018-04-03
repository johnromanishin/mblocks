// Includes
#include "Defines.h"
#include "Behavior.h"
#include "Cube.h"
#include "Face.h"
#include <Wire.h> 
#include <stdint.h>

// *INDEX - crt + find - to go to...
// *PLAN-E | plane related
// *UPDAT-E | - related to updating things...
// *MOTIO-N | related to motion]
// *NEIGHBO-R  -related to figuring out neighbors
// *SENSOR-S - IMU's and light sensors
// *MIS-C - random things
// *LIGHT-S
// *STAT-E - related to IMUs...

Cube::Cube()
  :    
    currentPlaneBuffer(ARRAY_SIZEOF(this->currentPlaneBufferData),            this->currentPlaneBufferData),
    moveSuccessBuffer(ARRAY_SIZEOF(this->moveSuccessBufferData),              this->moveSuccessBufferData),
    topFaceBuffer(ARRAY_SIZEOF(this->topFaceBufferData),                      this->topFaceBufferData),
      
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
{ 
  for(int i = 0; i < ARRAY_SIZEOF(this->faces); i++)
  {
     this->faces[i].setIOExpanderAddress(this->faceExpanderAddresses[i]);
  }
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////// *UPDATE //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

bool Cube::update()
{
  /*
   * This functions updates all of the sensor buffers on each cube
   * It also refreshes variables like this->topFace/ forwardFace/ ...
   */
  this->processState();// this deals with anything involving IMUs 
  
  for(int i = 0; i< FACES; i++)
    {
      delay(3); // we wait a bit to allow the i2c switches to switch, so that we don't get collisions on the I2C bus
      this->faces[i].updateFace();  // updateFupdatesace  light and Magnetic sensors  
      delay(3);
    }
  return(true);
}

bool Cube::processState()
/* This Function updates the cube's Information about its physical state
 *  that can be determined by its accelerometers.
 *  This works by first attempting to update both the frame and the core IMU values.
 *  
 *  Then we update which face is pointed up (this->topFace)
 *  and which face is "forward" (In the movement direction if we apply a forward movement)
 */
{
  bool succeed = false; // This variable stores the status of the updateBothImu command
  if(this->updateBothIMUs() == true) // this runs the code to check on the two MPU6050 accelerometers
  {
    wifiDelay(4);
    succeed = true;
  }
  else // This means that one or both of the acellerometers are not functioning correctly
  {
    wifiDelay(200); // We wait a little while(While checking on WIFI stuff) to figure out WHAT IS GOING ON
    if((this->updateBothIMUs() == true) || (this->cubeID > 60)) // Try to update the two IMUS, if the cubeID is larger than 
                                                                // 60, then it isn't a real cube, so we say that the update 
                                                                // succedded just so we can keep going...
    {
      succeed = true;
    }
    else // this means we have failed TWICE to properly update both IMU's
    {
       {
         wifiDelay(300);
         this->resetI2C(); // We try to reset the I2C bus, since this could very well be the problem
         wifiDelay(300);
         if(this->updateBothIMUs() == true)
         {
            succeed = true;
         }
       }
    }
  }    
  this->determineTopFace();     // This checks the gravity vector of the frame, to determine which face (IF ANY) is pointed vertically updwards
  this->determineForwardFace(); // this combines the internal state of the core (Which plane it is in) with the TOP FACE to 
                                //figure out which face it will move towards
  return(succeed);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////// *MOTION /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
bool Cube::roll(bool forwardReverse, int rpm)
/*
 * This function is intended to be used when a cube is NOT on a lattice
 * It will roll around the environment, defaults to FORWARD with 6000 RPM
 * it returns TRUE and Blinks Green if it thinks it moved Substantially
 * and returns FALSE and Blinks REd if it thinks that it did not move...
 */
{
  this->processState(); // update IMU's and "topFace" 
  int timeToDelayBeforeBrake = (1700 + (rpm-6000)); // wait long eneough for flywheel to get to speed
  int faceUpBeginning = this->returnTopFace(); // record which face is pointing upwards when we start...
  bool succeed = false; // start out with the assumption that we haven't moved
  int shakingThreshold = 10000; // this variable repeatedly checks the GYRO readings while it should be moving to 
                                // see if we moved, but ended up in the same face anyway
  String CW_CCW; // this is an empty string which checks whether we move in a clockwise or a counter-clockwise direction
  
  if(forwardReverse == false) // if the input arguement is false, we move in reverse...
  {
    this->superSpecialBlink(&yellow, 50); // blink just for fun!
    CW_CCW = " r "; // set the direction to "reverse" if forwardReverse is negatuve
  }
  else // alternativly if forwardReverse is true... we move forward...
  {
    this->superSpecialBlink(&teal, 50); // blinks just for funzies!
    CW_CCW = " f ";
  }
  
  String stringToPrint = "bldcspeed" + CW_CCW + String(rpm); // generate String to print on the serial port to the motor controller PCBa
  this->printString(stringToPrint); // we use the function "printString" since it checks for an acknowledgement to our string...
  
  wifiDelay(timeToDelayBeforeBrake); // motor is now getting up to speed...

  this->printString("bldcstop b");
  int shakingAmmount = wifiDelayWithMotionDetection(3400); // this delays for the specified time, but also periodically adds the 
                                                           // values of the gyroscopes, and then returns the sum at the end.
  
  if(MAGIC_DEBUG) 
  {
    Serial.print("We detected this ammount of Shaking: ");
    Serial.println(shakingAmmount);
  }
  
  this->processState(); // process state again, now we would know if the "TOPFACE" has changed...
  wifiDelay(100);
  if((this->returnTopFace() == faceUpBeginning) || (this->returnTopFace() == -1)) // If the same face is pointing up... or it failed
  {
    if(shakingAmmount > shakingThreshold) // if we shook a bunch... but same face is up...
    {
      succeed = true;                   // consider it a success!
    }
    else
    {
      this->superSpecialBlink(&red, 40); // blink red so the human knows that something went wrong...
      this->superSpecialBlink(&red, 40);
    }
    /*
     * this means the move failed...
     * If we roll a lot, and end up with the same face as we started, the shakingAmmount
     * should be quite high, above the threshold, so we will say we succeeded if we shake a ton, or if
     * the face that is up is different than the face that was up before we started...
     */
  }
  else
  {
    this->superSpecialBlink(&green, 40);
    succeed = true;
  }
  
  this->moveSuccessBuffer.push(succeed); // store the results in a buffer, so that if we fail to move repeatedly, we know something is wrong...
  return(succeed);
}

bool Cube::moveBLDCACCEL(bool forwardReverse, int current, int lengthOfTime)
/*
 * This function is designed to use the electrical acceleration of the modules
 * so that they can move in unstructured environments...
 */
{
  this->processState(); // update IMU's and "topFace" 
  wifiDelay(200);
  int faceUpBeginning = this->returnTopFace();
  bool succeed = false;
  int shakingThreshold = 10000;
  String CW_CCW;
  String stringToPrint = "ver";
  String singleSpace = " ";
  if(forwardReverse == false)
  {
    this->blockingBlink(&yellow);
    CW_CCW = " r "; // set the direction to "reverse" if forwardReverse is negatuve
  }
  else
  {
    this->blockingBlink(&teal);
    CW_CCW = " f ";
  }
  wifiDelay(20);
  stringToPrint = "bldcaccel" + CW_CCW + String(current) + singleSpace + String(lengthOfTime); // generate String for motor
  Serial.println(stringToPrint); // this actually tells the thing to move.
  wifiDelay(200+lengthOfTime);
  Serial.println("bldcstop b"); // this actually tells the Cube to start rolling
  int shakingAmmount = wifiDelayWithMotionDetection(3000);
  Serial.println("bldcstop");
  if(MAGIC_DEBUG) {Serial.print("We detected this ammount of Shaking: ");Serial.println(shakingAmmount);}
  this->processState();
  wifiDelay(100);
  if((this->returnTopFace() == faceUpBeginning) || (this->returnTopFace() == -1)) // If the same face is pointing up... or it failed
  {
    if(shakingAmmount > shakingThreshold) // if we shook a bunch... but same face is up...
    {
      succeed = true;                   // consider it a success!
    }
    else
    {
      this->superSpecialBlink(&red, 40);
    }
    /*
     * this means the move failed...
     * If we roll a lot, and end up with the same face as we started, the shakingAmmount
     * should be quite high, above the threshold, so we will say we succeeded if we shake a ton, or if
     * the face that is up is different than the face that was up before we started...
     */
  }
  else
  {
    this->superSpecialBlink(&green, 40);
    succeed = true;
  }
  
  this->moveSuccessBuffer.push(succeed);
  return(succeed);
}

bool Cube::moveOnLattice(Motion* motion)
/*
 * This program attempts to perform a lattice move.
 * The possible moves include the following:
 * ** See Defines.cpp for more information **
 * Motion traverse_F     
 * Motion traverse_R
 * 
 * Motion cornerClimb_F
 * Motion cornerClimb_R
 * 
 * Motion shake_F 
 * Motion softShake_F
 * 
 * Motion explode_F
 * Motion explode_R
 * 
 * Motion rollDouble_F
 * Motion rollDouble_R
 * 
 * Usage... c->moveOnLattice(&cornerClimb_F);
 * returns true or false if it thinks that the move was successful
 */
{
  if(MAGIC_DEBUG){Serial.println("moveIASimple(Motion* motion)");}
  
  this->processState(); // update IMU's and "topFace" 
  if(this->isValidPlane() == true) // checks to make sure we are in one of the 3 valid planes
  {
    // Figure out our current state...
    int faceUpBeginning = this->returnTopFace(); // record what our initial top face is
    bool succeed = false;
  
    // Actually send the action to Kyles Board...
    String iaString = "ia " + String(motion->for_rev)+ " " + String(motion->rpm) + " " + String(motion->current) + " " + String(motion->brakeTime) + " e 15";
    this->printString(iaString); // print the command to kyles Board
    wifiDelay(motion->timeout); // wait for the action to complete
  
    // we are now waiting, and collecting data
    wifiDelayWithMotionDetection(3000);
  
    // Check to see our NEW state...
    this->processState();
    // If UP face is the same... we say we failed =(
    if(this->returnTopFace() == faceUpBeginning)
    {
      this->superSpecialBlink(&red, 40);
    }
    
    //if up face is different we say we succeeded!!!
    else
    {
      this->superSpecialBlink(&green, 40);
      succeed = true;
    }
    
    this->moveSuccessBuffer.push(succeed);
    return(succeed);
  }
  else
  {
    return(false);
  }
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//////////////////// *NEIGHBOR RELATED////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

int Cube::numberOfNeighbors(int index, bool doIlightFace)
/*
 * This function returns an integer number corresponding to how many 
 * neighbors that it thinks that it has.
 * 
 * if doIlightFace is TRUE, it will light up each face that it thinks that it has a neighbor on
 */
{
  int neighbors = 0; // initialize a empty integer
  for(int face = 0; face < 6; face++)
    {
    if((this->faces[face].returnNeighborType(0) == TAGTYPE_REGULAR_CUBE) ||
       (this->faces[face].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE))
    {
      neighbors++;
      if(doIlightFace) 
      {
        this->lightFace(face,&green);
        wifiDelay(200);
      }
    }
  }
return(neighbors);
}

int Cube::whichFaceHasNeighbor(int index)
/*
 * This is designed to return the face number for each face connection...
 * it works like this, you say
 * whichFaceHasNeighbor(0) // or default - and it returns the first face, counting from 0
 * that has a neighbor connected.
 * 
 * whichFaceHasNeighbor(1) will return the next connected face...
 * 
 * So if the cube has neighbors on faces, 2, 4, and 5,
 * this SHOULD be the result...
 * 
 * whichFaceHasNeighbor(0) -> 2
 * whichFaceHasNeighbor(1) -> 4
 * whichFaceHasNeighbor(2) -> 5
 * whichFaceHasNeighbor(3) -> -1 -- no faces...
 * 
 * Yeah I am sure there is a fancier way to do this...
 * but I don't want to think about it right at this moment.
 */
{
  int facesNeighbors[6] = {-1, -1, -1, -1, -1, -1}; 
  
  int facesCount = 0;
  for(int face = 0; face < 6; face++)
  {
    if((this->faces[face].returnNeighborType(0) == TAGTYPE_REGULAR_CUBE) ||
       (this->faces[face].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE))
    {
      facesCount++;
      if(facesCount > 0)
        facesNeighbors[facesCount-1] = face;
    }
  }
return(facesNeighbors[index]);
}

////////
int Cube::numberOfNeighborsCheckNow()
{
  int neighbors = 0;
  for(int face = 0; face < 6; face++)
  {
    if(this->faces[face].isThereNeighbor() == true)
    {
      neighbors++;
    }
  }
return(neighbors);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//////////////////// *PLANE ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

bool Cube::isPlaneInPlaneOfFaces(int face1, int face2)
/*
 * 
 */
{
  bool result = false;
  if((face1 > -1) && (face1 < 6))
  {
    if((face2 > -1) && (face2 < 6))
    {
      if(this->currentPlaneBuffer.access(0) == returnPlane(face1, face2))
      {
        result = true;
      }
    }
  }
  return(result);
}

bool Cube::goToPlaneIncludingFaces(int face1, int face2)
/*
 * 
 */
{
  bool result = false;
  if((face1 > -1) && (face1 < 6))
  {
    if((face2 > -1) && (face2 < 6))
    {
      result = this->setCorePlaneSimple(returnPlane(face1, face2));
    }
  }
  return(result);
}

bool Cube::isPlaneParallel(int faceExclude)
/*
 * 
 */
{
  bool result = false;
  if((faceExclude == 4 )|| (faceExclude == 5))
  {
    if(this->currentPlaneBuffer.access(0) == PLANE0123)
      result = true;
  }
  else if((faceExclude == 1) ||( faceExclude == 3))
  {
    if(this->currentPlaneBuffer.access(0) == PLANE0425)
      result = true;
  }
  else if((faceExclude == 0) || (faceExclude == 2))
  {
    if(this->currentPlaneBuffer.access(0) == PLANE1453)
      result = true;
  }
return(result);
}

bool Cube::goToPlaneParallel(int faceExclude)
/*
 * 
 */
{
  bool result = false;
  if((faceExclude == 4 )|| (faceExclude == 5))
  {
    result = this->setCorePlaneSimple(PLANE0123);
  }
  else if((faceExclude == 1) ||( faceExclude == 3))
  {
    result = this->setCorePlaneSimple(PLANE0425);
  }
  else if((faceExclude == 0) || (faceExclude == 2))
  {
    result = this->setCorePlaneSimple(PLANE1453);
  }
  return(result);
}

bool Cube::setCorePlaneSimple(PlaneEnum targetCorePlane)
/*
 * 
 */
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
  this->lightPlaneRing(targetCorePlane); // blink the desired plane to go into...
  Serial.println("sma retractcurrent 1050");
  wifiDelay(800);
  bool succeed = false;
  this->printString("sma retract 7500");
  int beginTime = millis();
  wifiDelay(500);
  while((this->findPlaneStatus(false) != targetCorePlane) && ((millis()-beginTime) < 8300))
  {
    PlaneEnum likelyStatus = this->currentPlaneBuffer.access(0);
    if(likelyStatus == PLANEMOVING)
      wifiDelay(300);
      
    String bldcaccelString = "bldcaccel f " + String(GlobalPlaneAccel) + " 800";
    
    if(likelyStatus == PLANENONE)
       bldcaccelString = "bldcaccel f " + String(GlobalPlaneAccel) + " 500";
       
    Serial.println(bldcaccelString);
    wifiDelay(1000);
    
    if(this->findPlaneStatus(false) == targetCorePlane)
    {
      Serial.println("bldcstop");
      wifiDelay(1000);
    }
    else
    {
      Serial.println("bldcstop b");
    }
    wifiDelay(1800);
  }
  
  while((millis()-beginTime) < 8000)
  {
    wifiDelay(100);
  }
  
  if(this->findPlaneStatus(true) == targetCorePlane)
  {
    this->superSpecialBlink(&green, 100);
    return(true);
  }
  /*
   * Nothing checked out... So the default is to return false
   */
  this->superSpecialBlink(&red, 100);
  return(false);
}

/**
 * This function uses the 
 * We expect raw, signed 14-bit accelerometer readings
 */
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

bool Cube::isValidPlane()
/*
 * 
 */
{
  if((this->currentPlaneBuffer.access(0) == PLANE0123) ||
     (this->currentPlaneBuffer.access(0) == PLANE0425) || 
     (this->currentPlaneBuffer.access(0) == PLANE1453)) //  This makes sure we don't move when we shouldn't
  {
    return(true);
  }
  else
    return(false);
}

PlaneEnum Cube::findPlaneStatus(bool reset)
/*
 * 
 */
{
  if(this->cubeID > 50) // this means it is a benchtop example and it can't actually run this.
  {
    this->currentPlaneBuffer.push(PLANE0123);
    return(PLANE0123);
  }
  
  PlaneEnum likelyStatus = PLANEERROR; 
  if(this->updateBothIMUs()) // try to update IMU...
  {
    delay(1);
  }
  else                       // if it fails, flip power switch
  {
    wifiDelay(30);
    if(reset) // if the input parameter tells us that we should reset the i2c...
      {
        this->resetI2C();
      }
    if(this->updateBothIMUs())// and try again to contact the IMU's
    {
      delay(1);
    }
    else
    {
      this->currentPlaneBuffer.push(likelyStatus);
      return(likelyStatus);   // if it fails on second try... exit which will be PLANEERROR
    }
  }
    const int validPlaneThreshold = 220; // This number is what determines if it is actually in a proper plane
    const int gyroMovingThreshold  = 1700; // This represents what "moving" is in terms GYRO values
    
    int32_t coreAccel[3] =   {this->axCoreBuffer.access(0),     // Access the most recent X Y and Z Values for the accelerometer
                              this->ayCoreBuffer.access(0),     // These should have just been updated...
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
    // We are evaluating the "distance" from the actual accelerometer on the "core" 
    // to that of the accelerometer values on the "frame"
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
      likelyStatus = PLANEMOVING;
    }
    
    // If one of the distances, for either 0, 1 or 2 is less than the threshold
    // then we return that plane from the planeEnumMap... This means we are in an 
    // actual plane
    else if((distance[minidx] < validPlaneThreshold) && 
            (sumOfGyros < gyroMovingThreshold))
    {
      likelyStatus = planeEnumMap[minidx];
    }
    
    // This returns of the minumium distance is greater than the valid plane threshold... we might be close
    // but it is dangerous to assume we are in a valid plane if we really aren't
    else if(distance[minidx] > validPlaneThreshold)
    { 
      likelyStatus = PLANENONE;
    }  
    // the default is to return an error
    else
    {
      likelyStatus = PLANEERROR;
    }
  // Add the result of this else if tree to the currentPlaneBuffer
  this->currentPlaneBuffer.push(likelyStatus);
  return(likelyStatus);
}

static void apply_3x3_mult(const int32_t* R, const int32_t* V, int32_t* target)
/**
 * Multiplies two Q15.16 matricies.  R should be 3x3 and V should be should
 * be 1x3.
 */
{
    for(int i = 0; i < 3; i++)
    {
        target[i] = 0;
        for(int j = 0; j < 3; j++)
            target[i] += (V[j] * R[j + (3 * i)]) / (65536);
    }
}

static int32_t vector_distance_squared(const int32_t* a, const int32_t* b)
/*
 * 
 */
{
    int32_t accum = 0;
    for(int i = 0; i < 3; i++)
        accum += ((a[i] - b[i]) * (a[i] - b[i])) / (65536);

    return accum;
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// *MISC /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

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
  wifiDelay(300);
  this->reconnectI2C();
  wifiDelay(300);
}


void Cube::shutDown()
// Turns the entire cube off by printing "sleep" to the slave board
{
  while(1)
  {
    Serial.println("sleep");delay(1000);
  }
}   

void Cube::shutDownESP()
// Turns the entire cube off by printing "sleep" to the slave board
{
  while(1)
  {
    Serial.println("espoff");delay(1000);
  }
}  

int Cube::returnTopFace(int index)
{
  return(this->topFaceBuffer.access(index));
}

int Cube::returnBottomFace()
{
  return(oppositeFace(this->topFace));
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
    if(this->cubeID < 30)
    {
      if(this->updateCoreIMU())
      {
        updateCount++;
        motionSum += (abs(this->gxCoreBuffer.access(0)) + 
                      abs(this->gyCoreBuffer.access(0)) + 
                      abs(this->gzCoreBuffer.access(0)));     
      }
    }
    else
    {
    if(this->updateFrameIMU())
      {
        updateCount++;
        motionSum += (abs(this->gxFrameBuffer.access(0)) + 
                      abs(this->gyFrameBuffer.access(0)) + 
                      abs(this->gzFrameBuffer.access(0)));     
      }
    }
    mesh.update();
    wifiDelay(50);
  }
  if((motionSum == 0) || (updateCount == 0))
  {
    return(0); // to make sure we don't have a divide by zero error...
  }
  return(motionSum /(updateCount));
}





bool Cube::isFaceNeitherTopNorBottom(int face)
/*
 * Returns "true" if face arguement "face"
 * happens to currently be one of the four faces in ring
 * around relative to gravity
 * 
 * Returns "false" if it is top or bottom face or if 
 * the cube isn't sure what orientation its in
 */
{
  bool result = false;
  if(this->returnTopFace() == -1)
    delay(1);
  else if(face == this->returnTopFace())
    delay(1);
  else if(face == this->returnBottomFace())
    delay(1);
  else
    result = true;
  return result;
}




bool Cube::printString(String stringToPrint, int waitTime)
/*
 * This function is desinged to help fix a problem that we have with the serial communication
 * It is designed to print a string, and then wait a little while to see if the other board responds,
 * 
 * if the other board doesn't respond (See more than 5 characters...) within a certian time limit,
 * we try to print the message again. if it fails twice in a row, then we return false,
 */
{
  Serial.println(stringToPrint); // tries to print the string...
  if(this->anythingOnSerial(waitTime) == false) // if we see characters comming back withing waittime, we assume success...
  {
    this->blinkRingAll(20, 2); // just for fun blink a ring...
    Serial.println(stringToPrint);
    if(this->anythingOnSerial(waitTime) == false) 
    {
      return(false);
    }
  }
  return(true);
}

bool Cube::anythingOnSerial(int waitTime)
/*
 * First erases serial que... then
 * waits to see if we receive at least 5 serial characters...
 * 
 * Useage, put immediately after a Serial.println to see if we see anything...
 */
{
  int beginTime = millis();
  int bytesSeen = 0;
  // First we empty out the buffer...
  while((Serial.available() > 0) && (millis() - beginTime) < waitTime )
  {
    int newByte = Serial.read();
  }

  // if there is still time left, we count the incomming bytes...
  while((millis() - beginTime) < waitTime)
  {
    delay(1);
    if(Serial.available() > 0)
    {
      int newByte = Serial.read();
      bytesSeen++;
    }
    if(bytesSeen > 5)
      {
        return(true);
      }
  }
  return(false);
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////// *SENSORS ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

int Cube::returnXthBrightestFace(int X, bool ExcludeTop)
{
 /*
  * Returns the face number corresponding to the brightest face
  */
  if(ExcludeTop)
  {
    int topFace = this->returnTopFace();
    // make sure to protect inputs... it will be -1 if cube is funny orientation
    if((topFace > -1) && (topFace < FACES))
    {
      this->faces[topFace].forceUpdateAmbientValue(0);
    }
    int connectedFace = this->whichFaceHasNeighbor();
    if((connectedFace > -1) && (connectedFace < 6))
    {
      this->faces[connectedFace].forceUpdateAmbientValue(0);
    }
  }
  
  if((X < 0) || (X > FACES)) return(-1);
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
////////////////////// *STATE and IMU RELATED///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
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
  this->topFaceBuffer.push(this->topFace);

  if(sum > 0)
  {
    return(true); 
  }
  else
  {
    return(false);
  }
}

bool Cube::areFacesOpposite(int face1, int face2)
{
  if(face1 == oppositeFace(face2))
    return(true);
  else
    return(false);
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
  PlaneEnum plane = this->currentPlaneBuffer.access(0);
  
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

void Cube::updateCubeID(int idNUM, long wifiAddress)
{
  this->cubeID = idNUM;
  this->cubeWifiAddress = wifiAddress;
  Serial.print("My New cubeID is: ");
  Serial.println(this->cubeID);
  Serial.print("and my wifiAddress is: ");
  Serial.println(this->cubeWifiAddress);
  if(thisCubeID >= 99) // stationary cube
  {
    Serial.print("Printing Debugging Information");
    MAGIC_DEBUG = 1;
  }
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////// *LIGHTS BLINKING RELATED//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void Cube::blinkParasiteLED(int blinkTime)
{
  digitalWrite(LED, HIGH);
  wifiDelay(blinkTime);
  digitalWrite(LED, LOW);
}

bool Cube::blockingBlink(Color* inputColor, int howManyTimes, int waitTime)
{

  for(int times = 0; times < howManyTimes; times++)
  {
    this->lightCube(inputColor);
    wifiDelay(waitTime);
    this->lightCube(&off);
    wifiDelay(waitTime);
  }
}

bool Cube::lightCorner(int corner, Color* inputColor)
{
    /*
   * Lights up a particular face with the color r | g | b
   */
   bool r = inputColor->r;
   bool g = inputColor->g;
   bool b = inputColor->b;

  this->clearRGB(); // Sets all RGB bits to HIGH / aka off
  if(corner % 2 == 0)
  {
     if(r){this->faces[corner / 2].setPinLow(this->faces[corner / 2].r_0);}
     if(g){this->faces[corner / 2].setPinLow(this->faces[corner / 2].g_0);}
     if(b){this->faces[corner / 2].setPinLow(this->faces[corner / 2].b_0);}
     for(int i = 0; i< 4; i++)
     {
        this->faces[i].updateIOExpander();
     }
  }
  else
  {
     if(r){this->faces[corner / 2].setPinLow(this->faces[corner / 2].r_1);}
     if(g){this->faces[corner / 2].setPinLow(this->faces[corner / 2].g_1);}
     if(b){this->faces[corner / 2].setPinLow(this->faces[corner / 2].b_1);}
     for(int i = 0; i< 4; i++)
     {
        this->faces[i].updateIOExpander();
     }
  }      
}

bool Cube::lightFace(int face, Color* inputColor)
{
    /*
   * Lights up a particular face with the color r | g | b
   */
   bool r = inputColor->r;
   bool g = inputColor->g;
   bool b = inputColor->b;
  if((face > -1) && (face < 6)) // valid face arguement...
  {
   delay(1);
  }
  else
  {
    return(false);
  }
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

void Cube::superSpecialBlink(Color* inputColor, int delayTime)
/*
 * This function blinks each corner of the cube in series, going from corner
 * 1 to corner 8, turns it on, then turns it off, used for debugging.
 */
{
  for(int corner = 0; corner < 8; corner++)
  {
    this->lightCorner(corner, inputColor);
    wifiDelay(delayTime);
  }
  this->lightCube(&off);
}

void Cube::blinkRingAll(int delayLength, int numberOfTimes)
/*
 * This function blinks the white face LED's on each face in a circular patturn
 * Used for Debugging
 */
{
  for(int face = 0; face < FACES; face++)
  {
    this->faces[face].blinkRing(delayLength, numberOfTimes);
  }
}

void Cube::lightPlaneRing(PlaneEnum corePlane)
/*
 * 
 */
{
  if(corePlane == PLANE0123)
  {
    for(int i = 0; i < 2; i++)
    {
      this->faces[0].turnOnFaceLEDs(1, 0, 1, 0);
      this->faces[1].turnOnFaceLEDs(1, 0, 1, 0);
      this->faces[2].turnOnFaceLEDs(1, 0, 1, 0);
      this->faces[3].turnOnFaceLEDs(1, 0, 1, 0);
      wifiDelay(100);
      this->faces[0].turnOnFaceLEDs(0, 1, 0, 1);
      this->faces[1].turnOnFaceLEDs(0, 1, 0, 1);
      this->faces[2].turnOnFaceLEDs(0, 1, 0, 1);
      this->faces[3].turnOnFaceLEDs(0, 1, 0, 1);
      wifiDelay(100);
      this->faces[0].turnOffFaceLEDs();
      this->faces[1].turnOffFaceLEDs();
      this->faces[2].turnOffFaceLEDs();
      this->faces[3].turnOffFaceLEDs();
    }
  }
  else if(corePlane == PLANE0425)
  {
    for(int i = 0; i < 2; i++)
    {
      this->faces[0].turnOnFaceLEDs(1, 0, 1, 0);
      this->faces[4].turnOnFaceLEDs(1, 0, 1, 0);
      this->faces[2].turnOnFaceLEDs(1, 0, 1, 0);
      this->faces[5].turnOnFaceLEDs(1, 0, 1, 0);
      wifiDelay(100);
      this->faces[0].turnOnFaceLEDs(0, 1, 0, 1);
      this->faces[4].turnOnFaceLEDs(0, 1, 0, 1);
      this->faces[2].turnOnFaceLEDs(0, 1, 0, 1);
      this->faces[5].turnOnFaceLEDs(0, 1, 0, 1);
      wifiDelay(100);
      this->faces[0].turnOffFaceLEDs();
      this->faces[4].turnOffFaceLEDs();
      this->faces[2].turnOffFaceLEDs();
      this->faces[5].turnOffFaceLEDs();
    }
  }
  else if(corePlane == PLANE1453)
  {
  for(int i = 0; i < 2; i++)
    {
      this->faces[1].turnOnFaceLEDs(1, 0, 1, 0);
      this->faces[4].turnOnFaceLEDs(1, 0, 1, 0);
      this->faces[3].turnOnFaceLEDs(1, 0, 1, 0);
      this->faces[5].turnOnFaceLEDs(1, 0, 1, 0);
      wifiDelay(100);
      this->faces[1].turnOnFaceLEDs(0, 1, 0, 1);
      this->faces[4].turnOnFaceLEDs(0, 1, 0, 1);
      this->faces[5].turnOnFaceLEDs(0, 1, 0, 1);
      this->faces[3].turnOnFaceLEDs(0, 1, 0, 1);
      wifiDelay(100);
      this->faces[1].turnOffFaceLEDs();
      this->faces[4].turnOffFaceLEDs();
      this->faces[5].turnOffFaceLEDs();
      this->faces[3].turnOffFaceLEDs();
    }
  }
  else if(corePlane == PLANENONE)
  {
    wifiDelay(10);
  }
  else if(corePlane == PLANEERROR)
  {
    wifiDelay(10);
  }
  else if(corePlane == PLANEMOVING)
  {
    wifiDelay(10);
  }
}

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
/*
 * 
 */
{
  this->faces[primaryFace].turnOnFaceLEDs(faceLEDCornerMapping[primaryFace][adjacentFace][0],
                                          faceLEDCornerMapping[primaryFace][adjacentFace][1],
                                          faceLEDCornerMapping[primaryFace][adjacentFace][2],
                                          faceLEDCornerMapping[primaryFace][adjacentFace][3]);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////// *DEBUG PRINTING RELATED//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

String Cube::returnCurrentPlane_STRING()
{
 // typedef enum PlaneEnum {PLANE0123, PLANE0425, PLANE1453, PLANENONE, PLANEMOVING, PLANEERROR} PlaneEnum;
  String ResultString;
  PlaneEnum thePlaneNow = this->currentPlaneBuffer.access(0);
  if(       thePlaneNow == PLANE0123)   {ResultString = "PLANE0123";}
  else if ( thePlaneNow == PLANE0425)   {ResultString = "PLANE0425";}
  else if ( thePlaneNow == PLANE1453)   {ResultString = "PLANE1453";}
  else if ( thePlaneNow == PLANENONE)   {ResultString = "PLANENONE";}
  else if ( thePlaneNow == PLANEMOVING) {ResultString = "PLANEMOVING";}
  else if ( thePlaneNow == PLANEERROR)  {ResultString = "PLANEERROR";}
  return(ResultString);
}

String Cube::debugAccelerometers()
{
  //PlaneEnum likelyStatus = PLANEERROR;
  Serial.println("beginning Debug Accelerometers");
  if(this->updateBothIMUs())
  {
    Serial.println("Succeeded Updating Accelerometers");
  }
  else
  {
    Serial.println("Failed Updating Accelerometers");
  }
  String newmsg = " Core.ax: "    + String(this->axCoreBuffer.access(0))
                + " Core.ay: "    + String(this->ayCoreBuffer.access(0))
                + " Core.az: "    + String(this->azCoreBuffer.access(0))
                + " Frame.ax: "   + String(this->axFrameBuffer.access(0))
                + " Frame.ay: "   + String(this->ayFrameBuffer.access(0))
                + " Frame.az: "   + String(this->azFrameBuffer.access(0));
  Serial.println(newmsg);
  Serial.println("***********************************************************************");
  const int validPlaneThreshold = 200; // This number is what determines if it is actually in a proper plane
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
  Serial.print("Sum of Gyros = ");Serial.println(sumOfGyros);                      
  for(int i = 0; i < 3; i++)
  {
    apply_3x3_mult(&rotationMatricies[i][0][0], coreAccel, &transformed[i][0]);
    distance[i] = vector_distance_squared(&transformed[i][0], frameAccel);
    Serial.print("for index item: ");Serial.print(i);Serial.print(" result is: ");Serial.println(distance[i]);
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
  Serial.print("Smallest Index Item is: ");Serial.print(minidx);Serial.print(" result is: ");Serial.println(distance[minidx]);
  // At this point we should have a vector with the distance between the actual readings
  // and the perfect readings for each of the three planes... 
  // and we know the one with the lowest value: minidx
  if(sumOfGyros > gyroMovingThreshold)
  {
    Serial.println("RESULT IS PLANE MOVING");
  }
  else if((distance[minidx] < validPlaneThreshold) && 
          (sumOfGyros < gyroMovingThreshold))
  {
    Serial.println("RESULT IS A VALID PLANE... Figure out from indexes...");
  }
  else if((distance[minidx] > validPlaneThreshold))
  { 
    Serial.println("RESULT IS: PLANENONE");
  }  
  else
  {
    Serial.println("RESULT IS: PLANE ERROR");
  }
  return("bro");
  //**************************************************************************************                
}

