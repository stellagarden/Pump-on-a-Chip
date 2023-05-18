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
  analogWrite(PROPOR_PIN, 250);
  // digitalWrite(LED_BUILTIN, HIGH);
  // Serial.println(open);
  // delay(500);
  // digitalWrite(LED_BUILTIN, LOW);
  // open = open + 20;
  // if (open > 250) open = 0;
  delay(1500);
}
