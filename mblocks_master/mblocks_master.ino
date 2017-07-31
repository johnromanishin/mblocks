/**
    This includes the main code for the ESP8266 board, aka the "master"
    Copyright John Romanishin, MIT CSAIL
    johnrom@mit.edu
*/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>
#include <ArduinoHardware.h>

void setup() {
  initializeCube();
}

void loop() {
  stepTowardLight();
}
