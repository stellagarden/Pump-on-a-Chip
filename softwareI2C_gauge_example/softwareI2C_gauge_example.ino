// -----------------------------------------------------------
// FileName     : softwareI2C_gauge_example.ino
// Author       : Hyemin Stella Lee
// Created      : 04/24/2023
// Description  : Read pressure from gauge using softwareI2C
//                and print it as mbar unit via serial monitor in PC
// -----------------------------------------------------------

#include <SoftwareI2C.h>

#define GAUGE_ADDR 56 // 0x38
uint16_t rawdata = 0;
float pressure = 0;
SoftwareI2C softwarei2c;

void setup(){
  Serial.begin(9600);
  softwarei2c.begin(2,3);
  softwarei2c.beginTransmission(GAUGE_ADDR); //Send a request to begin communication with the device at the specified address
  softwarei2c.endTransmission();
}

void loop(){
  softwarei2c.requestFrom(GAUGE_ADDR, 2);  // requests 2 bytes from the specified address
  if (softwarei2c.available() > 0){
    rawdata = softwarei2c.read();
    rawdata = rawdata << 8;
    rawdata |= softwarei2c.read();
    pressure = 0.316456*rawdata-1590.23;  // Convert raw data to actual pressure
    Serial.print(pressure);
    Serial.print(" mbar | P_at + ");
    Serial.print(pressure-1013.25);
    Serial.println(" mbar");
  }  
  
  delay(100);
}