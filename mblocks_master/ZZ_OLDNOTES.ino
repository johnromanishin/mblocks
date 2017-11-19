//bool Cube::goToPlaneIncludingFaces(int face1, int face2, SerialDecoderBuffer* buf)
///*
// * I know there is a more elegant way to do this without all of the && and ||... but this works. Please forgive me.
// */
//{
//  returnPlane(int face1, int face2)
//  bool result = false;
//  if(((face1 == 0) && (face2 == 1)) || ((face2 == 0) && (face1 == 1)) ||
//     ((face1 == 1) && (face2 == 2)) || ((face2 == 1) && (face1 == 2)) ||
//     ((face1 == 2) && (face2 == 3)) || ((face2 == 2) && (face1 == 3)) ||
//     ((face1 == 3) && (face2 == 0)) || ((face2 == 3) && (face1 == 0)))
//  {
//    result = this->setCorePlane(returnPlane(face1, face2), buf, 8000);
//  }
//
//  else if(((face1 == 0) && (face2 == 4)) || ((face2 == 0) && (face1 == 4)) ||
//          ((face1 == 4) && (face2 == 2)) || ((face2 == 4) && (face1 == 2)) ||
//          ((face1 == 2) && (face2 == 5)) || ((face2 == 2) && (face1 == 5)) ||
//          ((face1 == 5) && (face2 == 0)) || ((face2 == 5) && (face1 == 0)))
//  {
//    result = this->setCorePlane(PLANE0425, buf, 8000);
//  }
//  
//  else if(((face1 == 1) && (face2 == 4)) || ((face2 == 1) && (face1 == 4)) ||
//          ((face1 == 4) && (face2 == 3)) || ((face2 == 4) && (face1 == 3)) ||
//          ((face1 == 3) && (face2 == 5)) || ((face2 == 3) && (face1 == 5)) ||
//          ((face1 == 5) && (face2 == 1)) || ((face2 == 5) && (face1 == 1)))
//  {
//    result = this->setCorePlane(PLANE1453, buf, 8000);
//  }
//  
//  return(result);
//}


//PlaneEnum Cube::findPlaneStatus()
//{
//  PlaneEnum likelyStatus = PLANEERROR; 
//  if(this->updateBothIMUs()) // This is true if we get valid readings from both IMUs
//  {
//    const int validPlaneThreshold = 160;
//    const int gyroMovingThreshold  = 1100;
//    
//    int32_t coreAccel[3] =   {this->axCoreBuffer.access(0),     
//                              this->ayCoreBuffer.access(0),
//                              this->azCoreBuffer.access(0)};
//    int32_t frameAccel[3] =  {this->axFrameBuffer.access(0), 
//                              this->ayFrameBuffer.access(0),
//                              this->azFrameBuffer.access(0)};
//    int32_t transformed[3][3];
//    int32_t distance[3];
//    
//    int     centralGyro[3] =  {this->gxCoreBuffer.access(0),
//                               this->gyCoreBuffer.access(0),
//                               this->gzCoreBuffer.access(0)};
//                               
//    int     sumOfGyros = (abs(centralGyro[0]) + abs(centralGyro[0]) + abs(centralGyro[0]));
//    
//    //test each of the rotation matricies.  Store all results for debug purposes.
//    for(int i = 0; i < 3; i++)
//    {
//      apply_3x3_mult(&rotationMatricies[i][0][0], coreAccel, &transformed[i][0]);
//      distance[i] = vector_distance_squared(&transformed[i][0], frameAccel);
//    }
//    int mindist = distance[0];
//    int minidx = 0;
//    for(int i = 1; i < 3; i++)
//    {
//      if(distance[i] < mindist)
//      {
//        minidx = i;
//        mindist = distance[i];
//      }
//    }
//
//    
//    if(sumOfGyros > gyroMovingThreshold)
//    {
//      this->currentPlane = PLANEMOVING;
//      likelyStatus = PLANEMOVING;
//    }
//    else if((distance[minidx] < validPlaneThreshold) && 
//            (sumOfGyros < gyroMovingThreshold))
//    {
//      this->currentPlane = planeEnumMap[minidx];
//      likelyStatus = planeEnumMap[minidx];
//    }
//    else
//    { 
//      this->currentPlane = PLANENONE;
//      likelyStatus = PLANENONE;
//    }  
//    //this->currentPlane = -1;
//  }
//  
//  else // this gets called if reading one of the IMUs has failed
//  {
//    this->currentPlane = PLANEERROR;
//    likelyStatus = PLANENONE;
//  }
//  ///////////////////////////////////
//  
//  String message = "";
//  if(likelyStatus == PLANE0123)       {message = "PLANE0123";}
//  else if(likelyStatus == PLANE0425)  {message = "PLANE0425";}
//  else if(likelyStatus == PLANE1453)  {message = "PLANE1453";}
//  else if(likelyStatus == PLANENONE)  {message = "PLANENONE";}
//  else if(likelyStatus == PLANEMOVING){message = "PLANEMOVING";}
//  else if(likelyStatus == PLANEERROR) {message = "PLANEERROR";}
//  //String(sumOfGyros);               
//  StaticJsonBuffer<512> jsonBuffer; //Space Allocated to store json instance
//  JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
//  root["msg"] = message;       
//  root["cmd"]  = "debugMSG";  
//  root["cubeID"] = -1;
//  String newStr;
//  root.printTo(newStr); 
//  mesh.sendBroadcast(newStr);
//  
/////////////////////////
//  return(likelyStatus);
//}

//if(faceVersion == 0) // Alternate method for Old Face Version
//  {
//    switch (face)
//    {
//      case 0: this->CornerRGB(1,1,r,g,b); this->CornerRGB(1,0,r,g,b); 
//              this->CornerRGB(2,0,r,g,b); this->CornerRGB(2,1,r,g,b); 
//              this->CornerRGB(3,1,0,0,0); this->CornerRGB(3,0,0,0,0); 
//              this->CornerRGB(4,0,0,0,0); this->CornerRGB(4,1,0,0,0);
//              break;
//              
//      case 1: this->CornerRGB(2,1,r,g,b); this->CornerRGB(2,0,r,g,b); 
//              this->CornerRGB(3,0,r,g,b); this->CornerRGB(3,1,r,g,b);
//              this->CornerRGB(1,1,0,0,0); this->CornerRGB(1,0,0,0,0); 
//              this->CornerRGB(4,0,0,0,0); this->CornerRGB(4,1,0,0,0);
//              break;
//              
//      case 2: this->CornerRGB(3,1,r,g,b); this->CornerRGB(3,0,r,g,b); 
//              this->CornerRGB(4,0,r,g,b); this->CornerRGB(4,1,r,g,b);
//              this->CornerRGB(1,1,0,0,0); this->CornerRGB(1,0,0,0,0); 
//              this->CornerRGB(2,0,0,0,0); this->CornerRGB(2,1,0,0,0);
//              break;
//              
//      case 3: this->CornerRGB(4,1,r,g,b); this->CornerRGB(4,0,r,g,b); 
//              this->CornerRGB(1,0,r,g,b); this->CornerRGB(1,1,r,g,b);
//              this->CornerRGB(2,1,0,0,0); this->CornerRGB(2,0,0,0,0); 
//              this->CornerRGB(3,0,0,0,0); this->CornerRGB(3,1,0,0,0);
//              break;
//              
//      case 4: for(int i=1;i<=4;i++) 
//              {this->CornerRGB(i,1,r,g,b); this->CornerRGB(i,0,0,0,0);}                         
//              break;
//              
//      case 5: for(int i=1;i<=4;i++) 
//              {this->CornerRGB(i,0,r,g,b); this->CornerRGB(i,1,0,0,0);}                         
//              break;
//    }
//    return(true);
//  }
//
//
// if(faceVersion == 0) // Alternate method for Old Face Version
//    {
//      int final_reading = 0;
//      int lightSensorGain = 10;      
//      for(int i = 0; i < 3; i++) // We read the actual sensor 3 times, and return average
//        { 
//          int reading = 0;
//          delay(6);
//          Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset); 
//          Wire.write(byte(0x10)); // this is the register where the ambient values are stored
//          Wire.endTransmission();
//          Wire.requestFrom((this->IOExpanderAddress - this->versionOffset), 2);
//          if (2 <= Wire.available()) //ambientLight  = twiBuf[0] << 2;
//            {
//              reading =  Wire.read()<<1;     //  ambientLight |= twiBuf[1] >> 6;
//              reading |=  Wire.read()>>7;   // Bit shifting for ambient values
//            }
//          else 
//            {
//              return(false);
//            }
//          final_reading += reading*lightSensorGain;
//        }
//      this->ambientBuffer.push(final_reading);
//      return(true);
//    }
//
//  else   //// Code runs for Regular faceVersion
//
//if(faceVersion == 0) // Alternate method for Old Face Version
//   {   
//      int dataByte;
//      dataByte  = LED_A ? 0x01 : 0x00;
//      dataByte |= LED_B ? 0x02 : 0x00;
//      dataByte |= LED_C ? 0x04 : 0x00;
//      dataByte |= LED_D ? 0x08 : 0x00;
//      Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset);
//      Wire.write(byte(0x20)); // register address for IRled's
//      Wire.write(byte(dataByte));
//      Wire.endTransmission();
//      return(true);
//  }

//
//  if(faceVersion == 0) // Alternate method for Old Face Version
//  {
//    Wire.beginTransmission(this->IOExpanderAddress - this->versionOffset);// this goes from 0x20 to 0x01 addresses
//    Wire.write(byte(0x43)); // access FBRXEN register
//    Wire.write(byte(0x00)); // sets FBRXEN register to ACTIVE
//    Wire.endTransmission(); 
//    this->turnOffFaceLEDs(); // On version 0 sensors are turned off using the same pins as the LEDs
//    return(true);
//  }


//#ifndef SENSATION_H
//#define SENSATION_H
//
//#include "Defines.h"
//#include <Arduino.h>
//
///// Sensor Helper Functions ///
//int readAmbient(int address);
//void activateLightSensor(int address);
//void readGyroAndAccel(int i2cAddress);
//int readMagnetSensorAngle(int i2cAddress);
//int readMagnetSensorFieldStrength(int i2cAddress);
//int magnetSensorRead(int i2cAddress, byte dataRegisterAddress);
//void wakeMPU(int i2cAddress);
//
//#endif

//#include <Wire.h>
//#include "Sensation.h"
////
//int readAmbient(int address)
//{
//  activateLightSensor(address);
//  delay(15);
//  int reading = 0;
//  Wire.beginTransmission(address); 
//  Wire.write(byte(0x8C)); // this is the register where the Ambient values are stored
//  Wire.endTransmission();
//  Wire.requestFrom(address, 2);
//  if (2 <= Wire.available()) 
//  {
//    reading = Wire.read();
//    reading |= Wire.read()<<8;
//  }
//  return reading;
//}
//
//void activateLightSensor(int address)
//{
//  Wire.beginTransmission(address); 
//  Wire.write(byte(0x80)); // 1000 0000 - Selects command register
//  Wire.write(byte(0x03)); // 0000 0010 - Activates device
//  Wire.endTransmission();
//  Wire.beginTransmission(address); 
//  Wire.write(byte(0x81));
//  Wire.write(byte(0x10)); // Sets integration time to 15 ms ... // 00010XX sets gain to 16x
//  // 0x10 = 100 ms
//  // 0x10 = 14ms w/ 16x High Gain
//  Wire.endTransmission();
//}
//
//int readMagnetSensorAngle(int i2cAddress) {
//  // Returns the angle of the measured magnet as a 14 bit number
//  int value = magnetSensorRead(i2cAddress, byte(0xFF));
//  return(value);
//}
//
//int readMagnetSensorFieldStrength(int i2cAddress) {
//  // AGC is the "strength" of the magnet returned as an 8-bit number, 255 = magnet field is too weak, 0 = very strong magnetic field.
//  return(magnetSensorRead(i2cAddress, byte(0xFA)));
//}
//
//int magnetSensorRead(int i2cAddress, byte dataRegisterAddress) {
//  // read either the angle or the field strength of the AMS5048
//  Wire.beginTransmission(i2cAddress);
//  Wire.write(dataRegisterAddress);
//  Wire.endTransmission();
//  Wire.requestFrom(i2cAddress, 2); // request 2 bytes
//
//  if (2 <= Wire.available())
//  {
//    int sensorValue;
//    sensorValue = Wire.read();
//    sensorValue = sensorValue << 6;
//    sensorValue |= Wire.read();
//    return sensorValue;
//  }
//  return -1;
////}
//
//if(DEBUG1)
//    {    
//         if(targetCorePlane == PLANENONE){Serial.println("PLANENONE");}
//    else if(targetCorePlane == PLANEERROR){Serial.println("PLANEERROR");}
//    else if(targetCorePlane == PLANEMOVING){Serial.println("PLANEMOVING");}
//    else if(targetCorePlane == PLANE1453){Serial.println("PLANE1453");}
//    else if(targetCorePlane == PLANE0425){Serial.println("PLANE0425");}
//    else if(targetCorePlane == PLANE0123){Serial.println("PLANE0123");}
//    }











#include <Arduino.h>
//#include "MagTag.h"
//#include "Defines.h"
//
////typedef struct Tag
////{
////  TagType type;   //
////  int angle ; // Either -1, 0, 1, 2 or 3 - corresponding to 90 deg angle
////  int id; // ID or message code attached to tag
////  int face; // face number (0,1,2,3,4,5) associated with a cube
////  int strength; // Validitity of the tag, basically just agc1+agc2
////  TagCommand command; // Text of command or behavior to go to... if it exists
////} Tag;
//
///*
// * Analyze Tag takes as arguements two magnetic sensor angles, and two 
// * corresponding AGC (Valid Magnetic field strength) values.
// */
// 
////void analyzeTag(int angle1, int agc1, int angle2, int agc2, Tag* t)
////{
////  int strengthThreshold = 450;
////  int magDigit1 = 0;
////  int magDigit2 = 0;
////        if (agc1 == 0 || agc1 >= 255)       {magDigit1 = 0;}
////   else if (angle1 < 6 || angle1 > 354)     {magDigit1 = 1;}
////   else                                     {magDigit1 = int(angle1 + 18)/12;}
////
////        if (agc2 == 0 || agc2 >= 255)       {magDigit2 = 0;}
////   else if(angle2 < 6 || angle2 > 354)      {magDigit2 = 1;}  
////   else                                     {magDigit2 = int(angle2 + 18)/12;}
////  //
////  t->strength = agc1+agc2; // this is a measurement of how accurate the tag strength is
////  t->type = TAGTYPE_INVALID;    // Resets all of these values
////  t->angle = -1; //             // Resets all of these values
////  t->id = -1; //                // Resets all of these values
////  t->face = -1; //              // Resets all of these values
////  t->command = TAGCOMMAND_NONE; // Resets all of these values
////      
////  if(((agc1+agc2) > strengthThreshold) || (t->strength == 0)) // this means there isn't a valid tag
////    {
////      return; // DOES NOT EVAUATE ANYTHING ELSE EXITS PROGRAM here
////    }
/////*============================================================================================================
//// * CHECK IF TAG REPRESENTS A MODULE
//// */
////  if((magDigit1 >= 17 && magDigit1 <= 29) &&  // Means magdigit1 is a faceID
////     (magDigit2 >= 1 && magDigit2 <= 17))     // Means magdifit2 stores an ID # 
////    {
////      t->type = TAGTYPE_REGULAR_CUBE;
////      t->id   = magDigit2;
////      t->face = returnFaceNumber(magDigit1);
////      if(magDigit1 % 2 ==0)
////      {
////        t->angle = 0;
////        if(DEBUG1){Serial.println("Found an actual Cube, *0* ");}
////      }
////      else
////      { 
////        t->angle = 1;
////        if(DEBUG1){Serial.println("Found an actual Cube, *1* ");}
////      }
////    }
////    
////  if((magDigit2 >= 17 && magDigit2 <= 29) &&  // Means magdigit1 is a faceID
////     (magDigit1 >= 1 && magDigit1 <= 17))     // Means magdifit2 stores an ID # 
////    {
////      t->type = TAGTYPE_REGULAR_CUBE;
////      t->id   = magDigit1;
////      t->face = returnFaceNumber(magDigit2);
////      if(magDigit2 % 2 ==0)
////        {
////          t->angle = 2;
////          if(DEBUG1){Serial.println("Found an actual Cube, *2* ");}
////        }
////      else                 
////        {
////          t->angle = 3;
////          if(DEBUG1){Serial.println("Found an actual Cube, *3* ");}
////        }
////    }
////  /*============================================================================================================
////  * CHECK IF TAG REPRESENTS A PASSIVE MODULE
////  */
////
////       if((magDigit1 == 15 || magDigit1 == 16 || magDigit1 == 17 || // Means magdigit1 is a faceID
////           magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2 ) &&
////          (magDigit2 == 8  || magDigit2 == 9  || magDigit2 == 10))     // Means magdigit2 stores an ID # 
////          {
////            t->type = TAGTYPE_PASSIVE_CUBE;
////              if(magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2)
////                  {
////                    t->angle = 2;
////                    if(DEBUG1){Serial.println("FOUND A PASSIVE CUBE ORIENTATION **2**");}
////                  }
////              else 
////                  {
////                    t->angle = 3;
////                    if(DEBUG1){Serial.println("FOUND A PASSIVE CUBE ORIENTATION **3**");}
////                  }
////          }
////       if((magDigit2 == 15 || magDigit2 == 16 || magDigit2 == 17  // Means magdigit1 is a faceID
////        || magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2 ) &&
////          (magDigit1 == 8  || magDigit1 == 9  || magDigit1 == 10))     // Means magdifit2 stores an ID # 
////          {
////            t->type = TAGTYPE_PASSIVE_CUBE;
////              if(magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2)
////                  {
////                    t->angle = 0;
////                    if(DEBUG1){Serial.println("FOUND A PASSIVE CUBE ORIENTATION **0** WOOO!");}
////                  }
////              else 
////                  {
////                    t->angle = 1;
////                    if(DEBUG1){Serial.println("FOUND A PASSIVE CUBE ORIENTATION **1** WOOO!");}
////                  }
////          }
////
////  /* ================================================================================================
////  * CHECK IF TAG REPRESENTS A COMMAND TAG
////  */
////  if(((magDigit1 - magDigit2 > -2) || (magDigit1 - magDigit2 < 2)) &&  // Means magdigit1 is a faceID
////     (magDigit1 != 17 && magDigit2 != 17) &&
////     (magDigit1 != 30 && magDigit2 != 30)
////    )
////  {
////    t->type = TAGTYPE_COMMAND;
////    if(magDigit1 == 25) // Sleep Command
////      t->command = TAGCOMMAND_SLEEP;
////    if(magDigit1 == 27) // Sleep Command
////      t->command = TAGCOMMAND_27;
////    if(magDigit1 == 23) // Sleep Command
////      t->command = TAGCOMMAND_23;
////    if(magDigit1 == 5) // Sleep Command
////      t->command = TAGCOMMAND_PURPLE;
////  }
////}
////
////int returnFaceNumber(int magDigit)
////{
////         if((magDigit == 29) || (magDigit == 28))
////            {return 0;}
////    else if((magDigit == 27) || (magDigit == 26))
////            {return 1;}
////    else if((magDigit == 25) || (magDigit == 24))
////            {return 2;}
////    else if((magDigit == 23) || (magDigit == 22))
////            {return 3;}
////    else if((magDigit == 21) || (magDigit == 20))
////            {return 4;}
////    else if((magDigit == 19) || (magDigit == 18))
////            {return 5;}
////    else
////            {return -1;}
////}     














//#ifndef MAGTAG_H
//#define MAGTAG_H
//
//#include <Arduino.h>
//
//#include "Cube.h"
//#include "Face.h"
//#include "Communication.h"
//#include "Initialization.h"
//#include "Defines.h"
////
////typedef enum TagType
////{
////  TAGTYPE_NOTHING,
////  TAGTYPE_INVALID,
////  TAGTYPE_REGULAR_CUBE,
////  TAGTYPE_PASSIVE_CUBE,
////  TAGTYPE_COMMAND
////} TagType;
//
////typedef enum TagCommand
////{
////  TAGCOMMAND_NONE,
////  TAGCOMMAND_SLEEP,
////  TAGCOMMAND_PURPLE,
////  TAGCOMMAND_27,
////  TAGCOMMAND_25,
////  TAGCOMMAND_23,
////  TAGCOMMAND_21,
////  TAGCOMMAND_19,
////} TagCommand;
//
////typedef struct Tag
////{
////  TagType type;   
////  /* 0 = not a valid tag
////   * 1 = Regular Cube Attached
////   * 2 = Passive Cube Attached
////   * 3 = COMMAND tag
////   */ 
////  int angle ; // Either -1, 0, 1, 2 or 3 - corresponding to 90 deg angle
////  int id; // ID or message code attached to tag
////  int face; // face number (0,1,2,3,4,5) associated with a cube
////  int strength; // Validitity of the tag, basically just agc1+agc2
////  TagCommand command; // Text of command or behavior to go to... if it exists
////} Tag;
////
////void analyzeTag(int angle1, int agc1, int angle2, int agc2, Tag*);
////int returnFaceNumber(int magDigit);
//
////#endif










//=============================================================================================
//=============================MAGNETIC TAGS CHECKING==========================================
//=============================================================================================
//bool returnNeighborPresence(int index);
//int returnNeighborID(int index);
//int returnNeighborAngle(int index);
//int returnNeighborFace(int index);
//TagType returnNeighborType(int index);
//TagCommand returnNeighborCommand(int index);
//    
//Behavior checkForMagneticTagsStandard(Cube* c, Behavior currentBehavior, SerialDecoderBuffer* buf)
//{
//  Behavior resultBehavior = currentBehavior;
//  Tag t;
//  for(int i = 0; i < 6; i++)
//    {
//      //int reflect = c->faces[i].returnReflectivityValue(0);
//      
//      analyzeTag(c->faces[i].returnMagnetAngle_A(0),          // This calls fubnction which reads tags, populate data fields
//                 c->faces[i].returnMagnetStrength_A(0),       //
//                 c->faces[i].returnMagnetAngle_B(0),          //
//                 c->faces[i].returnMagnetStrength_B(0), &t);  //  
//                     
//      //if((t.type != TAGTYPE_NOTHING))//&& // If a valid tag exists...
//      //(ESP.getChipId() != 13374829)) // if the ID # is not that of the base station
//            
//      //if(reflect < 400 && reflect > 50) 
//        //{c->lightCube(1,1,0); wifiDelay(300);}
//        
//      if(t.angle != -1) // This means we are seeing some "arrow"
//      {
//        c->lightFace(faceArrowPointsTo(i, t.angle),0,1,0);
//      }
//      
//      if(t.type == TAGTYPE_PASSIVE_CUBE)
//      {
//        resultBehavior = FOLLOW_ARROWS;
//      }
//      
//      if(t.command == TAGCOMMAND_SLEEP)
//      {
//        resultBehavior = relayBehavior(c, SLEEP);
//      }
//      if(t.command == TAGCOMMAND_PURPLE)
//      {
//        c->lightCube(1,0,1);
//        //====================SEND DEBUG =====================      
//        c->goToPlaneParallel(i, buf);
//        //mesh.sendBroadcast(Str);
//        //==================END SEND DEBUG ===================  
//      }
//      if(t.command == TAGCOMMAND_27)
//      {
//        resultBehavior = relayBehavior(c, TEAL);
//      }
//      
//      if(t.command == TAGCOMMAND_23)
//      {
//        resultBehavior = relayBehavior(c, PURPLE);
//      }
//      
//      if(t.command == TAGCOMMAND_19)
//      {
//        //====================SEND DEBUG =================== 
//        StaticJsonBuffer<512> jsonBuffer; //Space Allocated to store json instance
//        JsonObject& root2 = jsonBuffer.createObject(); // & is "c++ reference"
//        String message =  "  ID#: " + String(ESP.getChipId()) +
//                          "  on Face: " + String(i) + "  " +
//                          "  t.type: " + String(t.type) +
//                          "  t.angle: " + String(t.angle) +
//                          "  t.id: " + String(t.id) +
//                          "  t.face: " + String(t.face) +
//                          //"  t.Strength: " + String(t.strength) +
//                          "  t.command: " + String(t.command) +
//                          " --- " +
//                          " A0: " +  String(c->faces[i].returnMagnetAngle_A(0)) + 
//                          " S0: " +  String(c->faces[i].returnMagnetStrength_A(0)) + 
//                          " A1: " +  String(c->faces[i].returnMagnetAngle_B(0)) + 
//                          " S1: " +  String(c->faces[i].returnMagnetStrength_B(0));   
//                         // " Reflectivity: " + String(reflect);                    
//                          
//        root2["msg"] = message;       
//        root2["cmd"]  = "debugMSG";  
//        root2["cubeID"] = -1;
//        String newStr;
//        root2.printTo(newStr); 
//        mesh.sendBroadcast(newStr);
//        //====================END SEND DEBUG ===================       
//      }
//   }
//   return(resultBehavior);
//}


void analyzeTag(int angle1, int agc1, int angle2, int agc2, Tag* t)
{
  int strengthThreshold = 450;
  int magDigit1 = 0;
  int magDigit2 = 0;
        if (agc1 == 0 || agc1 >= 255)       {magDigit1 = 0;}
   else if (angle1 < 6 || angle1 > 354)     {magDigit1 = 1;}
   else                                     {magDigit1 = int(angle1 + 18)/12;}

        if (agc2 == 0 || agc2 >= 255)       {magDigit2 = 0;}
   else if(angle2 < 6 || angle2 > 354)      {magDigit2 = 1;}  
   else                                     {magDigit2 = int(angle2 + 18)/12;}
  //
  t->strength = agc1+agc2; // this is a measurement of how accurate the tag strength is
  t->type = TAGTYPE_INVALID;    // Resets all of these values
  t->angle = -1; //             // Resets all of these values
  t->id = -1; //                // Resets all of these values
  t->face = -1; //              // Resets all of these values
  t->command = TAGCOMMAND_NONE; // Resets all of these values
      
  if(((agc1+agc2) > strengthThreshold) || (t->strength == 0)) // this means there isn't a valid tag
    {
      return; // DOES NOT EVAUATE ANYTHING ELSE EXITS PROGRAM here
    }
/*============================================================================================================
 * CHECK IF TAG REPRESENTS A MODULE
 */
  if((magDigit1 >= 17 && magDigit1 <= 29) &&  // Means magdigit1 is a faceID
     (magDigit2 >= 1 && magDigit2 <= 17))     // Means magdifit2 stores an ID # 
    {
      t->type = TAGTYPE_REGULAR_CUBE;
      t->id   = magDigit2;
      t->face = returnFaceNumber(magDigit1);
      if(magDigit1 % 2 ==0)
      {
        t->angle = 0;
        if(DEBUG1){Serial.println("Found an actual Cube, *0* ");}
      }
      else
      { 
        t->angle = 1;
        if(DEBUG1){Serial.println("Found an actual Cube, *1* ");}
      }
    }
    
  if((magDigit2 >= 17 && magDigit2 <= 29) &&  // Means magdigit1 is a faceID
     (magDigit1 >= 1 && magDigit1 <= 17))     // Means magdifit2 stores an ID # 
    {
      t->type = TAGTYPE_REGULAR_CUBE;
      t->id   = magDigit1;
      t->face = returnFaceNumber(magDigit2);
      if(magDigit2 % 2 ==0)
        {
          t->angle = 2;
          if(DEBUG1){Serial.println("Found an actual Cube, *2* ");}
        }
      else                 
        {
          t->angle = 3;
          if(DEBUG1){Serial.println("Found an actual Cube, *3* ");}
        }
    }
  /*============================================================================================================
  * CHECK IF TAG REPRESENTS A PASSIVE MODULE
  */

       if((magDigit1 == 15 || magDigit1 == 16 || magDigit1 == 17 || // Means magdigit1 is a faceID
           magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2 ) &&
          (magDigit2 == 8  || magDigit2 == 9  || magDigit2 == 10))     // Means magdigit2 stores an ID # 
          {
            t->type = TAGTYPE_PASSIVE_CUBE;
              if(magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2)
                  {
                    t->angle = 2;
                    if(DEBUG1){Serial.println("FOUND A PASSIVE CUBE ORIENTATION **2**");}
                  }
              else 
                  {
                    t->angle = 3;
                    if(DEBUG1){Serial.println("FOUND A PASSIVE CUBE ORIENTATION **3**");}
                  }
          }
       if((magDigit2 == 15 || magDigit2 == 16 || magDigit2 == 17  // Means magdigit1 is a faceID
        || magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2 ) &&
          (magDigit1 == 8  || magDigit1 == 9  || magDigit1 == 10))     // Means magdifit2 stores an ID # 
          {
            t->type = TAGTYPE_PASSIVE_CUBE;
              if(magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2)
                  {
                    t->angle = 0;
                    if(DEBUG1){Serial.println("FOUND A PASSIVE CUBE ORIENTATION **0** WOOO!");}
                  }
              else 
                  {
                    t->angle = 1;
                    if(DEBUG1){Serial.println("FOUND A PASSIVE CUBE ORIENTATION **1** WOOO!");}
                  }
          }

  /* ================================================================================================
  * CHECK IF TAG REPRESENTS A COMMAND TAG
  */
  if((((magDigit1 - magDigit2) > -2) || ((magDigit1 - magDigit2) < 2)) &&  // Means magdigit1 is a faceID
     (magDigit1 != 17 && magDigit2 != 17) &&
     (magDigit1 != 30 && magDigit2 != 30)
    )
  {
    t->type = TAGTYPE_COMMAND;
    if(magDigit1 == 25) // Sleep Command
      t->command = TAGCOMMAND_SLEEP;
    if(magDigit1 == 27) // Sleep Command
      t->command = TAGCOMMAND_27;
    if(magDigit1 == 23 || magDigit1 == 24) // Sleep Command
      t->command = TAGCOMMAND_23;
    if(magDigit1 == 5) // Sleep Command
      t->command = TAGCOMMAND_PURPLE;
  }
}

