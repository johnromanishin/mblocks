#include "Defines.h"
#include "Cube.h"
#include "Face.h"
#include <Wire.h> 
#include <Arduino.h>

Face::Face()
  : ambientBuffer(ARRAY_SIZEOF(this->ambientData), this->ambientData),
    reflectivityBuffer(ARRAY_SIZEOF(this->reflectivityData), this->reflectivityData),
    magnetAngleBuffer_A(ARRAY_SIZEOF(this->magnetAngleData_A), this->magnetAngleData_A),
    magnetStrengthBuffer_A(ARRAY_SIZEOF(this->magnetStrengthData_A), this->magnetStrengthData_A),
    
    magnetAngleBuffer_B(ARRAY_SIZEOF(this->magnetAngleData_B), this->magnetAngleData_B),
    magnetStrengthBuffer_B(ARRAY_SIZEOF(this->magnetStrengthData_B), this->magnetStrengthData_B),
    
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

//Face::Face(int IOExpanderAddress);//, int faceVersion = 1)
//  : ambientBuffer(ARRAY_SIZEOF(this->ambientData), this->ambientData),
//    magnetBuffer_A(ARRAY_SIZEOF(this->magnetData_A), this->magnetData_A),
//    magnetBuffer_B(ARRAY_SIZEOF(this->magnetData_B), this->magnetData_B),
//    
//    neighborIDBuffer(ARRAY_SIZEOF(this->neighborIDData), this->neighborIDData),
//    neighborFaceBuffer(ARRAY_SIZEOF(this->neighborFaceData), this->neighborFaceData),
//    neighborAttributeBuffer(ARRAY_SIZEOF(this->neighborAttributeData), this->neighborAttributeData)
//{
//  this->IOExpanderAddress = IOExpanderAddress;
//  //this->faceVersion = faceVersion;
//  this->IOExpanderState[0] = (this->IOExpanderState[1] = byte(0xff));
//}

bool Face::updateFace(bool blinkLEDs)
{
  bool updateSuccess = false;
  if(blinkLEDs == true) // this is true if we are trying to communicate through light blinking...
  {
  updateSuccess = (this->enableSensors() &&
                   this->updateAmbient(true) &&
                   this->turnOnFaceLEDs(0,0,1,0) &&
                   this->updateMagneticBarcode() && // actually reads magnetic valuess
                   this->turnOffFaceLEDs());
  }
  else
  {
  updateSuccess = (this->enableSensors() &&
                   this->updateAmbient(true) &&
                   this->updateMagneticBarcode()); // actually reads magnetic valuess
  }
  this->neighborPresenceBuffer.push(this->processTag()); // actually processes Tag... adds 
  
  if(this->returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE)
  {
    this->blinkRingDigit(2, 3);
    magicTheLight = true;
  }
  // if we are connected... and we are supposed to check for light, wait 2 seconds to try to find a message
  if((this->returnNeighborType(0) == TAGTYPE_REGULAR_CUBE) && blinkLEDs) // checks for lightdigits...
  {
    for(int i = 0; i < 121; i++) // take 120 light samples
      this->updateAmbient(false);
    this->neighborLightDigitBuffer.push(this->processLightData()); // add the lightDigit to the buffer
    if(magicTheLight)
        this->turnOnFaceLEDs(1,1,1,1);
  }
  else
  {
    this->neighborLightDigitBuffer.push(-2);
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
  bool tagPresent = false;
  int angle1 = this->returnMagnetAngle_A(0);
  int angle2 = this->returnMagnetAngle_B(0);
  int agc1 = this->returnMagnetStrength_A(0);
  int agc2 = this->returnMagnetStrength_B(0);
  int strengthThreshold = 490;
  int magDigit1 = 0;
  int magDigit2 = 0;
        if (agc1 == 0 || agc1 >= 255)       {magDigit1 = 0;}
   else if (angle1 < 6 || angle1 > 354)     {magDigit1 = 1;}
   else                                     {magDigit1 = int(angle1 + 18)/12;}

        if (agc2 == 0 || agc2 >= 255)       {magDigit2 = 0;}
   else if(angle2 < 6 || angle2 > 354)      {magDigit2 = 1;}  
   else                                     {magDigit2 = int(angle2 + 18)/12;}
  //
  
  int tagStrength = agc1+agc2; // this is a measurement of how accurate the tag strength is
  
  TagType tagType = TAGTYPE_INVALID;        // Resets all of these values
  TagCommand tagCommand = TAGCOMMAND_NONE;  // Resets all of these values
  int tagAngle = -1; //                     // Resets all of these values
  int tagID = -1; //                        // Resets all of these values
  int tagFace = -1; //                      // Resets all of these values
        
  if(((agc1+agc2) < strengthThreshold) && (tagStrength > 0)) // this means there is a valid tag!! woo!
  {
    tagPresent = true; // THIS MEANS WE HAVE A TAG!!
    /*============================================================================================================
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
    if((magDigit2 >= 17 && magDigit2 <= 29) &&  // Means magdigit1 is a faceID
     (magDigit1 >= 1 && magDigit1 <= 17))     // Means magdifit2 stores an ID # 
    {
      tagType = TAGTYPE_REGULAR_CUBE;
      tagID   = magDigit1;
      tagFace = returnFaceNumber(magDigit2);
      if(magDigit2 % 2 ==0)
          tagAngle = 2;
      else                 
          tagAngle = 3;
    }
    /*============================================================================================================
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
    if((magDigit2 == 15 || magDigit2 == 16 || magDigit2 == 17 ||  // Means magdigit1 is a faceID
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
    if((((magDigit1 - magDigit2) > -2) && ((magDigit1 - magDigit2) < 2)) &&  // if the difference between the two is small
       (magDigit1 != 17 && magDigit2 != 17) && (magDigit1 != 30 && magDigit2 != 30))
    {
      tagType = TAGTYPE_COMMAND;
      if(magDigit1 == 25) // Sleep Command
        tagCommand = TAGCOMMAND_SLEEP;
      if(magDigit1 == 27) // 
        tagCommand = TAGCOMMAND_27;
      if(magDigit1 == 23 || magDigit1 == 24) 
        tagCommand = TAGCOMMAND_23;
      if(magDigit1 == 5 || magDigit1 == 4) // change plane
        tagCommand = TAGCOMMAND_PURPLE;
      if((magDigit1 == 13) || (magDigit1 == 12))
        tagCommand = TAGCOMMAND_13_ESPOFF;
    }
  }
this->neighborTypeBuffer.push(tagType); 
this->neighborCommandBuffer.push(tagCommand); 
this->neighborFaceBuffer.push(tagFace); 
this->neighborAngleBuffer.push(tagAngle); 
this->neighborIDBuffer.push(tagID); 
return(tagPresent); 
}


bool Face::updateIOExpander()
{
  bool success;
  Wire.beginTransmission(this->IOExpanderAddress);
  Wire.write(this->IOExpanderState[0]);
  Wire.write(this->IOExpanderState[1]);
  if(Wire.endTransmission() == 0){return(true);}
  else                           {return(false);}
}

bool Face::readAmbient(bool activate)
{
  if(activate)
  {
    activateLightSensor(this->ambientSensorAddress);
  }
  delay(19);
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

int Face::processLightData()
/*  This code will process 60, or possibly... 100 samples of light data, 
 *  
 *        for each sample, it will break it up into sections of 20 samples so...
 *        0.... 23 | 24.... 47 | 48 ... 71 | 72... 95 | 96 ... 119 
 *index       0         1            2          3         4
 *
 *for each of these sections it will count total duty cycle...
 *or absolute number of # High
 *
 *and map them to the following bins
 *0-3   Periods High = 0
 *4-7   periods High = 1
 *8-11  periods High = 2
 *12-15 periods High = 3
 *16-19 Periods High = 4
 *20-23 Periods High = 5
 */
{
  int results[6] = {0,0,0,0,0,0};
  int endResult = -1;
  int index = 0;
  int tempResult = 0;
  int thresholdValue = 3700;

  for(int binIndex = 0; binIndex < 5; binIndex++)
  {
    tempResult = 0;
    for(int sample = 0; sample < 24; sample++)
      {
        if(this->returnAmbientValue(index) > thresholdValue)
          tempResult++;
        index++;
      }
    if(tempResult < 4)
      results[0]++;
    else if((tempResult > 3)  && (tempResult < 8))
      results[1]++;
    else if((tempResult > 7)  && (tempResult < 12))
      results[2]++;
    else if((tempResult > 11) && (tempResult < 16))
      results[3]++;
    else if((tempResult > 15) && (tempResult < 20))
      results[4]++;
    else if(tempResult > 19)
      results[5]++;
  }

  for(int tempIndex = 0; tempIndex < 6; tempIndex++) // go through the six bins from results[6]
  {
  
    if(results[tempIndex] > 2) // if we see 3 of the same number... we consider it a success... Voting based...
      endResult = tempIndex;
  }
//Serial.print("We Found the Following Result... from the LIGHT DIGIT CHECK: ");
//Serial.println(endResult);
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
    delay(5);
  }
  this->turnOffFaceLEDs();
  return;
}

void Face::blinkRingDigit(int digit, int numberOfTimes)
{
  int delayTime = 50;
  if(digit == 1) // 1/5
  {
    for(int i = 0; i < numberOfTimes; i++)
    {
      this->turnOnFaceLEDs(1, 0, 0, 0);
      delay(delayTime);
      this->turnOnFaceLEDs(0, 1, 0, 0);
      delay(delayTime);
      this->turnOnFaceLEDs(0, 0, 1, 0);
      delay(delayTime);
      this->turnOnFaceLEDs(0, 0, 0, 1);
      delay(delayTime);
      this->turnOffFaceLEDs();
      delay(delayTime);
    }
  }
  else if(digit == 2) //2/5
  {
    for(int i = 0; i < numberOfTimes; i++)
      {
        this->turnOnFaceLEDs(1, 1, 0, 0);
        delay(delayTime);
        this->turnOnFaceLEDs(0, 1, 1, 0);
        delay(delayTime);
        this->turnOnFaceLEDs(0, 0, 1, 1);
        delay(delayTime);
        this->turnOnFaceLEDs(1, 0, 0, 1);
        delay(delayTime);
        this->turnOffFaceLEDs();
        delay(delayTime);
      }
  }
  else if(digit == 3) //2/5
  {
    for(int i = 0; i < numberOfTimes; i++)
      {
        this->turnOnFaceLEDs(1, 1, 1, 0);
        delay(delayTime);
        this->turnOnFaceLEDs(0, 1, 1, 1);
        delay(delayTime);
        this->turnOnFaceLEDs(1, 0, 1, 1);
        delay(delayTime);
        this->turnOnFaceLEDs(1, 1, 0, 1);
        delay(delayTime);
        this->turnOffFaceLEDs();
        delay(delayTime);
      }
  }
  else if(digit == 4) //2/5
  {
    for(int i = 0; i < numberOfTimes; i++)
      {
        this->turnOnFaceLEDs(1, 1, 1, 1);
        delay(delayTime*4);
        this->turnOffFaceLEDs();
        delay(delayTime);
      }
  }
  else if(digit == 5) //5/5
  {
    for(int i = 0; i < numberOfTimes; i++)
      {
        this->turnOnFaceLEDs(1, 1, 1, 1);
        delay(delayTime*5);
      }
  }
this->turnOffFaceLEDs();
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

int Face::returnReflectivityValue(int index)
{
  return(this->reflectivityBuffer.access(index));
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
  delay(20);
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
  delay(10);
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
/*
* 
*/
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
  // 
  //Wire.endTransmission();
}

int readMagnetSensorAngle(int i2cAddress) {
  // Returns the angle of the measured magnet as a 14 bit number
  int value = magnetSensorRead(i2cAddress, byte(0xFF));
  return(value);
}

int readMagnetSensorFieldStrength(int i2cAddress) {
  // AGC is the "strength" of the magnet returned as an 8-bit number, 255 = magnet field is too weak, 0 = very strong magnetic field.
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
