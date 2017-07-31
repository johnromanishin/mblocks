void initializeCube()
{
  if (inputVoltage() > 3400) {
    shutDownMasterBoard(); // This turns off ESP if we are on a charging pad
  }
}




void shutDownMasterBoard()
{
  while (true) {
    Serial.println("espoff");  // attempt to shut down, with a delay between attempts
    delay(300);
  }
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
