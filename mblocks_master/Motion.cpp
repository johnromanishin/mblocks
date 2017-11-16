#include "Motion.h"
#include "SerialDecoder.h"

// GLOBAL VARIABLES
//int TRAVERSE_RPM_F = 2800;
//int TRAVERSE_RPM_R = 2800;
//int TRAVERSE_CURRENT_F = 2800;
//int TRAVERSE_CURRENT_R = 2800;
//
//int CC_RPM_F = 2800;
//int CC_RPM_R = 2800;
//int CC_CURRENT_F = 2800;
//int CC_CURRENT_R = 2800;

//                      moveName      , brake   , rpm             , timout, current             , brakeTime     , difficult , for_rev
Motion traverse_F     = {"traverse"   , true    , TRAVERSE_RPM_F  , 6000  , TRAVERSE_CURRENT_F  , 12            , 9         ,"f"};
Motion traverse_R     = {"traverse"   , true    , TRAVERSE_RPM_R  , 6000  , 3000                , 12            , 9         ,"r"};

Motion roll_F         = {"roll"       , false   , 6500            ,    0  ,    0                , 0             , 1         ,"f"};
Motion roll_R         = {"roll"       , false   , 6500            ,    0  ,    0                , 0             , 1         ,"r"};

Motion cornerClimb_F  = {"cornerClimb", true    , CC_RPM_F        , 5000  , 4000                , 20            , 250       ,"f"};
Motion cornerClimb_R  = {"cornerClimb", true    , CC_RPM_R        , 5000  , 4000                , 20            , 250       ,"r"};

/**
 * This command takes predefined motion Structs and attempts to mvoe 
 * usage would work like: Move(traverse, 1);
 */
bool MoveIA(Motion* motion, SerialDecoderBuffer* buf)
{
  int attempts = 0;
  long beginTime = millis();
  while(numberOfAttempts) // we subtract 1 from "numberOfAttempts" each time around
  {
    String iaString = "ia" + " " + motion->dir + 
    //Step 1:  Set Motor to a specific speed
    String rpmString = String("bldcspeed") + " " + Direction + " " + String(motion->rpm);
    String brakeString = String("brake") + String(" ") + Direction +
      " " + String(motion->current) + " " + String(motion->brakeTime) + "e 10";
    Serial.println(rpmString);
    
    while((millis() - beginTime) < motion->timeout)
      {
        delay(1);
        SerialResponse resp = pushNewChar(buf);
        //Serial.println(buf->size);
        // update Wifi
        // maybe check gyro values
        // if(StringParser() == verifiedRPM)]
        if(resp == RESPONSE_BLDC_STABLE)
        { 
          //Serial.println("wooo! we parsed the sh*t out of that serial bitch");
          break;
        }
      }
    // Step 2 - Send the Brake Command
    if(motion->brake){Serial.println(brakeString);}
    delay(10);
    Serial.println("bldcstop b");
    delay(10);
    Serial.println("bldcstop b");
    // Step 3 - Make sure the move worked.
    numberOfAttempts--;
    return(true);
  }
}

bool Move(Motion* motion, SerialDecoderBuffer* buf)
{
  int attempts = 0;
  long beginTime = millis();
  while(numberOfAttempts) // we subtract 1 from "numberOfAttempts" each time around
  {
    //String iaString = 
    //Step 1:  Set Motor to a specific speed
    String rpmString = String("bldcspeed") + " " + Direction + " " + String(motion->rpm);
    String brakeString = String("brake") + String(" ") + Direction +
      " " + String(motion->current) + " " + String(motion->brakeTime) + "e 10";
    Serial.println(rpmString);
    
    while((millis() - beginTime) < motion->timeout)
      {
        delay(1);
        SerialResponse resp = pushNewChar(buf);
        //Serial.println(buf->size);
        // update Wifi
        // maybe check gyro values
        // if(StringParser() == verifiedRPM)]
        if(resp == RESPONSE_BLDC_STABLE)
        { 
          //Serial.println("wooo! we parsed the sh*t out of that serial bitch");
          break;
        }
      }
    // Step 2 - Send the Brake Command
    if(motion->brake){Serial.println(brakeString);}
    delay(10);
    Serial.println("bldcstop b");
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
