#include "Motion.h"
#include <ArduinoHardware.h>


bool roll(bool forwardOrReverse, int flywheelRPM, int wait)
{

}

bool lattice_transition()
{

  
}

bool setCorePlane(int targetCorePlane) 
  {
    if (targetCorePlane == currentCorePlane()) {
    return true;
  }

  // ADD CODE HERE
  // physically adjust core plane to match target

  if (targetCorePlane == currentCorePlane()) {
    return true;
  }
  return false;
}

int currentCorePlane() {
  // ADD CODE HERE
  // use accelerometers and maybe magnet sensor to identify
}
