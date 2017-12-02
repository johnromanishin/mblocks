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
  actuallyLoadMotionData();
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
  delay(2000);
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
    case 9086927: // This is the cube on the BIG Breadboard
      Serial.println("WOOO!");
      GlobalCubeID = 99;
      
      TRAVERSE_RPM_F = 6969;
      TRAVERSE_RPM_R = 69696;
      TRAVERSE_CURRENT_F = 69696;
      TRAVERSE_CURRENT_R = 6969;

      CC_RPM_F = 999999;
      CC_RPM_R = 999999;
      CC_CURRENT_F = 999999;
      CC_CURRENT_R = 999999;
      CC_BRAKETIME_F = 999;
      CC_BRAKETIME_R = 999;
 
      break;
    //********************************
    
    case 13374829:   
      GlobalCubeID = 99;
      
      TRAVERSE_RPM_F = 999;
      TRAVERSE_RPM_R = 999;
      TRAVERSE_CURRENT_F = 999;
      TRAVERSE_CURRENT_R = 999;

      CC_RPM_F = 999;
      CC_RPM_R = 999;
      CC_CURRENT_F = 999;
      CC_CURRENT_R = 999;
      CC_BRAKETIME_F = 999;
      CC_BRAKETIME_R = 999;
      break;
    //********************************
    case 959694: // Purple PEI
      GlobalCubeID = 0;
      GlobalPlaneAccel = 2000;
      TRAVERSE_RPM_F = 7000;
      TRAVERSE_RPM_R = 7000;
      TRAVERSE_CURRENT_F = 3600;
      TRAVERSE_CURRENT_R = 4000;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 2800;
      CC_CURRENT_R = 2800;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
    //********************************
    case 960348: // GREEN PEI
      GlobalCubeID = 0;
      
      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 6500;
      TRAVERSE_CURRENT_F = 2700;
      TRAVERSE_CURRENT_R = 2800;

      CC_RPM_F = 15500;
      CC_RPM_R = 15000;
      CC_CURRENT_F = 3400;
      CC_CURRENT_R = 3100;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;
    case 960043: // YELLOW PEI
      GlobalCubeID = 0;
      
      TRAVERSE_RPM_F = 7000;
      TRAVERSE_RPM_R = 7000;
      TRAVERSE_CURRENT_F = 3500;   
      TRAVERSE_CURRENT_R = 4000;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 4500;
      CC_CURRENT_R = 4500;
      CC_BRAKETIME_F = 15;
      CC_BRAKETIME_R = 15;
      break;
      
    case 8577715: // Brown PC
      GlobalCubeID = 1;
      
      TRAVERSE_RPM_F = 7000;
      TRAVERSE_RPM_R = 7000;
      TRAVERSE_CURRENT_F = 3500;
      TRAVERSE_CURRENT_R = 4000;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3600;
      CC_CURRENT_R = 4000;
      CC_BRAKETIME_F = 11;
      CC_BRAKETIME_R = 13;
      break;

    case 960242: // ORANGE PEI
      GlobalCubeID = 0;
      
      TRAVERSE_RPM_F = 7000;
      TRAVERSE_RPM_R = 7000;
      TRAVERSE_CURRENT_F = 3000;
      TRAVERSE_CURRENT_R = 2500;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3300;
      CC_CURRENT_R = 3300;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;
      
    case 15044426: // RED PEI
      GlobalCubeID = 23;
      
      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 6500;
      TRAVERSE_CURRENT_F = 3000;
      TRAVERSE_CURRENT_R = 3000;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3100;
      CC_CURRENT_R = 3300;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;

    case 8575308: // GREEN PC
      GlobalCubeID = 23;
      
      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 6500;
      TRAVERSE_CURRENT_F = 2800;
      TRAVERSE_CURRENT_R = 3000;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3000;
      CC_CURRENT_R = 3000;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;
    break;
  }
}


void loadMotionData(Motion* motion, int RPM, int Current, int brakeTime)
{
  motion->rpm = RPM;
  motion->current = Current;
  motion->brakeTime = brakeTime;
}
void actuallyLoadMotionData()
{
  loadMotionData(&traverse_F, TRAVERSE_RPM_F, TRAVERSE_CURRENT_F, 10);
  loadMotionData(&traverse_R, TRAVERSE_RPM_R, TRAVERSE_CURRENT_R, 10);
  loadMotionData(&cornerClimb_F, CC_RPM_F, CC_CURRENT_F, CC_BRAKETIME_F);
  loadMotionData(&cornerClimb_R, CC_RPM_R, CC_CURRENT_R, CC_BRAKETIME_R);
  loadMotionData(&roll_F, 7, 7, 7);
  loadMotionData(&roll_R, 7, 7, 7);
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

