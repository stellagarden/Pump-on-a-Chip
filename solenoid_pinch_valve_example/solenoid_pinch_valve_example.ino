// -----------------------------------------------------------
// FileName     : solenoid_pinch_valve_example.ino
// Author       : Hyemin Stella Lee
// Created      : 4/6/2023
// Description  : Turn on and off solenoid pinch valve with push button
// -----------------------------------------------------------

#include <Wire.h>

#define SOLPINCH_PIN 3
#define BUTTON_PIN 2
byte solPinchState = LOW;

void solPinch() {
  solPinchState = !solPinchState;
}

void setup() {
  pinMode(SOLPINCH_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), solPinch, RISING);
}

void loop() {
  if (solPinchState) {
    analogWrite(SOLPINCH_PIN, 240);   // close
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    analogWrite(SOLPINCH_PIN, 0);     // open
    digitalWrite(LED_BUILTIN, HIGH);
  }

  delay(100);
}