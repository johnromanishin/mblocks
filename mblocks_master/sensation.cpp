#include <Wire.h>
#include "sensation.h"
#include <ArduinoHardware.h>

int readAmbient(int address)
{
  activateLightSensor(address);
  delay(15);
  int reading = 0;
  Wire.beginTransmission(address); 
  Wire.write(byte(0x8C)); // this is the register where the Ambient values are stored
  Wire.endTransmission();
  Wire.requestFrom(address, 2);
  if (2 <= Wire.available()) 
  {
    reading = Wire.read();
    reading |= Wire.read()<<8;
  }
  return reading;
}

void activateLightSensor(int address)
{
  Wire.beginTransmission(address); 
  Wire.write(byte(0x80)); // 1000 0000 - Selects command register
  Wire.write(byte(0x03)); // 0000 0010 - Activates device
  Wire.endTransmission();
  Wire.beginTransmission(address); 
  Wire.write(byte(0x81));
  Wire.write(byte(0x10)); // Sets integration time to 15 ms ... // 00010XX sets gain to 16x
  // 0x10 = 100 ms
  // 0x10 = 14ms w/ 16x High Gain
  Wire.endTransmission();
}

void readGyroAndAccel(int i2cAddress)
{
  int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
  wakeMPU(i2cAddress);
  Wire.beginTransmission(i2cAddress);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(i2cAddress, 14, 1); // 7 reads of 2 bytes
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}


void wakeMPU(int i2cAddress)
// Wakes the MPU_6050 accelerometer
{
  Wire.beginTransmission(i2cAddress);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

int magnetSensorAngle(int i2cAddress) {
  // Returns the angle of the measured magnet as a 14 bit number
  magnetSensorRead(i2cAddress, byte(0xFF));
}

int magnetSensorFieldStrength(int i2cAddress) {
  // AGC is the "strength" of the magnet returned as an 8-bit number, 255 = magnet field is too weak, 0 = very strong magnetic field.
  magnetSensorRead(i2cAddress, byte(0xFA));
}

int magnetSensorRead(int i2cAddress, byte dataRegisterAddress) {
  // read either the angle or the field strength of the AMS5048
  Wire.beginTransmission(i2cAddress);
  Wire.write(dataRegisterAddress);
  Wire.endTransmission();
  Wire.requestFrom(i2cAddress, 2); // request 2 bytes


  if (2 <= Wire.available())
  {
    int sensorValue;
    sensorValue = Wire.read();
    sensorValue = sensorValue << 6;
    sensorValue |= Wire.read();
    return sensorValue;
  }
  return 0;
}

