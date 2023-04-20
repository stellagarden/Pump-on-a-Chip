// -----------------------------------------------------------
// FileName     : main.ino
// Author       : Hyemin Stella Lee
// Created      : 04/19/2023
// Description  : Runs overall pump-on-ac-chip system
// -----------------------------------------------------------

#include <Wire.h>

// Gauge
#define GAUGE_ADDR 56 // 0x38
uint16_t rawdata = 0;
float pressure = 0;

// Solenoid Pinch
#define SOLPINCH_PIN 3
#define BUTTON_PIN 2
byte solPinchState = LOW;

void solPinch() {
  solPinchState = !solPinchState;
}

void setup() {
  // Gauge
  Serial.begin(9600);
  Serial1.begin(9600);
  Wire.begin();
  Wire.beginTransmission(GAUGE_ADDR); //Send a request to begin communication with the device at the specified address
  Wire.endTransmission();

  // Solenoid Pinch
  pinMode(SOLPINCH_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), solPinch, RISING);
}

void loop() {
  // Gauge
  Wire.requestFrom(GAUGE_ADDR, 2);  // requests 2 bytes from the specified address
  if (Wire.available() > 0){
    rawdata = Wire.read();
    rawdata = rawdata << 8;
    rawdata |= Wire.read();
    pressure = 0.316456*rawdata-1580.74;  // Convert raw data to actual pressure
    Serial.print(pressure);
    Serial.print(" mbar | P_at + ");
    Serial.print(pressure-1013.25);
    Serial.println(" mbar");
    Serial1.print(pressure);
    Serial1.print(" mbar | P_at + ");
    Serial1.print(pressure-1013.25);
    Serial1.println(" mbar");
  }  

  // Solenoid Pinch
  if (solPinchState) {
    analogWrite(SOLPINCH_PIN, 240);
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    analogWrite(SOLPINCH_PIN, 0);
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(100);
}