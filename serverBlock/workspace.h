//    StaticJsonBuffer<200> jsonBuffer; //Space Allocated to store json instance
//    JsonObject& root = jsonBuffer.createObject(); // & is "c++ reference"
//    //^class type||^ Root         ^class method                   
//    root["type"] = "cmd";
//    root["targetID"] = 55;
//    root["senderID"] = getCubeIDFromEsp(ESP.getChipId());
//    root["cmd"] = "update";
//    //^ "key"   |  ^ "Value"
//    String str = "yo"; // generate empty string
//    root.printTo(str); // print to JSON readable string...

    
//Behavior cmdToBehaviors(String cmd, Behavior defaultBehavior)
///*  
// *   This function takes in a string, and if it matches to a behavior, it 
// *   returns that behavior. If the string doesn't match, it returns
// */
//{
//  Behavior behaviorToReturn = defaultBehavior;
//       if(cmd == "solo_light_track")    {behaviorToReturn = SOLO_LIGHT_TRACK;}
//  else if(cmd == "duo_light_track")     {behaviorToReturn = DUO_LIGHT_TRACK;}
//  else if(cmd == "follow_arrows")       {behaviorToReturn = FOLLOW_ARROWS;}
//  else if(cmd == "chilling")            {behaviorToReturn = CHILLING;}
//  else if(cmd == "climb")               {behaviorToReturn = CLIMB;}
//  else if(cmd == "attractive")          {behaviorToReturn = ATTRACTIVE;}
//  else if(cmd == "shut_down")           {behaviorToReturn = SHUT_DOWN;}
//  else if(cmd == "sleep")               {behaviorToReturn = SLEEP;}
//  else if(cmd == "multi_light_track")   {behaviorToReturn = MULTI_LIGHT_TRACK;}
//  else if(cmd == "pre_solo_light")      {behaviorToReturn = PRE_SOLO_LIGHT;}
//  
//return(behaviorToReturn);
//}
//
//String behaviorsToCmd(Behavior inputBehavior)
///*  
// *   This function takes in a string, and if it matches to a behavior, it 
// *   returns that behavior. If the string doesn't match, it returns
// */
//{
//  String stringToReturn = " ";
//       if(inputBehavior == SOLO_LIGHT_TRACK)      {stringToReturn = "solo_light_track";} 
//  else if(inputBehavior == DUO_LIGHT_TRACK)       {stringToReturn = "duo_light_track";}
//  else if(inputBehavior == FOLLOW_ARROWS)         {stringToReturn = "follow_arrows";}
//  else if(inputBehavior == CHILLING)              {stringToReturn = "chilling";}
//  else if(inputBehavior == CLIMB)                 {stringToReturn = "climb";}
//  else if(inputBehavior == ATTRACTIVE)            {stringToReturn = "attractive";}
//  else if(inputBehavior == SHUT_DOWN)             {stringToReturn = "shut_down";}
//  else if(inputBehavior == SLEEP)                 {stringToReturn = "sleep";}
//  else if(inputBehavior == MULTI_LIGHT_TRACK)     {stringToReturn = "multi_light_track";}
//  else if(inputBehavior == PRE_SOLO_LIGHT)        {stringToReturn = "pre_solo_light";}
//return(stringToReturn);
//}
