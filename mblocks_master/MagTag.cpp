#include <Arduino.h>
#include "MagTag.h"
//  int face1A = 29; // Magnet centerpoint for face 1 magnet position a
//  int face1B = 28;
//  int face2A = 27;
//  int face2B = 26;
//  int face3A = 25;
//  int face3B = 24;
//  int face4A = 23;
//  int face4B = 22; 
//  int face5A = 21;
//  int face5B = 20;
//  int face6A = 19;
//  int face6B = 18; 

  
void analyzeTag(int angle1, int agc1, int angle2, int agc2)
{
//  Serial.println("analyzing a tag!");
/*
 * Analyze Tag takes as arguements two magnetic sensor angles, and two 
 * corresponding AGC (Valid Magnetic field strength) values.
 * 
 * The function then generates an integer of the form - 
 * 
 * X
 * 
 * 
 */
  
  int magDigit1 = 0;
  int magDigit2 = 0;
        if (agc1 == 0 || agc1 >= 255)       {magDigit1 = 0;}
   else if (angle1 < 6 || angle1 > 354)     {magDigit1 = 1;}
   else                                     {magDigit1 = int(angle1 + 18)/12;}

        if (agc2 == 0 || agc2 >= 255)       {magDigit2 = 0;}
   else if(angle2 < 6 || angle2 > 354)      {magDigit2 = 1;}  
   else                                     {magDigit2 = int(angle2 + 18)/12;}
   
  if(agc1 != 0)
    {
      Serial.print("angle1: "); Serial.println(angle1);
      Serial.print("agc1:   "); Serial.println(agc1);
      Serial.print("angle2: "); Serial.println(angle2);
      Serial.print("agc2:   "); Serial.println(agc2);
      Serial.print("magDigit1: "); Serial.println(magDigit1);
      Serial.print("magDigit2: "); Serial.println(magDigit2);
    }

/*
 * CHECK IF TAG REPRESENTS A MODULE
 */
  if((magDigit1 >= 17 && magDigit1 <= 29) &&  // Means magdigit1 is a faceID
     (magDigit2 >= 1 && magDigit2 <= 17))     // Means magdifit2 stores an ID # 
    {
      if(magDigit1 % 2 ==0){Serial.println("Found an actual Cube, **1** ");delay(1000);}
      else                 {Serial.println("Found an actual Cube, **2** ");delay(1000);}
    }
    
  if((magDigit2 >= 17 && magDigit2 <= 29) &&  // Means magdigit1 is a faceID
     (magDigit1 >= 1 && magDigit1 <= 17))     // Means magdifit2 stores an ID # 
    {
      if(magDigit2 % 2 ==0){Serial.println("Found an actual Cube, **3** ");delay(1000);}
      else                 {Serial.println("Found an actual Cube, **4** ");delay(1000);}
    }
    
/*
 * CHECK IF TAG REPRESENTS A PASSIVE MODULE
 */
 
       if((magDigit1 == 15 || magDigit1 == 16 || magDigit1 == 17  // Means magdigit1 is a faceID
        || magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2 ) &&
          (magDigit2 == 8  || magDigit2 == 9  || magDigit2 == 10))     // Means magdifit2 stores an ID # 
          {
              if(magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2)
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **3** WOOO!");delay(1000);
                  }
              else 
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **4** WOOO!");delay(1000);
                  }
          }
       if((magDigit2 == 15 || magDigit2 == 16 || magDigit2 == 17  // Means magdigit1 is a faceID
        || magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2 ) &&
          (magDigit1 == 8  || magDigit1 == 9  || magDigit1 == 10))     // Means magdifit2 stores an ID # 
          {
              if(magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2)
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **1** WOOO!");delay(1000);
                  }
              else 
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **2** WOOO!");delay(1000);
                  }
          }

/*
 * CHECK IF TAG REPRESENTS A COMMAND TAG
 */
  if((magDigit1 == magDigit2) &&  // Means magdigit1 is a faceID
     (magDigit1 != 17 && magDigit2 != 17) &&
     (magDigit1 != 30 && magDigit2 != 30)
     )     // Means magdifit2 stores an ID # 
    {
      if(magDigit1 == 25) // Sleep Command
        {
          Serial.println("Going to sleep!"); 
          sendMessage("sleep");
          long startTime = millis(); // start time
          long waitToSleep = 5000;
          while((millis()-startTime) < waitToSleep)
             {
              mesh.update();
             }
          sendMessage("sleep");
          delay(500);
          shutDownMasterBoard();
        }
    }
    Serial.println("Exiting out of the tag!");
}
