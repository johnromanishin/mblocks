// We want to implement classes for ease of reference as we construct and modify code.
// The classes we define here are the cube, the face.
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

bool Face::updateFace(int blinkOutDigit, bool checkForLightYo)
{
  bool success = 
        (this->enableSensors() 
      && this->updateAmbient()
      && this->turnOnFaceLEDs(0,0,1,0)
    //&& this->updateReflectivity()       
      && this->updateMagneticBarcode() // actually reads magnetic valuess
      && this->turnOffFaceLEDs());
  this->neighborPresenceBuffer.push(this->processTag()); // actually processes Tag... adds 
  
  if((this->returnNeighborType(0) == TAGTYPE_REGULAR_CUBE) && checkForLightYo) // checks for lightdigits...
  {
    this->turnOnFaceLEDs(1,1,1,1); // briefly turn on lights just for fun... || later change this to blink out actual digit
    delay(50);
    this->turnOffFaceLEDs(); // turn off all of the lights on your face...
    int lightDigit = this->checkForMessage(blinkOutDigit, 4000); // this samples light sensor at rate of 50 hz...
    if(this->isThereNeighbor()) // if we are still connected to a cube on this face...
    {
      this->neighborLightDigitBuffer.push(lightDigit); // add the lightDigit to the buffer
    }   
    else
    {
      this->neighborLightDigitBuffer.push(-1);         // if we were initially connected, but then disconnected, FAIL!
    }
  }
  this->disableSensors(); // turn off sensors...
  return(success);
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
  int strengthThreshold = 480;
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

int Face::checkForMessage(int blinkOutDigit, int waitTime)
/*  This code is blocking, and is intended to ONLY run
 *  on a face that is magnetically connected to a different 
 *  Cube.
 *  
 *  The goal of the code is to "listen" to the ambient light sensor
 *  and try to determine if it sees any valid "digits"
 *  A digit is as follows:
 *  blink for 100ms == "1"
 *  blink for 200ms == "2"
 *  blink for 300ms == "3"
 *  blink for 400ms == "4"
 *  blink for 500ms == "5"
 *  blink for 600ms == "6"
 *  blink for 700ms == "7"
 *  blink for 800ms == "8"
 *  blink for 900ms == "9"
 *  blink for 1000+ms== "10"
 *  
 *  It will also blink out a digit (int blinkOutDigit) every 1 second.
 */
{
  int cycles = waitTime/20;
  int howManyResults = 0;
  int result = 0;
  int counter = 0;
  int lengthOn = 0;
  int highLightThreshold = 4000;
  bool wasLastValueHigh = false;
  if(blinkOutDigit == 10)
  {
    this->turnOnFaceLEDs(1,1,1,1);
  }
  delay(10);
  this->updateAmbient(); // get initial reading
  while(this->returnAmbientValue(0) > highLightThreshold) // wait for it to go low...
  {
    this->updateAmbient(false);
    counter++;
    if(counter > 40) // if it never goes low... we return that we saw 6...
    {
      //Serial.println("WOOOOO!!! FUCK YAH");
      this->turnOffFaceLEDs();
      return(10);
    }
  }
  counter = 0;
  // we will get to this point once we see a low value so we know wasLastValueHigh == false;
  for(int i = 0; i < cycles; i++) // begin checking for messages... we gaurentee we have seen a low value as the first value
  {
    if((i % 50) && (blinkOutDigit < 10) && (blinkOutDigit > 0))
    {
       this->turnOnFaceLEDs(1,1,1,1);
    }
    if((i % (50 + 5*blinkOutDigit)) && (blinkOutDigit < 10) && (blinkOutDigit > 0))
    {
      this->turnOffFaceLEDs();
    }
    this->updateAmbient(false);
    if(this->returnAmbientValue(0) > highLightThreshold)
    {
      if(wasLastValueHigh == true)
        {
          counter++;
        }
      wasLastValueHigh = true;
    }
    else
    {
      if(wasLastValueHigh == true)
        {
          if(counter > 4)
          {
            howManyResults++;
            result = ((counter+3)/5);
            Serial.print("Counter result = ");
            Serial.println((counter+3)/5);
          }
          counter = 0;
        }
      wasLastValueHigh = false;
    }
    delay(1);
  }
this->turnOffFaceLEDs();
return(result);
 // for(int i = 0; // 
}

void Face::blinkOutMessage(int digit)
{
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
}

//bool Face::updateReflectivity()
///*
// * 
// */
//{
//  activateLightSensor(this->ambientSensorAddress);
//  bool error = false; // not yet implemented
//  delay(17); // 15ms delay to ensure integration period for light sensor works
//  int reading = 0;
//  Wire.beginTransmission(this->ambientSensorAddress); 
//  Wire.write(byte(0x8C)); // this is the register where the Ambient values are stored
//  Wire.endTransmission();
//  Wire.requestFrom(this->ambientSensorAddress, 2);
//  if (2 <= Wire.available())  // request data from the sensor
//    {
//      reading = Wire.read();
//      reading |= Wire.read()<<8;
//    }
//  this->reflectivityBuffer.push(reading); // adds the sensor value to the buffer
//  //Serial.print("Reflectivity: ");Serial.println(reading);
//  
//  return(true);
//}

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
  int mag_A = readMagnetSensorFieldStrength(this->faceMagnetAddresses[0]);
  int mag_B = readMagnetSensorFieldStrength(this->faceMagnetAddresses[1]);
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

bool Face::turnOnFaceLEDs(bool LED_A, bool LED_B, bool LED_C, bool LED_D)
{
  /* Turns on the four white LED's on each face
   * (bool LED_A = true, bool LED_B = true, bool LED_C = true, bool LED_D = true);
   */
  if(LED_A) {this->setPinLow(this->led_A);}
  if(LED_B) {this->setPinLow(this->led_B);}
  if(LED_C) {this->setPinLow(this->led_C);}
  if(LED_D) {this->setPinLow(this->led_D);}
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
