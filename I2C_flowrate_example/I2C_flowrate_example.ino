// -----------------------------------------------------------
// FileName     : I2C_flowrate_example.ino
// Author       : Hyemin Stella Lee
// Created      : 05/18/2023
// Description  : Read flowrate from the sensor and print it as ml/min
//                via serial monitor in PC
// -----------------------------------------------------------

#include <Wire.h>

#define FLOWRATE_ADDR 8  // 0x08
uint16_t rawdata = 0;
float flowrate = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(FLOWRATE_ADDR);
  Wire.write(0x3608);  // Start continuous reading
  Wire.endTransmission();
}

void loop() {
  Wire.requestFrom(FLOWRATE_ADDR, 2);  // requests 2 bytes from the the sensor
  if (Wire.available() > 0){
    rawdata = Wire.read();
    rawdata = rawdata << 8;
    rawdata |= Wire.read();
    flowrate = rawdata / 500;  // Convert raw data to actual flowrate
    Serial.print(flowrate);
    Serial.println(" ml/min");
  }
  delay(10);
}