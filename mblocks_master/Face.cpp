// We want to implement classes for ease of reference as we construct and modify code.
// The classes we define here are the cube, the face.
#include "Defines.h"
#include "Cube.h"
#include "Sensation.h"
#include "Face.h"
#include <Wire.h> 

Face::Face()
  : ambientBuffer(ARRAY_SIZEOF(this->ambientData), this->ambientData),
    reflectivityBuffer(ARRAY_SIZEOF(this->ambientData), this->ambientData),
    magnetAngleBuffer_A(ARRAY_SIZEOF(this->magnetAngleData_A), this->magnetAngleData_A),
    magnetStrengthBuffer_A(ARRAY_SIZEOF(this->magnetStrengthData_A), this->magnetStrengthData_A),
    
    magnetAngleBuffer_B(ARRAY_SIZEOF(this->magnetAngleData_B), this->magnetAngleData_B),
    magnetStrengthBuffer_B(ARRAY_SIZEOF(this->magnetStrengthData_B), this->magnetStrengthData_B),
    
    neighborIDBuffer(ARRAY_SIZEOF(this->neighborIDData), this->neighborIDData),
    neighborInfoBuffer(ARRAY_SIZEOF(this->neighborInfoData), this->neighborInfoData)
    
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

bool Face::updateFace()
{
  bool success = 
      (this->enableSensors() 
    && this->updateAmbient()
    && this->turnOnFaceLEDs(0,0,1,0)    // TEMP
    && this->updateReflectivity()            // temp
    && this->updateMagneticBarcode()
    && this->turnOffFaceLEDs()
    && this->disableSensors()); 
  return(success);
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

bool Face::readAmbient()
{
delay(15); // 15ms delay to ensure integration period for light sensor works
int reading = 0;
Wire.beginTransmission(this->ambientSensorAddress); 
Wire.write(byte(0x8C)); // this is the register where the Ambient values are stored
Wire.endTransmission();
Wire.requestFrom(this->ambientSensorAddress, 2);
if (2 <= Wire.available())  // request data from the sensor
  {
    reading = Wire.read();
    reading |= Wire.read()<<8;
  }
this->ambientBuffer.push(reading); // adds the sensor value to the buffer 
}

bool Face::updateAmbient()
/*
 * This function reads the ambient sensor on this instance of "Face" 
 */
{
activateLightSensor(this->ambientSensorAddress);
this->readAmbient();
return(true);
}

int Face::checkForMessage(int waitTime)
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
 *  blink for 600+ms == "6"
 */
{
  int result = 0;
  int lengthOn = 0;
  if(!this->isThereNeighbor()) // if we are not connected... EXIT
  {
    return(result);
  }
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

bool Face::updateReflectivity()
/*
 * 
 */
{
  activateLightSensor(this->ambientSensorAddress);
  bool error = false; // not yet implemented
  delay(20); // 15ms delay to ensure integration period for light sensor works
  int reading = 0;
  Wire.beginTransmission(this->ambientSensorAddress); 
  Wire.write(byte(0x8C)); // this is the register where the Ambient values are stored
  Wire.endTransmission();
  Wire.requestFrom(this->ambientSensorAddress, 2);
  if (2 <= Wire.available())  // request data from the sensor
    {
      reading = Wire.read();
      reading |= Wire.read()<<8;
    }
  this->reflectivityBuffer.push(reading); // adds the sensor value to the buffer
  //Serial.print("Reflectivity: ");Serial.println(reading);
  return(true);
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
  return this->updateIOExpander();
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
