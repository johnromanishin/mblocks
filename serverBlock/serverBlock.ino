/**
    This includes the main code for the ESP8266 board, aka the "Master Board"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/
// External Libraries
#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal
#include <painlessMesh.h>         // Wifi Mesh Library found on the internet  
#include <ArduinoJson.h>
#include <Arduino.h>              // library with basic arduino commands

#include "Interaction.h"
#include "Communication.h"
#include "Defines.h"
#include "Database.h"
#include <VL6180X.h>

// Header Files
// extern inboxEntry inbox;
// extern outboxEntry outbox[2];

void testOutboxes()
/*
 * This loads specific outboxes with specific message for testing and 
 * debugging purposes
 */
{
  //pushBlinkMessage(TESTCUBE_ID);
  //pushBlinkMessage(15);
  //pushBlinkMessage(4);
}

void interactWithRangeSensor()
{
  int rangeValue = readRangeSensor();
  if (rangeValue < 20)
  {
    //sendBroadcastMessage(createJsonStringFlood(-1, "sleep"));
    //Serial.println("Putting the cubes to sleep...");
    wifiDelay(300); // wait a bit to make sure it is a valid reading
    rangeValue = readRangeSensor();
    if(rangeValue < 20)
    {
      //Serial.println("Putting the cubes to sleep");
      //pushSleepMessage(15);
      //pushSleepMessage(4);
      testDatabase();
      wifiDelay(1000);
    }
  }
  
  else if (rangeValue > 20 && rangeValue < 50)
  {
    //sendBroadcastMessage(createJsonStringFlood(-1, "lightSeek"));
  }

  else if (rangeValue > 50 && rangeValue < 100)
  {
    //sendBroadcastMessage(createJsonStringFlood(-1, "attractive"));
  }

  else if (rangeValue > 100 && rangeValue < 200)
  {
    pushBlinkMessage(15);
    //pushBlinkMessage(4);
    pushBlinkMessage(TESTCUBE_ID);
    wifiDelay(300);
  }
}

void setup()
{
  Serial.begin(115200);
  initializeWifiMesh();
  initializeRangeSensor();
  initializeDatabase();
  wifiDelay(500);
  Serial.print("\n WIFI ID: ");
  Serial.println(mesh.getNodeId());
  espconn_tcp_set_max_con(6); // this is supposed to increase the maximum number of WIFI connections to 6
  wifiDelay(1000);  
  testOutboxes();
  Serial.println("Exiting Setup");
}

bool lineOfThree = false;
int foundFlag;
int loopCounter = 0;

void loop()
{
  updateBoxes(); // checks messages from the WiFi Message queue
  interactWithRangeSensor(); // checks the range value to send messages based on user input
  wifiDelay(5);
  if((loopCounter % 250) == 0)
  {
    pushBlinkMessage(TESTCUBE_ID);
    
    pushBlinkMessage(4);
    pushBlinkMessage(5);
    pushBlinkMessage(7);
    pushBlinkMessage(8);
    //Serial.println("Added message to que");
  }
  if((loopCounter % 333) == 0)
  {    
    pushBlinkMessage(10);
    pushBlinkMessage(13);
    
    pushBlinkMessage(2);
    pushBlinkMessage(1);
    pushBlinkMessage(3);
    pushBlinkMessage(9);
    
    //Serial.println("Added message to que");
  }
  if((loopCounter % 188) == 0)
  {
    pushBlinkMessage(TESTCUBE_ID);
    
    pushBlinkMessage(6);
    pushBlinkMessage(12);
    pushBlinkMessage(16);
    pushBlinkMessage(11);
    //Serial.println("Added message to que");
  }
  if((loopCounter % 50) == 0)
  {
    //pushBlinkMessage(TESTCUBE_ID);
    //Serial.println("Added message to que");
    testDatabase();
  }
  //  testThingNow();
  loopCounter++;
}















/////////// NOTES////


// Old version of finding if cubes are in a line...

//  foundFlag = 0;
//  for (int face = 1; face < 5; face <<= 1)
//  {
//    if ( (cubesState[TESTCUBE_ID][face] != 0) && (cubesState[TESTCUBE_ID][oppositeFace(face)] != 0) )
//    {
//      foundFlag = face;
//    }
//  }
//
//  if (foundFlag != 0)
//  {
//    if (lineOfThree == false)
//    {
//      lineOfThree = true;
//      pushReverseMessage(TESTCUBE_ID);
//
//    }
//  }
//  else if (lineOfThree == true)
//  {
//    lineOfThree = false;
//    pushForwardMessage(TESTCUBE_ID);
//  }

  
//bool Cube::isPlaneInPlaneOfFaces(int face1, int face2)
///*
// * 
// */
//{
//  bool result = false;
//  if((face1 > -1) && (face1 < 6))
//  {
//    if((face2 > -1) && (face2 < 6))
//    {
//      if(this->currentPlaneBuffer.access(0) == returnPlane(face1, face2))
//      {
//        result = true;
//      }
//    }
//  }
//  return(result);
//}
//
//bool Cube::goToPlaneIncludingFaces(int face1, int face2)
///*
// * 
// */
//{
//  bool result = false;
//  if((face1 > -1) && (face1 < 6))
//  {
//    if((face2 > -1) && (face2 < 6))
//    {
//      result = this->setCorePlaneSimple(returnPlane(face1, face2));
//    }
//  }
//  return(result);
//}
//
//bool Cube::isPlaneParallel(int faceExclude)
///*
// * 
// */
//{
//  bool result = false;
//  if((faceExclude == 4 )|| (faceExclude == 5))
//  {
//    if(this->currentPlaneBuffer.access(0) == PLANE0123)
//      result = true;
//  }
//  else if((faceExclude == 1) ||( faceExclude == 3))
//  {
//    if(this->currentPlaneBuffer.access(0) == PLANE0425)
//      result = true;
//  }
//  else if((faceExclude == 0) || (faceExclude == 2))
//  {
//    if(this->currentPlaneBuffer.access(0) == PLANE1453)
//      result = true;
//  }
//return(result);
//}

//int Cube::whichFaceHasNeighbor(int index)
///*
// * This is designed to return the face number for each face connection...
// * it works like this, you say
// * whichFaceHasNeighbor(0) // or default - and it returns the first face, counting from 0
// * that has a neighbor connected.
// * 
// * whichFaceHasNeighbor(1) will return the next connected face...
// * 
// * So if the cube has neighbors on faces, 2, 4, and 5,
// * this SHOULD be the result...
// * 
// * whichFaceHasNeighbor(0) -> 2
// * whichFaceHasNeighbor(1) -> 4
// * whichFaceHasNeighbor(2) -> 5
// * whichFaceHasNeighbor(3) -> -1 -- no faces...
// * 
// * Yeah I am sure there is a fancier way to do this...
// * but I don't want to think about it right at this moment.
// */
//{
//  int facesNeighbors[6] = {-1, -1, -1, -1, -1, -1}; 
//  
//  int facesCount = 0;
//  for(int face = 0; face < 6; face++)
//  {
//    if((this->faces[face].returnNeighborType(0) == TAGTYPE_REGULAR_CUBE) ||
//       (this->faces[face].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE))
//    {
//      facesCount++;
//      if(facesCount > 0)
//        facesNeighbors[facesCount-1] = face;
//    }
//  }
//return(facesNeighbors[index]);
//}
//int Cube::numberOfNeighbors(int index, bool doIlightFace)
///*
// * This function returns an integer number corresponding to how many 
// * neighbors that it thinks that it has.
// * 
// * if doIlightFace is TRUE, it will light up each face that it thinks that it has a neighbor on
// */
//{
//  int neighbors = 0; // initialize a empty integer
//  for(int face = 0; face < 6; face++)
//    {
//    if((this->faces[face].returnNeighborType(0) == TAGTYPE_REGULAR_CUBE) ||
//       (this->faces[face].returnNeighborType(0) == TAGTYPE_PASSIVE_CUBE))
//    {
//      neighbors++;
//      if(doIlightFace) 
//      {
//        this->lightFace(face,&green);
//        wifiDelay(200);
//      }
//    }
//  }
//return(neighbors);
//}



