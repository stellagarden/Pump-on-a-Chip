// -----------------------------------------------------------
// FileName     : I2C_change_address.ino
// Author       : Hyemin Stella Lee
// Created      : 04/23/2023
// Description  : Change address of I2C device
// -----------------------------------------------------------

#include <Wire.h>

const int SLAVE_ADDR = 0x38;

void setup() {
  Wire.begin(); // Enable I2C communication as master
  Serial.begin (9200); // Enable communication back to host PC at 9200 bps
}

void loop() {
  Wire.beginTransmission(SLAVE_ADDR);
  int error = Wire.endTransmission();
  if( error != 0)
  {
    Serial.println( "The sensor is not at 0x38");
  }
  else
  {
    Serial.println( "The sensor is found, changing I2C address");
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write( 0x53);  // password register
    Wire.write( 0xAA);  // password
    Wire.endTransmission();

    delay(10);    // not described somewhere, just for safety

    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write( 0x00);  // I2C address register
    Wire.write( 0x50);  // new I2C address
    Wire.endTransmission();
  }
}