// -----------------------------------------------------------
// FileName     : I2C_gauge_example.ino
// Author       : Hyemin Stella Lee
// Created      : 04/05/2023
// Description  : Read pressure from gauge and print it as mbar unit
//                via serial monitor in PC
// -----------------------------------------------------------

#include <Wire.h>

#define GAUGE_ADDR 56 // 0x38
uint16_t rawdata = 0;
float pressure = 0;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(GAUGE_ADDR); //Send a request to begin communication with the device at the specified address
  Wire.endTransmission();
}

void loop(){
  Wire.requestFrom(GAUGE_ADDR, 2);  // requests 2 bytes from the specified address
  if (Wire.available() > 0){
    rawdata = Wire.read();
    rawdata = rawdata << 8;
    rawdata |= Wire.read();
    pressure = 0.316456*rawdata-1590.23;  // Convert raw data to actual pressure
    Serial.print(pressure);
    Serial.println(" mbar");
  }  
  delay(1000);
}