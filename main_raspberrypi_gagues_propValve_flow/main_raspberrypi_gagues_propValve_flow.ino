// -----------------------------------------------------------
// FileName     : main_raspberrypi_gagues_propValve_flow.ino
// Author       : Hyemin Stella Lee
// Created      : 06/15/2023
// Description  : The whole system operating based on the protocol
// -----------------------------------------------------------

#include <Wire.h>
#include <SoftwareI2C.h>
#include "sensirion-lf.h"

// Pinch
#define PROPOR_PIN A0
#define PUMP_PIN 1
// Gauge1 4,5 | Gauge2 2,3 | Serial 6,7
#define SOLPINCH_PIN 8
#define PUMP_SOL_PIN 9
#define P_VENT_PIN 10
#define C_VENT_PIN 11
const float pumpTarget = 3000;
const unsigned long cellLoadTimer = 2000;
const unsigned long ventTimer = 1000;

// System
#define accumulatedMillis millis() - timerMillis
#define GAUGE_ADDR 56 // 0x38
unsigned long timerMillis;

SoftwareI2C softwarei2c;

uint16_t raw_resP = 0;
float resP = 0;
uint16_t raw_cellP = 0;
float cellP = 0;
float flowrate = 0.0;
float prop = 0.0;
float prop_target;

enum {Ready, Admin_C, Admin_P, Admin_A, Admin_B, Pressurizing, CellLoading, Running, Finish};
unsigned char state;

void setup() {
  Serial.begin(9600);
  state = Ready;
  
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(SOLPINCH_PIN, OUTPUT);
  pinMode(PUMP_SOL_PIN, OUTPUT);
  pinMode(P_VENT_PIN, OUTPUT);
  pinMode(C_VENT_PIN, OUTPUT);
  analogWriteResolution(10);
  // Gauge1
  Wire.begin();
  Wire.beginTransmission(GAUGE_ADDR);
  Wire.endTransmission();
  // Gauge2
  softwarei2c.begin(2,3);
  softwarei2c.beginTransmission(GAUGE_ADDR);
  softwarei2c.endTransmission();
  // Flow rate
  SLF3X.init();
}

void loop() {
  // Status control
  switch (state) {
    case Ready:
      // Administrator Mode
      if (Serial.available() > 0){
            String data = Serial.readString();
            switch (data.charAt(0)){
              case 'S':
                // Start button
                if (data.charAt(1) == '0'){
                  // Pressure input
                  float pressure_target = data.substring(2).toFloat();
                  Serial.println("*Set pressure target as "+String(pressure_target));
                } else {
                  // Flow rate input
                  float flowrate_target = data.substring(2).toFloat();
                  Serial.println("*Set flow rate target as "+String(flowrate_target));
                }
                state = Pressurizing;
                break;
              case 'C':
                // Cell Loading Pinch valve
                timerMillis = millis();
                digitalWrite(SOLPINCH_PIN, LOW);     // open
                state = Admin_C;
                break;
              case 'P':
                // Pump
                digitalWrite(PUMP_PIN, HIGH);
                digitalWrite(PUMP_SOL_PIN, LOW);     // open
                state = Admin_P;
                break;
              case 'A':
                // Vent Pressure Reservoir
                timerMillis = millis();
                digitalWrite(P_VENT_PIN, LOW);     // open
                state = Admin_A;
                break;
              case 'B':
                // Vent Cell Reservoir
                timerMillis = millis();
                digitalWrite(C_VENT_PIN, LOW);     // open
                state = Admin_B;
                break;
              case 'V':
                // Proportional Valve
                prop_target = data.substring(1).toFloat();
                analogWrite(PROPOR_PIN, int(prop_target*255/100));
                break;
            }
      }
      break;
    case Admin_C:
      if (accumulatedMillis > cellLoadTimer) {
        state = Ready;
        digitalWrite(SOLPINCH_PIN, HIGH);     // close
      }
      break;
    case Admin_P:
      if (resP > pumpTarget) {
        state = Ready;
        digitalWrite(PUMP_PIN, LOW);
        digitalWrite(PUMP_SOL_PIN, HIGH);     // close
      }
      break;
    case Admin_A:
      if (accumulatedMillis > ventTimer) {
        state = Ready;
        digitalWrite(P_VENT_PIN, HIGH);     // close
      }
      break;
    case Admin_B:
      if (accumulatedMillis > ventTimer) {
        state = Ready;
        digitalWrite(C_VENT_PIN, HIGH);     // close
      }
      break;
    case Pressurizing:
      if (accumulatedMillis > 2000){
        state = CellLoading;
        Serial.println("S2");
        timerMillis = millis();
      }
      break;
    case CellLoading:
      if (accumulatedMillis > 2000){
        state = Running;
        Serial.println("S3");
        timerMillis = millis();
      }
      break;
    case Running:
      if (accumulatedMillis > 2000){
        state = Finish;
        Serial.println("S4");
        timerMillis = millis();
      }
      break;
    case Finish:
      if (accumulatedMillis > 2000){
        state = Ready;
        Serial.println("S0");
        timerMillis = millis();
      }
      break;
  }


  // Sensor values print

  // Gauge1
  Wire.requestFrom(GAUGE_ADDR, 2);
  if (Wire.available() > 0){
    raw_resP = Wire.read();
    raw_resP = raw_resP << 8;
    raw_resP |= Wire.read();
    resP = 0.316456*raw_resP-1590.23;
    Serial.println("P"+String(resP));
  }  

  // Gauge2
  softwarei2c.requestFrom(GAUGE_ADDR, 2);
  if (softwarei2c.available() > 0){
    raw_cellP = softwarei2c.read();
    raw_cellP = raw_cellP << 8;
    raw_cellP |= softwarei2c.read();
    cellP = 0.316456*raw_cellP-1590.23;
    Serial.println("C"+String(cellP));
  }

  // Flow rate
  if (SLF3X.readSample() == 0) {
    flowrate = SLF3X.getFlow();
    Serial.println("F"+String(flowrate));
  }
  
  Serial.println("V"+String(prop_target));

  delay(100);
  
}