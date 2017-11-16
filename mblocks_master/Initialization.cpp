#include "Initialization.h"
#include "Communication.h"
#include "Defines.h"
#include "Cube.h"
#include <Wire.h>  
#include <Arduino.h>
#include <painlessMesh.h>  // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoJson.h>

void initializeCube()
{
  initializeHardware();
  lookUpCalibrationValues();
  initializeWifiMesh();
  wifiDelay(200);
  int count = 0;
  while(!checkFaceVersion) // check to see if face boards are connected
  {
    whatToDoIfIamNotConnectedAtBeginning(); // assuming we are not connected...
                                            // we reset things.
    count++;
    if(count > 3)
    {
        for(int i = 0; i < 10; i++) 
        {Serial.println("sleep"); delay(500);}
    }
  }
}
//// Smaller Functions ////

void shutDownMasterBoard()
{
  while (true) {
    Serial.println("espoff");  // attempt to shut down, with a delay between attempts
    delay(300);
  }
}

bool checkFaceVersion()
/*
 * This functions checks to see which version of the cube's Face hardware is running
 * It will first check for version 1: Containing i2c address 0x21,
 * Then it will check for version 0: Containing i2c address 
 */
{
  int error;
  for(int i = 0; i < 6; i++)
  {
    Wire.beginTransmission(0x20); // Check for i2c address 0x20
    error = Wire.endTransmission();  
    if(error == 0){faceVersion = 1; return(true);}
    delay(100);      
    
    Wire.beginTransmission(0x21); // Check for i2c address 0x21
    error = Wire.endTransmission();  
    if(error == 0){faceVersion = 1; return(true);}
    delay(100);

    Wire.beginTransmission(0x22); // Check for i2c address 0x22
    error = Wire.endTransmission();  
    if(error == 0){faceVersion = 1; return(true);}
    delay(100);

    Wire.beginTransmission(0x23); // Check for i2c address 0x22
    error = Wire.endTransmission();  
    if(error == 0){faceVersion = 1; return(true);}
    delay(100);
      
    Wire.beginTransmission(0x04);
    error = Wire.endTransmission();  
    if(error == 0){faceVersion = 0; return(true);}
    delay(500);
  }
  return(false);
}

void whatToDoIfIamNotConnectedAtBeginning()
{
  mesh.update();
  resetI2cBus();
}

void initializeHardware()
{
  Serial.begin(115200);       // open serial connection to the Slave Board
  Serial.println("stillalive");
  delay(800);
  Serial.print("ESP Chip ID: ");
  Serial.println(ESP.getChipId());
  pinMode(Switch, OUTPUT);    // Initialize the pin to controll the power switching circuitry
  digitalWrite(Switch, LOW);  // Set the power switch to be OFF - 
                              //this is so that we don't disrupt charging if we are on a charging pad
  pinMode(LED, OUTPUT);       // Initialize the pin to control the blinky LED
  Wire.begin(SDA, SCL);       // Begin Two Wire Bus (i2c) to contact all of the sensors
  
  int timesToCheck = 2;       // We need to verify that the cube is not 
                              // being charged at startup, we do this by asking
  for(int i = 0; i < timesToCheck; i++)
  {
    if (inputVoltage() > 3400) {
      shutDownMasterBoard(); // This turns off ESP if we are on a charging pad - checks three times
      delay(500);
    }
  }
  disableAutoReset();
  digitalWrite(Switch, HIGH); // turns on power to the face Boards
}

/*
 *this tells the slave board not to accidently turn off its power, 
 *it prints it three times incase it is lost
 */ 
void disableAutoReset() 
{
  Serial.println("espprogram");
  delay(50);
  Serial.println("espprogram");
  delay(100);
  Serial.println("espprogram");
}

/*
 * Obtains the input voltage from the slave board to 
 * detect when the block is charging, and to put the master board to sleep
 */
int inputVoltage()
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
  while (Serial.available() != 0 && (millis() - startTime) < timeout) 
  // while there are things in the serial buffer to read, and timeout has not passed
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

int get_battery_voltage()
  {
    int vbat[4];
    long begin_function = millis();
    while(Serial.available()){Serial.read();} // empty serial buffer just in case...
    delay(3);
    Serial.println("vbat");
    delay(23);
    char prev_char = ' ';
    String temp_string = "";
    int battery_counter = 1;
    while (Serial.available() > 0 && (millis()-begin_function) < 60) 
    // while there are things in the serial buffer...
    {
        char c = Serial.read();
        if(c == ' ' && prev_char == ':')
          {
              for(int i = 0; i < 4; i++)
                  {
                  char a = Serial.read();
                  delayMicroseconds(250);
                  if(isDigit(a)){temp_string += a;}
                  } 
              if(battery_counter == 1){vbat[1] = temp_string.toInt();}
              else if(battery_counter == 2){vbat[2] = temp_string.toInt();}
              else if(battery_counter == 3){vbat[3] = temp_string.toInt();}
              else if(battery_counter == 4){vbat[4] = temp_string.toInt();}
              battery_counter++;
              temp_string = ""; 
          }
        delayMicroseconds(200);
        prev_char = c;
    }
  vbat[0] = (vbat[1]+vbat[2]+vbat[3]+vbat[4])/4;
  return (vbat[0]);
}

/*
 * 
 * Updates global variables with values according to the esp.getchipid
 */
void lookUpCalibrationValues()
{
  switch (ESP.getChipId()) 
  {
    //********************************
    case 9086927:
      Serial.println("WOOO!");
      cubeID = 0;
      
      TRAVERSE_RPM_F = 2800;
      TRAVERSE_RPM_R = 2800;
      TRAVERSE_CURRENT_F = 2800;
      TRAVERSE_CURRENT_R = 2800;

      CC_RPM_F = 2800;
      CC_RPM_R = 2800;
      CC_CURRENT_F = 2800;
      CC_CURRENT_R = 2800;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
 
      break;
    //********************************
    
    case 2:    
      break;
      
    break;
  }
}

void resetI2cBus()
{

  digitalWrite(Switch, LOW); 
  for(int i = 0 ; i<10 ; i++)
  {
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
    delay(50);
  }
  digitalWrite(Switch, HIGH); 
}

