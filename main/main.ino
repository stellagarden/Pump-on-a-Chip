// -----------------------------------------------------------
// FileName     : main.ino
// Author       : Hyemin Stella Lee
// Created      : 04/19/2023
// Description  : Runs overall pump-on-a-chip system
// -----------------------------------------------------------

#include <Wire.h>
#include "Goldelox_Serial_4DLib.h"
#include "Goldelox_Const4D.h"

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

// uLCD
#define DisplaySerial Serial1
#define LCD_Reset 8
Goldelox_Serial_4DLib Display(&DisplaySerial);

void LCD_print(float a, float b) {
  Display.txt_MoveCursor(2,0) ;
  Display.print(a);
  Display.txt_MoveCursor(6,0) ;
  Display.print(b);
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

  // uLCD
  pinMode(LCD_Reset, OUTPUT);
  digitalWrite(LCD_Reset, 0);
  delay(100);
  digitalWrite(LCD_Reset, 1);
  delay(3000);
  Display.TimeLimit4D = 3000 ; // 3 second timeout on all commands
  DisplaySerial.begin(9600);
  Display.txt_FGcolour(WHITE);
  Display.gfx_Cls() ;
  Display.txt_Height(2) ;
  Display.txt_Width(2) ;
  Display.putstr("Pump (mbar)") ;
  Display.putstr("\n\n\n\nChip (mbar)");
}

void loop() {
  // Gauge
  Wire.requestFrom(GAUGE_ADDR, 2);  // requests 2 bytes from the specified address
  if (Wire.available() > 0){
    rawdata = Wire.read();
    rawdata = rawdata << 8;
    rawdata |= Wire.read();
    pressure = 0.316456*rawdata-1580.74;  // Convert raw data to actual pressure
    LCD_print(pressure, 2000.0-pressure);
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