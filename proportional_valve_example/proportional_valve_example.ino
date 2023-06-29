// -----------------------------------------------------------
// FileName     : proportional_valve_example.ino
// Author       : Hyemin Stella Lee
// Created      : 05/02/2023
// Description  : Control proportional
// -----------------------------------------------------------

#include <Wire.h>

#define PROPOR_PIN A0

int open = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(PROPOR_PIN, OUTPUT);
  analogWriteResolution(10);
}

void loop() {
  Serial.println(open);
  open = 1023;
  analogWrite(PROPOR_PIN, open);
  delay(1500);
  open = 0;
  analogWrite(PROPOR_PIN, open);
  delay(1500);
}
