// -----------------------------------------------------------
// FileName     : I2C_gauge_example.ino
// Author       : Hyemin Stella Lee
// Created      : 04/05/2023
// Description  : Read pressure from gauge and print it as mbar unit
//                via serial monitor in PC
// -----------------------------------------------------------

#include <Wire.h>

#define PROPOR_PIN 1
#define PUMP_PIN 6
// Gauge1 4,5 | Gauge2 2,3
#define PUMP_SOL_PIN 8
#define P_VENT_PIN 9
#define C_VENT_PIN 10

#define GAUGE_ADDR 56 // 0x38
#define OPEN HIGH
#define CLOSE LOW
uint16_t rawdata = 0;
float pressure = 0;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(GAUGE_ADDR); //Send a request to begin communication with the device at the specified address
  Wire.endTransmission();
  
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(PUMP_SOL_PIN, OUTPUT);
  pinMode(P_VENT_PIN, OUTPUT);
  pinMode(C_VENT_PIN, OUTPUT);
  pinMode(PROPOR_PIN, OUTPUT);
  analogWriteResolution(10);
  
  // Reset valves
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(PUMP_SOL_PIN, CLOSE);
  digitalWrite(P_VENT_PIN, CLOSE);
  digitalWrite(C_VENT_PIN, CLOSE);
  analogWrite(PROPOR_PIN, 0);
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