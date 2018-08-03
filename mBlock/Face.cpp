#include "Defines.h"
#include "Cube.h"
#include "Face.h"
#include <Wire.h> 
#include <Arduino.h>

Face::Face()
      // Buffers for the raw sensor values
  : ambientBuffer(ARRAY_SIZEOF(this->ambientData), this->ambientData),
    magnetAngleBuffer_A(ARRAY_SIZEOF(this->magnetAngleData_A), this->magnetAngleData_A),
    magnetStrengthBuffer_A(ARRAY_SIZEOF(this->magnetStrengthData_A), this->magnetStrengthData_A),
    magnetAngleBuffer_B(ARRAY_SIZEOF(this->magnetAngleData_B), this->magnetAngleData_B),
    magnetStrengthBuffer_B(ARRAY_SIZEOF(this->magnetStrengthData_B), this->magnetStrengthData_B),
    
    // Buffers for the data which we extract from the sensor values
    neighborIDBuffer(ARRAY_SIZEOF(this->neighborIDData), this->neighborIDData),
    neighborAngleBuffer(ARRAY_SIZEOF(this->neighborAngleData), this->neighborAngleData),
    neighborCommandBuffer(ARRAY_SIZEOF(this->neighborCommandData), this->neighborCommandData),
    neighborFaceBuffer(ARRAY_SIZEOF(this->neighborFaceData), this->neighborFaceData),
    neighborTypeBuffer(ARRAY_SIZEOF(this->neighborTypeData), this->neighborTypeData),
    neighborPresenceBuffer(ARRAY_SIZEOF(this->neighborPresenceData), this->neighborPresenceData),
    neighborLightDigitBuffer(ARRAY_SIZEOF(this->neighborLightDigitData), this->neighborLightDigitData)
{
  this->IOExpanderState[0] = (this->IOExpanderState[1] = byte(0xff)); // sets bytes of IO expander to be all 1's
}

bool Face::updateFace()
{
  bool updateSuccess = false;
  /* 
   * If the lights were originally on,
   * We turn off the lights temporarily in order to check the light sensor
   * without being blinded by the light right next to
   */
  if(FACES_LIGHTS[IOExpanderAddress - IO_Address_offset] > 0)
  {
    this->turnOffFaceLEDs();
    wifiDelay(10);
  }
  
  updateSuccess = (this->enableSensors() &&
                   this->updateAmbient(true) &&
                   this->turnOnFaceLEDs(0,1,1,0) &&
                   this->updateMagneticBarcode() && // actually reads magnetic valuess
                   this->turnOffFaceLEDs());

  /*
   * This section processes the magnetic tags, it updates several buffers
   */
 
  this->neighborPresenceBuffer.push(this->processTag());

  /*
   * If we are supposed to turn on our lights on a particular face, we likely don't
   * care what is connected to it, so we basically keep this turned on, and don't need to check
   * for more light digits...
   */
   
  if(FACES_LIGHTS[IOExpanderAddress - IO_Address_offset] > 0)
  {
    this->turnOnFaceLEDs(1,1,1,1);
  }

  /*
   * If we are not supposed to turn on the LED's on this face, then we check for a light digit...
   * This also prevents us from both checking for light digits, but then both faces being off
   * and not detecting it afterall
   */
  else if(this->returnNeighborType(0) == TAGTYPE_REGULAR_CUBE)
  /*
   * Now we are going to check our light sensor 16 times, to try to determine if we are connected to
   * a cube which has its LIGHTS on - we do this 16 times to make sure we don't accidently get a false
   * positive from a quick blink
   * We only do this if we are connected to an actual cube...
   */
  {
    if(MAGIC_DEBUG)
    {
      Serial.println("Investigating a regular cube...");
    }
    
    int numberOfSamplesToTake = 16;
    for(int i = 0; i < numberOfSamplesToTake; i++)
      {
        this->updateAmbient(false);
      }
    /*
     * Now our the last 16 items in our buffer have ambient light values taken 20ms apart
     */
    this->neighborLightDigitBuffer.push(this->processLightData(numberOfSamplesToTake)); 

    /*
     * If the "Game" is "Line" and we just saw a light on this face, then we are going to set a flag
     * to permenantly turn on the light on the OPPOSITE face from our face...
     */
    if((Game == "Line") && (this->returnNeighborLightDigit(0) > 0))
    {
      int oppositeFaceFromUs = oppositeFace(IOExpanderAddress - IO_Address_offset);
      FACES_LIGHTS[oppositeFaceFromUs] = 1;
      //Serial.println("WHAT THE FUCK");
      //Serial.print("REturn neighbor light digit(0): ");
      //Serial.println(this->returnNeighborLightDigit(0));
      magicTheLight = true;
    }
  }

  this->disableSensors(); // turn off sensors...
  return(updateSuccess);
}


bool Face::processTag()
/*
 * This function evaluates the magnetic "tags" on 5 different characteristics
 * and then updates face-specific circular buffers for the following characteristics
 * The function returns data from the most recent magnetic buffer readings.
 * 
 * Tag Presence: True | False
 * Tag Type:  TAGTYPE_INVALID | TAGTYPE_REGULAR_CUBE | TAGTYPE_PASSIVE_CUBE | TAGTYPE_NOTHING,
 * Tag Angle: -1 |    0 | 1 | 2 | 3
 * Tag Command: TAGCOMMAND_NONE | ...
 */
{
    // Create temporary variables, and initialize them to be negative / invalid values
  bool tagPresent = false;
  TagType tagType = TAGTYPE_INVALID;        // Resets all of these values
  TagCommand tagCommand = TAGCOMMAND_NONE;  // Resets all of these values
  int tagAngle = -1; //                     // Resets all of these values
  int tagID = -1; //                        // Resets all of these values
  int tagFace = -1; //                      // Resets all of these values
  int magDigit1 = 0;
  int magDigit2 = 0;
  
  // Create local variables to store the values from the magnetic sensors.
  int angle1 = this->returnMagnetAngle_A(0);
  int angle2 = this->returnMagnetAngle_B(0);
  int agc1 = this->returnMagnetStrength_A(0);
  int agc2 = this->returnMagnetStrength_B(0);

  int strengthThreshold = 500; // This Value is a threshold that the two magnet strength values must sum to

  // This section creates magnetic Digits from the raw sensor Values
  // Magnet digits are simply values from 1-30 evenly spaced arouynd the circle, so each
  // digit corresponds to 12 degrees.
        if (agc1 == 0 || agc1 >= 255)       {magDigit1 = 0;}
   else if (angle1 < 6 || angle1 > 354)     {magDigit1 = 1;}
   else                                     {magDigit1 = int(angle1 + 18)/12;}

        if (agc2 == 0 || agc2 >= 255)       {magDigit2 = 0;}
   else if(angle2 < 6 || angle2 > 354)      {magDigit2 = 1;}  
   else                                     {magDigit2 = int(angle2 + 18)/12;}
  
  int tagStrength = agc1+agc2; // this is a measurement of how accurate the tag strength is

  /* If this evaluates to be true, then we know there is a valid tag, we will then check the values
   *  against tables in order to generate meaningful data from the raw data
   */
  if(((agc1+agc2) < strengthThreshold) && (tagStrength > 0)) // this means there is a valid tag!! woo!
  {
    tagPresent = true; // THIS MEANS WE HAVE A TAG!!
    /*========================================================================================================
    * CHECK IF TAG REPRESENTS A MODULE
    */
    if((magDigit1 >= 17 && magDigit1 <= 29) &&  // Means magdigit1 is a faceID
       (magDigit2 >= 1 && magDigit2 <= 17))     // Means magdifit2 stores an ID # 
    {
      tagType = TAGTYPE_REGULAR_CUBE;
      tagID   = magDigit2;
      tagFace = returnFaceNumber(magDigit1);
      if(magDigit1 % 2 ==0)
        tagAngle = 0;
      else
        tagAngle = 1;
    }    
    else if((magDigit2 >= 17 && magDigit2 <= 29) &&   // Means magdigit1 is a faceID
     (magDigit1 >= 1 && magDigit1 <= 17))             // Means magdigit2 stores an ID # 
    {
      tagType = TAGTYPE_REGULAR_CUBE;
      tagID   = magDigit1;
      tagFace = returnFaceNumber(magDigit2);
      if(magDigit2 % 2 ==0)
          tagAngle = 2;
      else                 
          tagAngle = 3;
    }
    /*=======================================================================================================
    * CHECK IF TAG REPRESENTS A PASSIVE MODULE
    */
    if((magDigit1 == 15 || magDigit1 == 16 || magDigit1 == 17 || // Means magdigit1 is a faceID
       magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2 ) &&
      (magDigit2 == 8  || magDigit2 == 9  || magDigit2 == 10))     // Means magdigit2 stores an ID # 
    {
      tagType = TAGTYPE_PASSIVE_CUBE;
      if(magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2)
        tagAngle = 2;
      else 
        tagAngle = 3;
    }
    else if((magDigit2 == 15 || magDigit2 == 16 || magDigit2 == 17 ||  // Means magdigit1 is a faceID
             magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2 ) &&
            (magDigit1 == 8  || magDigit1 == 9  || magDigit1 == 10))     // Means magdifit2 stores an ID # 
    {
      tagType = TAGTYPE_PASSIVE_CUBE;
      if(magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2)
            tagAngle = 0;
      else 
            tagAngle = 1;
    }
    /* ================================================================================================
    * CHECK IF TAG REPRESENTS A COMMAND TAG
    */
    if((((magDigit1 - magDigit2) > -2) && ((magDigit1 - magDigit2) < 2)) &&  // if the difference
    //between the two is small
       (magDigit1 != 17 && magDigit2 != 17) && (magDigit1 != 30 && magDigit2 != 30))
    {
      tagType = TAGTYPE_COMMAND;
      if(magDigit1 == 27) 
        tagCommand = TAGCOMMAND_27_GREEN;

      if(magDigit1 == 23 || magDigit1 == 24) 
        tagCommand = TAGCOMMAND_23_BLUE;
        
      if(magDigit1 == 9 || magDigit1 == 10) // change plane
        tagCommand = TAGCOMMAND_09_ORANGE;
        
      if(magDigit1 == 5 || magDigit1 == 4) // change plane
        tagCommand = TAGCOMMAND_05_PURPLE;
        
      if((magDigit1 == 13) || (magDigit1 == 12))
        tagCommand = TAGCOMMAND_13_RED;
    }
  }
   /*
    * We have now processed the tag, we now push all of the values into the circular buffer
    * that each cube has internally.
    */
  this->neighborTypeBuffer.push(tagType); 
  this->neighborCommandBuffer.push(tagCommand); 
  this->neighborFaceBuffer.push(tagFace); 
  this->neighborAngleBuffer.push(tagAngle); 
  this->neighborIDBuffer.push(tagID); 

  /*
  * However we also need to put the same information to global variables in order to send in wifi messages
  * So we turn the number to be an int representing the ID number, the Face number, and the Angle...
  * in this format: X X X     X
  *                [id][face][tagangle]
  */

  if(tagType == TAGTYPE_PASSIVE_CUBE)
  {
    tagID = 18;
  }

  /*
   * numberToReturn is a complicated way to get
   * the neighbor information through the wifi...
   * we start out by setting it to be -1 - meaning no neighbors
   */
  int numberToReturn;

  /*
   * Check to see if there is a tag...
   */
  if(tagPresent && tagID > -1)
  {
    /* First we check to see if we didn't detect a face (Likely a passive cube)
     * Then we only return the tag ID...
     */
    if(tagFace == -1)
    {
      numberToReturn = tagID*100;
    }
    else
    {
      numberToReturn = abs(tagID*100) + abs(tagFace*10) + abs(tagAngle);
    }
  }
  /* This means that there is no tag, we return -1;
   */
  else
  {
    numberToReturn = -1;
  }

  switch(this->IOExpanderAddress)
  {
    case 32:
      f0 = numberToReturn;
      break;

    case 33:
      f1 = numberToReturn;
      break;

    case 34:
      f2 = numberToReturn;
      break;

    case 35:
      f3 = numberToReturn;
      break;

    case 36:
      f4 = numberToReturn; 
      break;

    case 37:
      f5 = numberToReturn;
      break;
  }
  
return(tagPresent); 
}


bool Face::updateIOExpander()
/*
 * The IO Expander is the "brain" for each face. The IO Expander controlls 16 different outputs
 * some of which are connected to various items... Like LED's, or to the sensors.
 * There are two bytes (this->IOExpanderState[0], and this->IOExpanderState[1] which 
 * contain bit fields representing the 16 possible bits of out puts.
 * This function actually pushes the 16 values to the IO Expander
 */
{
  bool success;
  Wire.beginTransmission(this->IOExpanderAddress);
  Wire.write(this->IOExpanderState[0]);
  Wire.write(this->IOExpanderState[1]);
  
  if(Wire.endTransmission() == 0){return(true);}
  else                           {return(false);}
}

bool Face::readAmbient(bool activate)
/*
 * Each Face circuit board contains a light sensor which can be accessed through
 * I2C - the sensors return a value from 1 to ~4000. 
 * The arguement (bool activate) is true if we first need to activate the sensor prior to reading it.
 * If the sensor is to be read repeaditly, then activate should only be true the first time, and then
 * false for every subsequent read.
 */
{
  if(activate)
  {
    activateLightSensor(this->ambientSensorAddress);
  }
  wifiDelay(19);
  int reading = 0;
  Wire.beginTransmission(this->ambientSensorAddress); 
  Wire.write(byte(0x8C)); // this is the register where the Ambient values are stored
  Wire.endTransmission();
  Wire.requestFrom(this->ambientSensorAddress, 2);
  if (2 <= Wire.available()) //ambientLight  = twiBuf[0] << 2;
  {
    reading = Wire.read();
    reading |= Wire.read()<<8;
  }
  this->ambientBuffer.push(reading); // adds the sensor value to the buffer 
  return(true);
}

void Face::forceUpdateAmbientValue(int value)
/*
 * This pushes a "fake" value to the ambient Value buffer.
 * We can use this to zero out a face when we are trying to determine which face is the brightest
 */
{
  this->ambientBuffer.push(value); // adds the sensor value to the buffer 
}

bool Face::updateAmbient(bool activate)
/*
 * This function reads the ambient sensor on this instance of "Face" 
 */
{
this->readAmbient(activate);
return(true);
}

int Face::processLightData(int samplesTaken)
/*
 * This code will sample the light sensor on any face and return the following
 * 
 * 2 = light mostly on
 * 1 = light on some of the time
 * 0 = did not see any light
 */
{
  int endResult = 0;
  int tempResult = 0;
  int thresholdValue = 3400;
  for(int indexx = 0; indexx < samplesTaken; indexx++)
  {
    if(this->returnAmbientValue(indexx) > thresholdValue)
      tempResult++;
  }
  if(tempResult > samplesTaken/2)
    endResult = 2;
    
  else if(tempResult > samplesTaken/4)
    endResult = 1;
    
  if(MAGIC_DEBUG)
  {
    Serial.print("PROCESS LIGHT DATA RESULT is: ");
    Serial.println(endResult);
  }
  return(endResult);
}

void Face::blinkOutMessage(int digit)
{
  if(MAGIC_DEBUG)
  {
     Serial.println("Starting to Blink Out a Digit");
     Serial.print("Digit to blink out is... ");Serial.println(digit);
  }
  int startTime = millis();
  if(digit > 0)
  {
    this->turnOnFaceLEDs(1, 1, 1, 1);
  }
  else
  {
    return;
  }
  while((millis()-startTime) < digit*100) // while the timer is still going
  {
    wifiDelay(5);
  }
  this->turnOffFaceLEDs();
  return;
}

void Face::blinkRing(int delayLength, int numberOfTimes)
{
  for(int i = 0; i < numberOfTimes; i++)
  {
    this->turnOnFaceLEDs(1, 0, 0, 0);
    wifiDelay(delayLength);
    this->turnOnFaceLEDs(0, 1, 0, 0);
    wifiDelay(delayLength);
    this->turnOnFaceLEDs(0, 0, 1, 0);
    wifiDelay(delayLength);
    this->turnOnFaceLEDs(0, 0, 0, 1);
    wifiDelay(delayLength);
  }
  this->turnOffFaceLEDs();
}

int Face::returnAmbientValue(int index)
{
  return(this->ambientBuffer.access(index));
}

int Face::returnMagnetStrength_A(int index)
{
  return(this->magnetStrengthBuffer_A.access(index));
}

int Face::returnMagnetAngle_A(int index)
{
  return(this->magnetAngleBuffer_A.access(index));
}

int Face::returnMagnetStrength_B(int index)
{
  return(this->magnetStrengthBuffer_B.access(index));
}

int Face::returnMagnetAngle_B(int index)
{
  return(this->magnetAngleBuffer_B.access(index));
}

TagType Face::returnNeighborType(int index)
{
  return(this->neighborTypeBuffer.access(index));
}

TagCommand Face::returnNeighborCommand(int index)
{
  return(this->neighborCommandBuffer.access(index));
}

int Face::returnNeighborID(int index)
{
  return(this->magnetAngleBuffer_B.access(index));
}

bool Face::returnNeighborPresence(int index)
{
  return(this->neighborPresenceBuffer.access(index));
}

int Face::returnNeighborAngle(int index)
{
  return(this->neighborAngleBuffer.access(index));
}

int Face::returnNeighborFace(int index)
{
  return(this->neighborFaceBuffer.access(index));
}

int Face::returnNeighborLightDigit(int index)
{
  return(this->neighborLightDigitBuffer.access(index));
}
/**       V 
 *   0111 1010
 * & 1111 0111  <--- how do we generate this one?
 * ------------
 *   0111 0010
 *   
 *   (1 << 3) = 
 *   0000 1000  then we bitwise NOT
 *   1111 0111  And this is what we need to bitwise AND
 */ 

bool Face::setPinLow(int pin)
{
  if((pin < 8) && (pin >= 0))
  {
    this->IOExpanderState[0] &= ~(1 << pin);
  }
  else if ((pin > 7) && (pin < 16))
  {
    this->IOExpanderState[1] &= ~(1 << (pin - 8));
  }
}

bool Face::setPinHigh(int pin)
{
    if(pin < 8 && pin >= 0)
  {
    this->IOExpanderState[0] |= (1 << pin);
  }
  else if (pin > 7 && pin < 16)
  {
    this->IOExpanderState[1] |= (1 << (pin - 8));
  }
}



bool Face::isThereNeighbor()
{
  this->enableSensors();
  wifiDelay(20);
  int mag_A = readMagnetSensorFieldStrength(this->faceMagnetAddresses[0]);
  int mag_B = readMagnetSensorFieldStrength(this->faceMagnetAddresses[1]);
  this->disableSensors();
  if(((mag_A < 250) && (mag_A > 0)) &&
   ((mag_B < 250) && (mag_B > 0)))
  {
    return(true);
  }
  else
  {
    return(false);
  }
}

bool Face::updateMagneticBarcode()
/*
 * This code actually reads and pushes the values from the magnetic sensors
 * into the buffers. Returns true if it successfully communicates with both 
 * sensors. False if either sensor is not able to be accessed.
 */
{
  this->getMagnetEncoderAddresses(this->faceMagnetAddresses);
  wifiDelay(10);
  float AMS5048_scaling_factor = 45.5111;

  int angle_A  = round(readMagnetSensorAngle(this->faceMagnetAddresses[0])/AMS5048_scaling_factor);
  int mag_A    = readMagnetSensorFieldStrength(this->faceMagnetAddresses[0]);
  int angle_B  = round(readMagnetSensorAngle(this->faceMagnetAddresses[1])/AMS5048_scaling_factor);
  int mag_B    = readMagnetSensorFieldStrength(this->faceMagnetAddresses[1]);

  this->magnetAngleBuffer_A.push(angle_A);  // push these values to a circular buffer 
  this->magnetStrengthBuffer_A.push(mag_A); // push these values to a circular buffer 
 
  this->magnetAngleBuffer_B.push(angle_B);  // push these values to a circular buffer 
  this->magnetStrengthBuffer_B.push(mag_B); // push these values to a circular buffer 
  
  if((mag_A == 0) || (mag_B == 0))  // if either sensor shows a field strength of 0 then it is disconnected
    {
      return(false);
    }
  else
    {
      return(true);
    }
}


bool Face::enableSensors()
{
  this->IOExpanderState[0] &= ~(1 << FB_EN1);
  delay(10);
  return(this->updateIOExpander());
}

bool Face::turnOffFaceLEDs()
{
/* Turns on the four white LED's on each face
* (bool LED_A = true, bool LED_B = true, bool LED_C = true, bool LED_D = true);
*/
  this->setPinHigh(this->led_A);
  this->setPinHigh(this->led_B);
  this->setPinHigh(this->led_C);
  this->setPinHigh(this->led_D);
  return(this->updateIOExpander());
}

bool Face::turnOnFaceLEDs(bool LED_A, bool LED_B,  bool LED_C,  bool LED_D)
{
  /* Turns on the four white LED's on each face
   * (bool LED_A = true, bool LED_B = true, bool LED_C = true, bool LED_D = true);
   */
   //if(MAGIC_DEBUG){Serial.println("WHAT THE BROBRO?");}
  if(LED_A)
    this->setPinLow(this->led_A);
  else
    this->setPinHigh(this->led_A);
    
  if(LED_B)
    this->setPinLow(this->led_B);
  else
    this->setPinHigh(this->led_B);
    
  if(LED_C) 
    this->setPinLow(this->led_C);
  else
    this->setPinHigh(this->led_C);
    
  if(LED_D) 
    this->setPinLow(this->led_D);
  else
    this->setPinHigh(this->led_D);
    
  return this->updateIOExpander();
}

bool Face::disableSensors()
/*
* 
*/
{
  this->IOExpanderState[0] |= (1 << FB_EN1); // Toggles FB_EN1 to deactivate sensors
  return(this->updateIOExpander());
}

void activateLightSensor(int i2caddress)
{
  //Serial.println("BEGINNING ACTIVATE AMBIENT...");
  delay(10);
  Wire.beginTransmission(i2caddress); 
  Wire.write(byte(0x80)); // 1000 0000 - Selects command register
  Wire.write(byte(0x03)); // 0000 0010 - Activates device
  Wire.endTransmission();
  delay(3);
  Wire.beginTransmission(i2caddress); 
  Wire.write(byte(0x81)); // 1000 
  Wire.write(byte(0x10)); // Sets integration time to 15 ms ... // 00010XX sets gain to 16x
  Wire.endTransmission();
  // 0x00 = 15ms
  // 0x10 = 100 ms
}

int readMagnetSensorAngle(int i2cAddress) {
  // Returns the angle of the measured magnet as a 14 bit number
  int value = magnetSensorRead(i2cAddress, byte(0xFF));
  return(value);
}

int readMagnetSensorFieldStrength(int i2cAddress) {
  // AGC is the "strength" of the magnet returned as an 8-bit number, 255 = magnet field is too weak, 
  //0 = very strong magnetic field.
  return(magnetSensorRead(i2cAddress, byte(0xFA)));
}

int magnetSensorRead(int i2cAddress, byte dataRegisterAddress) {
  // read either the angle or the field strength of the AMS5048
  Wire.beginTransmission(i2cAddress);
  Wire.write(dataRegisterAddress);
  Wire.endTransmission();
  Wire.requestFrom(i2cAddress, 2); // request 2 bytes

  if (2 <= Wire.available())
  {
    int sensorValue;
    sensorValue = Wire.read();
    sensorValue = sensorValue << 6;
    sensorValue |= Wire.read();
    return sensorValue;
  }
  return -1;
}

int returnFaceNumber(int magDigit)
{
         if((magDigit == 29) || (magDigit == 28))
            {return 0;}
    else if((magDigit == 27) || (magDigit == 26))
            {return 1;}
    else if((magDigit == 25) || (magDigit == 24))
            {return 2;}
    else if((magDigit == 23) || (magDigit == 22))
            {return 3;}
    else if((magDigit == 21) || (magDigit == 20))
            {return 4;}
    else if((magDigit == 19) || (magDigit == 18))
            {return 5;}
    else
            {return -1;}
}     


