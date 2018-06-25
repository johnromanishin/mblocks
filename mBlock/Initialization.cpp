#include "Initialization.h"
#include "Communication.h"
#include "Defines.h"
#include "Cube.h"
#include <Wire.h>
#include <Arduino.h>
#include <painlessMesh.h>  // Wireless library which forms mesh network https://github.com/gmag11/painlessMesh
#include <ArduinoJson.h>

/*
 * *************************************************************************************************************************************************************************************
 * ** Cube Name   **   Version    * Plane Ch      * BAD Red     * Bad White      * i2c Busted      * Mech. Rubbing *  Drains Bat  * Acceler Issue*  6000ma Kills * batteries Replaced
 * *************************************************************************************************************************************************************************************
 * ** PEI BLACK   **              *               *             *                *                 *               *              *              *               *    2017-12-21      *
 * ** PEI GREEN   **              *               *             *                *                 *               *              *              *               *    2017-12-17      *
 * ** PEI ORANGE  **              *               *    XX       *  X             *                 *               *              *              *               *                    *
 * ** PEI PURPLE  **              *               *             *                *                 *               *    X         *              *               *                    *
 * ** PEI RED     **    WIFI      *               *             *                *                 *               *              *              *               *                    *
 * ** PEI YELLOW  **              *               *    X        *                *                 *               *              *     X        *               *                    *
 * ** PEI BROWN   **              *               *    X        *                *                 *               *              *              *               *    2017-12-21      *
 * ** PEI BLUE    **              *               *    X        *                *                 *               *              *              *               *                    *
 *
 * ** PC BLACK    **              *               *             *                *                 *               *              *              *               *                    *
 * ** PC BLUE     **              *               *    X        *   X            *                 *               *              *              *               *                    *
 * ** PC PURPLE   **              *               *             *                *                 *   Brake Arms  *              *              *               *                    *
 * ** PC GREEN    **              *               *    XX       *                *                 *               *              *              *               *                    *
 * ** PC BROWN    **              *               *             *                *                 *               *              *              *               *                    * X
 * ** PC YELLOW   **              *               *    X        *                *                 *               *              *              *               *                    *
 * ** PC ORANGE   **              *               *             *                *                 *               *              *              *               *                    *
 *
 * ** ORANGEPC RED**              *               *    XX       *                *                 *               *              *              *               *                    *
 */

void initializeCube()
{
  initializeHardware();
  lookUpCalibrationValues(initializeWifiMesh());
  setMotionDataGlobalVariables();
  wifiDelay(200);
  int count = 0;
  while(!checkIfConnected()) // check to see if face boards are connected
  {
    whatToDoIfIamNotConnectedAtBeginning(); // assuming we are not connected...
                                            // we reset things.
    count++;
    if(count > 3)
    {
        for(int i = 0; i < 10; i++)
        {Serial.println("sleep"); delay(500);} // Go to sleep...
    }
  }
}

bool checkIfConnected()
/*
 * This function checks to see if the ESP board is connected to the face boards through I2C
 */
{
  int error;
  for(int i = 0; i < 6; i++)
  {
    Wire.beginTransmission(0x20);
    error = Wire.endTransmission();
    if(error == 0)
      return(true);
    wifiDelay(1000);
  }
}
void shutDownMasterBoard()
/*
 * this shuts down just the board with the ESP on it (The one running this software)
 * so that the other board can charge the batteries.
 */
{
  while (true)
  {
    Serial.println("espoff");  // attempt to shut down, with a delay between attempts
    wifiDelay(300);
  }
}

void whatToDoIfIamNotConnectedAtBeginning()
{
  mesh.update();
  resetI2cBus();
  wifiDelay(100);
}

void initializeHardware()
/*
 * This functions sets the pins, starts the serial bus, starts the I2C bus, and checks to
 * see if we are connected to a charger
 */
{
  Serial.begin(115200);       // open serial connection to the Slave Board
  delay(1500);
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
      delay(1000);
    }
  }
  disableAutoReset(); // this disables a feature which would reset this board if it didn't send a special message...
  delay(100);
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


void lookUpCalibrationValues(long wifiID)
{
  if(MAGIC_DEBUG) Serial.println("Entering lookUpCalibrationValues()");
  switch (wifiID)  // used to be ESP.getChipID
  {
    //********************************
    case wifiAddress_cube00: // 9086927 This is the cube on the BIG Breadboard
      thisCubeID = 0;

      TRAVERSE_RPM_F = 6969;
      TRAVERSE_RPM_R = 69696;
      TRAVERSE_CURRENT_F = 69696;
      TRAVERSE_CURRENT_R = 6969;
      GlobalMaxAccel = 12345;    // ** FIX BAT

      CC_RPM_F = 999999;
      CC_RPM_R = 999999;
      CC_CURRENT_F = 999999;
      CC_CURRENT_R = 999999;
      CC_BRAKETIME_F = 999;
      CC_BRAKETIME_R = 999;

      break;
    //********************************

    case wifiAddress_SERVER:   // 13374829 Cube on Smaller Breadboard
      thisCubeID = 99;

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
    //*********Real Cubes*************
    //********************************

    case wifiAddress_cube16:   //959839 PEI BLACK DB:9D:99:1A:BA:23
      thisCubeID = 16;

      TRAVERSE_RPM_F = 6000;
      TRAVERSE_RPM_R = 6000;
      TRAVERSE_CURRENT_F = 2500;
      TRAVERSE_CURRENT_R = 2500;
      GlobalMaxAccel = 5000;    // ** FIX BAT

      CC_RPM_F = 16000;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3100;
      CC_CURRENT_R = 3100;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;

    //********************************
    case wifiAddress_cube14: //959694 PEI PURPLE | FA:AA:25:19:C7:DF
      thisCubeID = 14;

      GlobalPlaneAccel = 2000;
      TRAVERSE_RPM_F = 7000;
      TRAVERSE_RPM_R = 7000;
      TRAVERSE_CURRENT_F = 3000;
      TRAVERSE_CURRENT_R = 3000;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3400;
      CC_CURRENT_R = 3300;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;
    //********************************
    case wifiAddress_cube15: //960348 PEI GREEN | EC:47:A9:35:1F:02
      thisCubeID = 15;

      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 6500;
      TRAVERSE_CURRENT_F = 2700;
      TRAVERSE_CURRENT_R = 2800;

      CC_RPM_F = 15500;
      CC_RPM_R = 15000;
      CC_CURRENT_F = 3500;
      CC_CURRENT_R = 3200;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;

    case wifiAddress_cube07: //960242 PEI ORANGE E6:F6:05:69:08:F2
      thisCubeID = 7;

      TRAVERSE_RPM_F = 7000;
      TRAVERSE_RPM_R = 7000;
      TRAVERSE_CURRENT_F = 3000;
      TRAVERSE_CURRENT_R = 2500;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3400;
      CC_CURRENT_R = 3400;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;

    case wifiAddress_cube09: //8576514 PC BLACK  E3:6B:C6:CE:DA:31
      thisCubeID = 9;

      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 6500;
      TRAVERSE_CURRENT_F = 2800;
      TRAVERSE_CURRENT_R = 3300;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3100;
      CC_CURRENT_R = 3100;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;

    case wifiAddress_cube08: //959709 PC YELLOW  - FB:0D:8F:2C:3B:B4
      thisCubeID = 8;

      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 7000;
      TRAVERSE_CURRENT_F = 3000;
      TRAVERSE_CURRENT_R = 3300;
      GlobalPlaneAccel = 3000;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3100;
      CC_CURRENT_R = 3100;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;

     case wifiAddress_cube11: //8577103 PC ORANGE  - E6:E5:82:26:C7:8B
      thisCubeID = 11;
      GlobalPlaneAccel = 3000;

      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 7000;
      TRAVERSE_CURRENT_F = 3000;
      TRAVERSE_CURRENT_R = 3300;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 2700;
      CC_CURRENT_R = 2800;
      CC_BRAKETIME_F = 5;
      CC_BRAKETIME_R = 10;
      break;

     case wifiAddress_cube02: //10229112 PC PURPLE  - DF:DF:3C:A0:F1:77
      thisCubeID = 2;
      GlobalPlaneAccel = 3000;

      TRAVERSE_RPM_F = 7000;
      TRAVERSE_RPM_R = 7000;
      TRAVERSE_CURRENT_F = 3600;
      TRAVERSE_CURRENT_R = 4000;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3600;
      CC_CURRENT_R = 3900;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;

    case wifiAddress_cube01: //960662 PEI BROWN  - F1:E8:71:B2:99:B5
      thisCubeID = 1;

      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 7000;
      TRAVERSE_CURRENT_F = 2500;
      TRAVERSE_CURRENT_R = 4500;
      GlobalMaxAccel = 5000;    // ** FIX BAT

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3100;
      CC_CURRENT_R = 5400;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 13;
      break;

    case wifiAddress_cube10: //960558 PEI BLUE  f7:AE:59:2B:D9:4D
      thisCubeID = 10;

      TRAVERSE_RPM_F = 7000;
      TRAVERSE_RPM_R = 7000;
      TRAVERSE_CURRENT_F = 3000;
      TRAVERSE_CURRENT_R = 3500;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 2900;
      CC_CURRENT_R = 3900;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 11;
      break;

    case wifiAddress_cube12: //960043 PEI YELLOW  CC:F1:4F:AF:64:A8
      thisCubeID = 12;

      TRAVERSE_RPM_F = 7500;
      TRAVERSE_RPM_R = 7500;
      TRAVERSE_CURRENT_F = 6000;
      TRAVERSE_CURRENT_R = 6000;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 4600;
      CC_CURRENT_R = 5900;
      CC_BRAKETIME_F = 15;
      CC_BRAKETIME_R = 20;
      break;

     case wifiAddress_cube05: //960427 PEI RED  D0:D5:6F:CB:32:4C
      thisCubeID = 5;

      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 6500;
      TRAVERSE_CURRENT_F = 2500;
      TRAVERSE_CURRENT_R = 2500;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3000;
      CC_CURRENT_R = 2900;
      CC_BRAKETIME_F = 8;
      CC_BRAKETIME_R = 5;
      break;

    case wifiAddress_cube03: //15044359 ORANGE PC RED  CD:2B:5E:AB:3E:F3
      thisCubeID = 3;

      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 6500;
      TRAVERSE_CURRENT_F = 2800;
      TRAVERSE_CURRENT_R = 3000;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 2900;
      CC_CURRENT_R = 3800;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 7;
      break;

  case wifiAddress_cube06: //8577715 PC Brown  C5:FF:AB:04:3B:9D
      thisCubeID = 6;

      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 6500;
      TRAVERSE_CURRENT_F = 2800;
      TRAVERSE_CURRENT_R = 3100;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 2800;
      CC_CURRENT_R = 3100;
      CC_BRAKETIME_F = 6;
      CC_BRAKETIME_R = 10;
      break;

    case wifiAddress_cube13: //15044426 PC Blue  D8:9C:4D:EA:27:65
      thisCubeID = 13;

      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 6500;
      TRAVERSE_CURRENT_F = 2500;
      TRAVERSE_CURRENT_R = 2500;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3100;
      CC_CURRENT_R = 2900;
      CC_BRAKETIME_F = 9;
      CC_BRAKETIME_R = 5;
      break;

    case wifiAddress_cube04: //8575308 PC Green : ED:A6:6A:8E:1B:58
      thisCubeID = 4;
      GlobalPlaneAccel = 2500;

      TRAVERSE_RPM_F = 6500;
      TRAVERSE_RPM_R = 6500;
      TRAVERSE_CURRENT_F = 3000;
      TRAVERSE_CURRENT_R = 3000;

      CC_RPM_F = 15500;
      CC_RPM_R = 15500;
      CC_CURRENT_F = 3100;
      CC_CURRENT_R = 3100;
      CC_BRAKETIME_F = 10;
      CC_BRAKETIME_R = 10;
      break;
    break;
  }
  if(MAGIC_DEBUG) Serial.println("Exiting lookUpCalibrationValues()");
}

void loadMotionData(Motion* motion, int RPM, int Current, int brakeTime)
/*
 * This updates the arrays with indivudually calibrated motion data values for the
 * brake the and RPM for several types of moves...
 *
 * This function takes in a Motion Struct, and the values associated with it, and adds the values
 * to the struct.
 */
{
  motion->rpm = RPM;
  motion->current = Current;
  motion->brakeTime = brakeTime;
}

void setMotionDataGlobalVariables()
/*
 * This function goes through the list of motions, and applies
 * "moadMotionData" to each one of them.
 */
{
  loadMotionData(&traverse_F, TRAVERSE_RPM_F, TRAVERSE_CURRENT_F, 10);
  loadMotionData(&traverse_R, TRAVERSE_RPM_R, TRAVERSE_CURRENT_R, 10);
  loadMotionData(&cornerClimb_F, CC_RPM_F, CC_CURRENT_F, CC_BRAKETIME_F);
  loadMotionData(&cornerClimb_R, CC_RPM_R, CC_CURRENT_R, CC_BRAKETIME_R);
  loadMotionData(&roll_F, 7, 7, 7);
  loadMotionData(&roll_R, 7, 7, 7);
  loadMotionData(&rollDouble_F, (CC_RPM_F-1000), (CC_CURRENT_F-300), ((CC_BRAKETIME_F/2)+2));
  loadMotionData(&rollDouble_R, (CC_RPM_R-1000), (CC_CURRENT_R-300), ((CC_BRAKETIME_R/2)+2));
}


void resetI2cBus()
{

  digitalWrite(Switch, LOW); // this turns off power to the I2C electronics
  for(int i = 0 ; i<10 ; i++)
  {
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
    delay(50);
  }
  digitalWrite(Switch, HIGH);
}
