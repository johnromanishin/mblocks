#include "Motion.h"
#include "SerialDecoder.h"

//                      moveName    , brake   , rpm   , timout, current, brakeTime, difficult
Motion traverse     = {"traverse"   , true    , 6500  , 6000  , 3000   , 12        , 9         };
Motion roll         = {"roll"       , false   , 6500  ,    0  ,    0   , 0        , 1         };
Motion cornerClimb  = {"cornerClimb", true    , 14000 , 5000  , 4000   , 20       , 250       };

/**
 * This command takes predefined motion Structs and attempts to mvoe 
 * usage would work like: Move(traverse, 1);
 */
 
bool Move(Motion* motion, String Direction, int numberOfAttempts, SerialDecoderBuffer* buf)
{
  long beginTime = millis();
  while(numberOfAttempts)
  {
    //Step 1:  Set Motor to a specific speed HEY
    String rpmString = String("bldcspeed") + " " + Direction + " " + String(motion->rpm);
    String brakeString = String("brake") + String(" ") + Direction +
      " " + String(motion->current) + " " + String(motion->brakeTime);
    Serial.println(rpmString);
    
    while((millis() - beginTime) < motion->timeout)
      {
        delay(1);
        SerialResponse resp = pushNewChar(buf);
        Serial.println(buf->size);
        // update Wifi
        // maybe check gyro values
        // if(StringParser() == verifiedRPM)]
        if(resp == RESPONSE_BLDC_STABLE)
        { 
          Serial.println("wooo! we parsed the sh*t out of that serial bitch");
          break;
        }
      }
    // Step 2 - Send the Brake Command
    if(motion->brake){Serial.println(brakeString);}
    delay(10);
    Serial.println("bldcstop b");
    // Step 3 - Make sure the move worked.
    numberOfAttempts--;
    return(true);
  }
}

bool lattice_transition()
{

  
}
