// -----------------------------------------------------------
// FileName     : proportional_valve_example.ino
// Author       : Hyemin Stella Lee
// Created      : 05/02/2023
// Description  : Control proportional
// -----------------------------------------------------------

#include <Wire.h>

#define PROPOR_PIN 1

int open = 0;

void setup() {
  Serial.begin(9600);

  pinMode(PROPOR_PIN, OUTPUT);
  analogWriteResolution(10);
}

void loop() {
  analogWrite(PROPOR_PIN, 1023);
  delay(3000);
  analogWrite(PROPOR_PIN, 750);
  delay(3000);
  analogWrite(PROPOR_PIN, 512);
  delay(3000);
  analogWrite(PROPOR_PIN, 256);
  delay(3000);
  analogWrite(PROPOR_PIN, 0);
  delay(3000);
}
