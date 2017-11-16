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
