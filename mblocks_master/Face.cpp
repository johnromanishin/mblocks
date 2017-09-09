// We want to implement classes for ease of reference as we construct and modify code.
// The classes we define here are the cube, the face.
#include "Defines.h"
#include "Cube.h"
#include "Sensation.h"
#include "Face.h"
#include <Wire.h> 

Face::Face()
  : ambientBuffer(ARRAY_SIZEOF(this->ambientData), this->ambientData),
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
    && this->turnOnFaceLEDs(1,0,0,0)
    && this->updateAmbient()
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

bool Face::updateAmbient()
{
  /*
   * This function reads the ambient sensor on this instance of "Face" 
   * There is the standard version (When faceVersion == 1), and a
   * legacy version (when faceVerstion ==0);
   */
  if(faceVersion == 0) // Alternate method for Old Face Version
    {
      int final_reading = 0;
      int lightSensorGain = 10;
      
      for(int i = 0; i < 3; i++) // We read the actual sensor 3 times, and return average
        { 
          int reading = 0;
          delay(6);
          Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset); 
          Wire.write(byte(0x10)); // this is the register where the ambient values are stored
          Wire.endTransmission();
          Wire.requestFrom((this->IOExpanderAddress - this->versionOffset), 2);
          if (2 <= Wire.available()) //ambientLight  = twiBuf[0] << 2;
            {
              reading =  Wire.read()<<1;     //  ambientLight |= twiBuf[1] >> 6;
              reading |=  Wire.read()>>7;   // Bit shifting for ambient values
            }
          else 
            {
              return(false);
            }
          final_reading += reading*lightSensorGain;
        }
      this->ambientBuffer.push(final_reading);
      return(true);
    }

  else   //// Code runs for Regular faceVersion
    {
      activateLightSensor(this->ambientSensorAddress);
      bool error = false; // not yet implemented
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
      return(true);
    }
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

/*
 * This code actually reads and pushes the values from the magnetic sensors
 * into the buffers. Returns true if it successfully communicates with both 
 * sensors. False if either sensor is not able to be accessed.
 */
bool Face::updateMagneticBarcode()
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
{
    /*
   * 
   */
  if(faceVersion == 0) // Alternate method for Old Face Version
  {
    Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset); // this goes from 0x20 to 0x01 addresses
    Wire.write(byte(0x43)); // access FBRXEN register
    Wire.write(byte(0x01)); // sets FBRXEN register to ACTIVE
    Wire.endTransmission(); 
    this->turnOnFaceLEDs(1,0,1,0);
  return(true);
  }
  
  //// Code runs for Regular faceVersion
  this->IOExpanderState[0] &= ~(1 << FB_EN1);
  return this->updateIOExpander();
}

bool Face::turnOffFaceLEDs()
{
 /* Turns on the four white LED's on each face
   * (bool LED_A = true, bool LED_B = true, bool LED_C = true, bool LED_D = true);
   */
   if(faceVersion == 0) // Alternate method for Old Face Version
  {   
      Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset);
      Wire.write(byte(0x20)); // register address for IRled's
      Wire.write(byte(0x00));
      Wire.endTransmission();
      return(true);
  }
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
   if(faceVersion == 0) // Alternate method for Old Face Version
   {   
      int dataByte;
      dataByte  = LED_A ? 0x01 : 0x00;
      dataByte |= LED_B ? 0x02 : 0x00;
      dataByte |= LED_C ? 0x04 : 0x00;
      dataByte |= LED_D ? 0x08 : 0x00;
      Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset);
      Wire.write(byte(0x20)); // register address for IRled's
      Wire.write(byte(dataByte));
      Wire.endTransmission();
      return(true);
  }
  if(LED_A) {this->setPinLow(this->led_A);}
  if(LED_B) {this->setPinLow(this->led_B);}
  if(LED_C) {this->setPinLow(this->led_C);}
  if(LED_D) {this->setPinLow(this->led_D);}
  return this->updateIOExpander();
}

bool Face::disableSensors()
{
   /*
   * 
   */
  if(faceVersion == 0) // Alternate method for Old Face Version
  {
    Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset);// this goes from 0x20 to 0x01 addresses
    Wire.write(byte(0x43)); // access FBRXEN register
    Wire.write(byte(0x00)); // sets FBRXEN register to ACTIVE
    Wire.endTransmission(); 
    this->turnOffFaceLEDs(); // On version 0 sensors are turned off using the same pins as the LEDs
    return(true);
  }
  
  //// Code runs for Regular faceVersion
  
  this->IOExpanderState[0] |= (1 << FB_EN1); // Toggles FB_EN1 to deactivate sensors
  return(this->updateIOExpander());
}
