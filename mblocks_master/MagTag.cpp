#include <Arduino.h>
#include "MagTag.h"
#include "Defines.h"

//typedef struct Tag
//{
//  TagType type;   //
//  int angle ; // Either -1, 0, 1, 2 or 3 - corresponding to 90 deg angle
//  int id; // ID or message code attached to tag
//  int face; // face number (0,1,2,3,4,5) associated with a cube
//  int strength; // Validitity of the tag, basically just agc1+agc2
//  TagCommand command; // Text of command or behavior to go to... if it exists
//} Tag;

/*
 * Analyze Tag takes as arguements two magnetic sensor angles, and two 
 * corresponding AGC (Valid Magnetic field strength) values.
 */
 
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
  
  if(((agc1+agc2) > strengthThreshold) || (t->strength == 0)) // this means there isn't a valid tag
    {
      t->type = TAGTYPE_NOTHING;   
      t->angle = -1;  
      t->id = -1;  
      t->face = -1; 
      t->command = TAGCOMMAND_NONE; 
      return; // DOES NOT EVAUATE ANYTHING ELSE EXITS PROGRAM here
    }
  else // this makes sure that all of the datafields are not set randomly.
    {
      t->type = TAGTYPE_INVALID;   //
      t->angle = -1; // 
      t->id = -1; // 
      t->face = -1; //
      t->command = TAGCOMMAND_NONE;// 
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
          (magDigit2 == 8  || magDigit2 == 9  || magDigit2 == 10))     // Means magdifit2 stores an ID # 
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
  if((magDigit1 == magDigit2) &&  // Means magdigit1 is a faceID
     (magDigit1 != 17 && magDigit2 != 17) &&
     (magDigit1 != 30 && magDigit2 != 30)
    )
  {
    t->type = TAGTYPE_COMMAND;
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
