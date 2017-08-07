#include <ArduinoHardware.h>
#include "initialization.h"
#include "communication.h"
#include <Wire.h>  


// Hardware Pin Definitions
#define Switch 12 // Digital Output | Switch which controlls power to the face boards,  High = power given to faceboards, Low = ability to charge
#define LED 13    // Digital Output | Directly Controlls a small white LED on the "Master" circuit board
#define SDA 2     // Managed by the wire.begin in initializeCube()
#define SCL 14    // Managed by the wire.begin in initializeCube() 

void initializeCube()
{
  initializeHardware();
  lookUpCalibrationValues();
  initializeWifiMesh();
  initializeClasses(1);
}

void initializeClasses(int faceVersion)
{
  
}

//// Smaller Functions ////

void lookUpCalibrationValues()
{
  
}

void shutDownMasterBoard()
{
  while (true) {
    Serial.println("espoff");  // attempt to shut down, with a delay between attempts
    delay(300);
  }
}

void initializeHardware()
{
  delay(500);
  pinMode(Switch, OUTPUT); // Initialize the pin to controll the power switching circuitry
  digitalWrite(Switch, LOW); // Set the power switch to be OFF - this is so that we don't disrupt charging if we are on a charging pad
  pinMode(LED, OUTPUT); // Initialize the pin to control the blinky LED
  Serial.begin(115200); // open serial connection to the Slave Board
  Wire.begin(SDA, SCL); // Begin Two Wire Bus (i2c) to contact all of the sensors
  
  int timesToCheck = 2; // We need to verify that the cube is not being charged at startup, we do this by asking
  for(int i = 0; i++; i < timesToCheck)
  {
    if (inputVoltage() > 3400) {
      shutDownMasterBoard(); // This turns off ESP if we are on a charging pad - checks three times
      delay(300);
    }
  }
  disableAutoReset();
  digitalWrite(Switch, HIGH); // turns on power to the 
}

void disableAutoReset() // this tells the slave board not to accidently turn off its power, it prints it three times incase it is lost
{
  Serial.println("espprogram");
  delay(50);
  Serial.println("espprogram");
  delay(100);
  Serial.println("espprogram");
}
int inputVoltage()
// Obtains the input voltage from the slave board to detect when the block is charging, and to put the master board to sleep
{
  int timeout = 60; // ms - how long the function will attempt to parse text for.
  long startTime = millis();
  char previousCharacter;
  String voltageAsString  = "";
  while (Serial.available()) {
    Serial.read(); // empty serial buffer, just in case the buffer is holding other stuff
  }
  Serial.println("vin");
  delay(25);                                // We need to wait for the slave board to process the command
  while (Serial.available() != 0 && (millis() - startTime) < timeout) // while there are things in the serial buffer to read, and timeout has not passed
  {
    char currentCharacter = Serial.read();
    if (previousCharacter == ':' && currentCharacter == ' ')
    {
      for (int i = 0; i < 4; i++)
      {
        char currentCharacter = Serial.read();
        delayMicroseconds(250);
        if (isDigit(currentCharacter)) {
          voltageAsString += currentCharacter;
        }
      }
      return voltageAsString.toInt();


    }
    delayMicroseconds(250);
    previousCharacter = currentCharacter;
  }
  return 0;
}
