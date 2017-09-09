#include <Arduino.h>
#include "MagTag.h"
#include "Defines.h"

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


//FACE      orient            forward     reverse     ...
//Face 0    PLANE_0123                    90  -- 180
//Face 1    PLANE_0123                    90  -- 180
//Face 2    PLANE_0123                    0   -- 90 
//Face 3    PLANE_0123                    90  -- 180
//Face 4    PLANE_0123
//Face 5    PLANE_0123
//Face 0    PLANE_0123                    90  -- 180
//Face 1    PLANE_0123
//Face 2    PLANE_0123
//Face 3    PLANE_0123
//Face 4    PLANE_0123
//Face 5    PLANE_0123
//Face 0    PLANE_0123                    90  -- 180
//Face 1    PLANE_0123
//Face 2    PLANE_0123
//Face 3    PLANE_0123
//Face 4    PLANE_0123
//Face 5    PLANE_0123


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
 
void analyzeTag(int angle1, int agc1, int angle2, int agc2, Tag* t)
{
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
  
  if((agc1 != 0) && DEBUG1)
    {
      Serial.println(DEBUG1);
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
      t->type = TAGTYPE_REGULAR_CUBE;
      t->id   = magDigit2;
      t->face = returnFaceNumber(magDigit1);
      if(magDigit1 % 2 ==0)
      {
        t->angle = 0;
        Serial.println("Found an actual Cube, *0* ");
      }
      else
      { 
        t->angle = 1;
        Serial.println("Found an actual Cube, *1* ");
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
          Serial.println("Found an actual Cube, *2* ");
        }
      else                 
        {
          t->angle = 3;
          Serial.println("Found an actual Cube, *3* ");
        }
    }
    
/*
 * CHECK IF TAG REPRESENTS A PASSIVE MODULE
 */
 
       if((magDigit1 == 15 || magDigit1 == 16 || magDigit1 == 17  // Means magdigit1 is a faceID
        || magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2 ) &&
          (magDigit2 == 8  || magDigit2 == 9  || magDigit2 == 10))     // Means magdifit2 stores an ID # 
          {
            t->type = TAGTYPE_PASSIVE_CUBE;
              if(magDigit1 == 30 || magDigit1 == 1  || magDigit1 == 2)
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **2**");
                  }
              else 
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **3**");
                  }
          }
       if((magDigit2 == 15 || magDigit2 == 16 || magDigit2 == 17  // Means magdigit1 is a faceID
        || magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2 ) &&
          (magDigit1 == 8  || magDigit1 == 9  || magDigit1 == 10))     // Means magdifit2 stores an ID # 
          {
            t->type = TAGTYPE_PASSIVE_CUBE;
              if(magDigit2 == 30 || magDigit2 == 1  || magDigit2 == 2)
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **0** WOOO!");
                  }
              else 
                  {
                    Serial.println("FOUND A PASSIVE CUBE ORIENTATION **1** WOOO!");
                  }
          }

/*
 * CHECK IF TAG REPRESENTS A COMMAND TAG
 */
  t->command = TAGCOMMAND_NONE;
  if((magDigit1 == magDigit2) &&  // Means magdigit1 is a faceID
     (magDigit1 != 17 && magDigit2 != 17) &&
     (magDigit1 != 30 && magDigit2 != 30)
     )     // Means magdifit2 stores an ID # 
  {
    if(magDigit1 == 25) // Sleep Command
      t->command = TAGCOMMAND_SLEEP;
    if(magDigit1 == 5) // Sleep Command
      t->command = TAGCOMMAND_PURPLE;
  }
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
