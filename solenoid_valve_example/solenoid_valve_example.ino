// -----------------------------------------------------------
// FileName     : solenoid_valve_example.ino
// Author       : Hyemin Stella Lee
// Created      : 04/23/2023
// Description  : Turn on and off solenoid valve with push button
// -----------------------------------------------------------

#include <Wire.h>

#define SOL_PIN 3
#define BUTTON_PIN 2
byte solState = LOW;

void sol() {
  solState = !solState;
}

void setup() {
  pinMode(SOL_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), sol, RISING);
}

void loop() {
  if (solState) {
    analogWrite(SOL_PIN, 240);   // close
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    analogWrite(SOL_PIN, 0);     // open
    digitalWrite(LED_BUILTIN, HIGH);
  }

  delay(100);
}