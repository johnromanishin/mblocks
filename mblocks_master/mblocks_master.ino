/**
    This includes the main code for the ESP8266 board, aka the "master"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/

#include <Wire.h>                 // Arduino's implementation of the i2c wireless protocal - used to communicate with all of the sensors on the Mblocks
#include <painlessMesh.h>
#include <ArduinoHardware.h>      // Unsure of what this does, but it seemed like a good idea to keep it in here...
#include "initialization.h"       // Includes .h files for each of the "tabs" in arduino
#include "classDefinitions.h"     // Includes .h files for each of the "tabs" in arduino
#include "CBuff.h"                // Includes .h files for each of the "tabs" in arduino
#include "communication.h"        // Includes wifi 
#include "behavior.h"

String behavior = "soloSeekLight";


Cube c;
void setup() // Actually the main loop...
{
  long timer_counter = 0;
  initializeCube(); // Runs this code once to setup input/outputs, communication networks... 
                     // (Wifi, i2c, serial) and instantiates classes/ calibration values
  while(1)
  {
    for(int i = 0; i< 6; i++)
    {
      //Serial.print(c.faces[i].IOExpanderAddress);  Serial.print("  ");
      c.faces[i].enableSensors();
      c.faces[i].turnOnLedA();
      c.faces[i].updateAmbient();
      Serial.print(c.faces[i].returnAmbientValue(0)); Serial.print("  ");
      c.faces[i].turnOffLedA();
      c.faces[i].disableSensors();
      delay(1);
    }
    Serial.println("hello");
    delay(500);
  }
                    
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
