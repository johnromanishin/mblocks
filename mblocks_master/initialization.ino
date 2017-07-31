void initializeCube()
{
if(get_vin() > 3400) {turn_off_esp();} // This turns off ESP if we are on a charging pad 
}




void turn_off_esp()
{
for(int i = 0; i < 2;i++){Serial.println("fbsleep");delay(300);}// Tell the slaveboard NRF51422 to sleep the face-boards 3 seperate times.
for(int i = 0; i < 4;i++){Serial.println("espoff");delay(300);}// This should turn the lights completely off... Suicide basically
}


int get_vin()
// Obtains the input voltage from the Slave Board - this is used to detect when the module is charging, and to put itself to sleep
{
  int timeout_time = 60; // ms - how long the function will attempt to parse text for.
  long begin_function = millis();
  char prev_char      = ' ';
  String temp_string  = "";
  int temp_vin        = 0;
  while(Serial.available()){Serial.read();} // empty serial buffer just in case...
  delay(2);
  Serial.println("vin");
  delay(23);                                // We need to wait for the slave board to get its shit together
  while (Serial.available() > 0 && (millis()-begin_function) < timeout_time) // while there are things in the serial buffer...
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
              temp_vin = temp_string.toInt();
        
          }
        delayMicroseconds(250);
        prev_char = c;
  }
  return(temp_vin);
}
