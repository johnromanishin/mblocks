#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include <ArduinoHardware.h>

// We wish to define four behaviors, in order of coding priority:
//  1. Step toward a light source, solo
//  2. Step toward a light source, in tandem with an adjacent block
//  3. Step in the direction of an arrow, provided on an adjacent face
//  4. Step in a direction provided via external communication (WiFi)

void followArrows();

void soloSeekLight();

void duoSeekLight();

#endif
