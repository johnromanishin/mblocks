#ifndef SENSATION_H
#define SENSATION_H

#include <Arduino.h>
//#include <ArduinoHardware.h>

int readAmbient(int address);
void activateLightSensor(int address);
void readGyroAndAccel(int i2cAddress);
int magnetSensorAngle(int i2cAddress);
int magnetSensorFieldStrength(int i2cAddress);
int magnetSensorRead(int i2cAddress, byte dataRegisterAddress);
void wakeMPU(int i2cAddress);

#endif
