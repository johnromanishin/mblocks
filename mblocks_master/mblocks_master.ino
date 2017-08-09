/**
    This includes the main code for the ESP8266 board, aka the "master"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/

#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal - used to communicate with all of the sensors on the Mblocks
#include <painlessMesh.h>
#include <ArduinoHardware.h>      // Unsure of what this does, but it seemed like a good idea to keep it in here...
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
  while(millis() < c.shutDownTime)
  {
   //c.updateBothIMUs();
   //c.updateCoreMagnetSensor();
   
//   String newmsg = "Angle: " + String(c.coreMagnetAngleBuffer.access(0) - initialMagnetReadingOffset)
//   + " core.ax: " + String(c.axCoreBuffer.access(0))
//   + " core.ay: " + String(c.ayCoreBuffer.access(0))
//   + " core.az: " + String(c.azCoreBuffer.access(0))
//   + " Frame.ax: " + String(c.axFrameBuffer.access(0))
//   + " Frame.ay: " + String(c.ayFrameBuffer.access(0))
//   + " Frame.az: " + String(c.azFrameBuffer.access(0))
//   + " CoreMagAGC: " + String(c.coreMagnetStrengthBuffer.access(0));
   
   //Serial.print("Angle: ");Serial.print(c.coreMagnetAngleBuffer.access(0));Serial.print(" Magnitude: ");Serial.println(c.coreMagnetStrengthBuffer.access(0));
   //String newmsg = "Angle: ";
   //mesh.sendBroadcast(newmsg);
   for(int i = 0; i < 6; i++)
   {
    delay(10);
    if(c.faces[i].updateFace())
    {
   //Serial.print("Ambient Value on face "); Serial.print(i); Serial.print(": "); Serial.println(c.faces[i].returnAmbientValue(0));
   if((c.faces[i].returnMagnetStrength_B(0) < 255) && (c.faces[i].returnMagnetStrength_A(0) < 255) && (c.faces[i].returnMagnetStrength_A(0) != 0))
    {Serial.print("HEY!!");Serial.println(c.lightFace(i));}
   else if((c.faces[i].returnMagnetStrength_B(1) < 255) && (c.faces[i].returnMagnetStrength_B(0) > 254)){c.lightFace(i,0,0,0);}
    }
//    c.lightFace(i);
//    c.faces[i].enableSensors();
//    c.faces[i].turnOnFaceLEDs();
//    c.faces[i].updateAmbient();
//    Serial.print("Ambient Value on face "); Serial.print(i); Serial.print(": "); Serial.println(c.faces[i].returnAmbientValue(0));
//    delay(300);
//    c.faces[i].turnOffFaceLEDs();
//    c.faces[i].disableSensors();
   }
   
   delay(1000);
   
//   Serial.println(c.returnTopFace());
//   Serial.print("IMU_ax: ");Serial.println(c.axFrameBuffer.access(0));
//   Serial.print("IMU_ay: ");Serial.println(c.ayFrameBuffer.access(0));
//   Serial.print("IMU_az: ");Serial.println(c.azFrameBuffer.access(0));
   //Serial.print("IMU_gx: ");Serial.println(c.gxFrameBuffer.access(0));
   //Serial.print("IMU_gy: ");Serial.println(c.gyFrameBuffer.access(0));
   //Serial.print("IMU_gz: ");Serial.println(c.gzFrameBuffer.access(0));
   
   // c.lightFace(c.returnTopFace());
   // delay(100);
  }
  c.shutDown();    
   
  while(1)
  {
         if (behavior == "soloSeekLight") {soloSeekLight();}
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
