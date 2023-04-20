// -----------------------------------------------------------
// FileName     : gauge_example-read_rawdata.ino
// Author       : Hyemin Stella Lee
// Created      : 4/5/2023
// Description  : Read pressure from gauge and print it as mbar unit
// -----------------------------------------------------------

#include <Wire.h>

int address_sensor= 56; // 0x38
uint16_t rawdata = 0;
float pressure = 0;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  Wire.begin();

  Wire.beginTransmission(address_sensor); //Send a request to begin communication with the device at the specified address
  Wire.endTransmission();
}

void loop(){
  
  Wire.requestFrom(address_sensor, 2);    // requests 2 bytes from the specified address

  if (Wire.available() > 0){
    rawdata = Wire.read();
    rawdata = rawdata << 8;
    rawdata |= Wire.read();
    
    pressure = 0.316456*rawdata-1580.74;    // Convert rawdata to actual pressure
    Serial.println(pressure);
    Serial1.println(pressure);
  }  

  delay(100);
}