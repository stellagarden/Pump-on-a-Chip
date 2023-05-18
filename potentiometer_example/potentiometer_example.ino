// -----------------------------------------------------------
// FileName     : potentiometer_example.ino
// Author       : Hyemin Stella Lee
// Created      : 05/02/2023
// Description  : Print valve of potentiometer on Serial monitor
// -----------------------------------------------------------

#define POT_PIN 10

void setup() {
  pinMode(POT_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.println(analogRead(POT_PIN));
  delay(100);
}