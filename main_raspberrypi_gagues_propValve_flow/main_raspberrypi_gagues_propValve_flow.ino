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
// Gauge1 4,5 | Gauge2 2,3
#define SOLPINCH_PIN 6
#define PUMP_SOL_PIN 7
#define P_VENT_PIN 8
#define C_VENT_PIN 9
const float pumpTimer = 3000;
const unsigned long cellLoadTimer = 2000;
const unsigned long ventTimer = 1000;
const unsigned long runningTimer = 10000;

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
float prop_target = 0.0;
int mode;
float target;
unsigned char state;
enum {Ready, Admin_C, Admin_P, Admin_A, Admin_B, Pressurizing, CellLoading, Running, Finish};

void setup() {
  Serial.begin(9600);
  state = Ready;
  digitalWrite(LED_BUILTIN, OUTPUT);
  
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(SOLPINCH_PIN, OUTPUT);
  pinMode(PUMP_SOL_PIN, OUTPUT);
  pinMode(P_VENT_PIN, OUTPUT);
  pinMode(C_VENT_PIN, OUTPUT);
  pinMode(PROPOR_PIN, OUTPUT);
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

  // Reset valves
  analogWrite(PROPOR_PIN,250);
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
                mode = data.charAt(1)-'0';      // 0: pressure, 1: flowrate
                target = data.substring(2).toFloat();
                // To-do: Run pump and pressurize the pressure reservoir
                digitalWrite(PUMP_PIN, HIGH);
                digitalWrite(PUMP_SOL_PIN, LOW);     // open
                // /////////////////////////////////////////////////////
                timerMillis = millis();
                state = Pressurizing;
                break;
              case 'C':
                // Cell Loading Pinch valve
                digitalWrite(SOLPINCH_PIN, HIGH);     // open
                digitalWrite(LED_BUILTIN, HIGH);
                timerMillis = millis();
                state = Admin_C;
                break;
              case 'P':
                // Pump
                digitalWrite(PUMP_PIN, HIGH);
                digitalWrite(PUMP_SOL_PIN, LOW);     // open
                digitalWrite(LED_BUILTIN, HIGH);
                timerMillis = millis();
                state = Admin_P;
                break;
              case 'A':
                // Vent Pressure Reservoir
                digitalWrite(P_VENT_PIN, LOW);     // open
                timerMillis = millis();
                state = Admin_A;
                break;
              case 'B':
                // Vent Cell Reservoir
                digitalWrite(C_VENT_PIN, LOW);     // open
                timerMillis = millis();
                state = Admin_B;
                break;
              case 'V':
                // Proportional Valve
                prop_target = data.substring(1).toFloat();
                int prop_int = prop_target*255/100;
                Serial.println("*Set prop valve as "+String(prop_int));
                analogWrite(PROPOR_PIN, prop_int);
                break;
            }
      }
      break;
    case Admin_C:
      if (accumulatedMillis > cellLoadTimer) {
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(SOLPINCH_PIN, LOW);     // close
        state = Ready;
      }
      break;
    case Admin_P:
      if (accumulatedMillis > pumpTimer) {
        state = Ready;
        digitalWrite(PUMP_PIN, LOW);
        digitalWrite(PUMP_SOL_PIN, HIGH);     // close
        digitalWrite(LED_BUILTIN, LOW);
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
      if (accumulatedMillis > pumpTimer){
        digitalWrite(PUMP_PIN, LOW);
        digitalWrite(PUMP_SOL_PIN, HIGH);     // close
        Serial.println("S2");
        digitalWrite(SOLPINCH_PIN, HIGH);     // open
        digitalWrite(LED_BUILTIN, HIGH);
        state = CellLoading;
        timerMillis = millis();
      }
      break;
    case CellLoading:
      if (accumulatedMillis > cellLoadTimer){
        Serial.println("S3");
        digitalWrite(SOLPINCH_PIN, LOW);     // close
        digitalWrite(LED_BUILTIN, LOW);
        state = Running;
        timerMillis = millis();
      }
      break;
    case Running:
      // To-do: Make the target pressure in the cell reservoir or the flowrate
      //
      //
      //
      // ///////////////////////////////////////////////////
      if (accumulatedMillis > runningTimer){
        Serial.println("S4");
        digitalWrite(P_VENT_PIN, LOW);     // open
        digitalWrite(C_VENT_PIN, LOW);     // open
        state = Finish;
        timerMillis = millis();
      }
      break;
    case Finish:
      if (accumulatedMillis > ventTimer){
        Serial.println("S0");
        digitalWrite(P_VENT_PIN, HIGH);     // close
        digitalWrite(C_VENT_PIN, HIGH);     // close
        state = Ready;
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