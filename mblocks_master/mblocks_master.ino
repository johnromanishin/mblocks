/**
    This includes the main code for the ESP8266 board, aka the "master"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/

#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal - used to communicate with all of the sensors on the Mblocks
#include <painlessMesh.h>
//#include <ArduinoHardware.h>      // Unsure of what this does, but it seemed like a good idea to keep it in here...
//#include <arduino.h>
#include "initialization.h"       // Includes .h files for each of the "tabs" in arduino
#include "Cube.h"     // Includes .h files for each of the "tabs" in arduino
#include "Face.h"     // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"                // Includes .h files for each of the "tabs" in arduino
#include "communication.h"        // Includes wifi 
#include "behavior.h"

//WiFi::BSSIDstr(i);

String behavior = "soloSeekLight";
Cube c; // Initialize the Cube Object c // globally so that things don't crash

void setup() // Actually the main loop...
{
  long timer_counter = 0;
  initializeCube(); // Runs this code once to setup input/outputs, communication networks... 
                    // (Wifi, i2c, serial) and instantiates classes and calibration values
  Serial.println(ESP.getChipId());
  Serial.println("Starting Main Loop");
  
  c.updateCoreMagnetSensor();
  int initialMagnetReadingOffset = c.coreMagnetAngleBuffer.access(0);

  Serial.print("Face Version: ");Serial.println(faceVersion);
  ///////////////////////ACTUAL LOOP////////////////////
  while(millis() < c.shutDownTime && !(c.numberOfNeighbors(0,0)))
    {
       
      c.updateSensors();
      int brightestFace = c.returnXthBrightestFace(0);
      if(c.returnXthBrightestFace(0) == c.returnTopFace()) // now brightest Face now excludes the top face
      {
        brightestFace = c.returnXthBrightestFace(1);
      }
      
      c.lightFace(brightestFace,0,1,1);
      delay(500);
           if(brightestFace == c.returnForwardFace()) 
           {Serial.println("bldcspeed f 6000");c.blockingBlink(0,1,0);delay(3000);Serial.println("bldcstop b");}
      else if(brightestFace == c.returnReverseFace()) 
      {Serial.println("bldcspeed r 6000");c.blockingBlink(1,0,0);delay(3000);Serial.println("bldcstop b");}
      else if(c.returnForwardFace() == c.returnXthBrightestFace(2)){Serial.println("bldcspeed f 6000");c.blockingBlink(0,1,0);delay(3000);Serial.println("bldcstop b");}              
      else if(c.returnReverseFace() == c.returnXthBrightestFace(2)){Serial.println("bldcspeed r 6000");c.blockingBlink(1,0,0);delay(3000);Serial.println("bldcstop b");}
      else  {Serial.println("bldcaccel f 6000 2000"); delay(2000); Serial.println("bldcstop b");delay(5000);}
    }
  c.blockingBlink(0,0,1,30,200);
  c.shutDown();    
  ///////////////////////ACTUAL LOOP////////////////////
  while(1)
  {
         if (behavior == "soloSeekLight") {soloSeekLight(&c);}
    else if (behavior == "duoSeekLight")  {duoSeekLight();}
    else if (behavior == "followArrows")  {followArrows();}
    
    if(millis() > timer_counter)
      {
        //face1.updateAmbient();
        //Serial.println(face1.returnAmbientValue(0));
        String msg_new = "";
        msg_new = ("HEY BRO");// + String(msg_id); //+ " Sensor Magnitude: " +String(read_5048_agc(address4)) + "   Angle: " + String(read_5048_angle(address4)/45.5) + "  ");
        timer_counter += 5000;
        mesh.sendBroadcast(msg_new);
      }
    mesh.update();
  }
}

// This is here only becuase arduino won't compile without it, but it is never used, the real loop is "while(1)" in the void setup() Function
void loop() 
{  
}
/////////// Global Variables
int faceVersion = 1;
int cubeID = 0;
int planeChangeTime = 60;
int planeChangeRPM = 5000;
int traverseBrakeCurrent_F = 2800;
int traverseBrakeCurrent_R = 2800;
int cornerClimbBrakeCurrent_F = 3000;
int cornerClimbBrakeCurrent_R = 3000;
int plane0321Magnet = 0;
int plane0425Magnet = 120;
int plane1435Magnet = 240;
