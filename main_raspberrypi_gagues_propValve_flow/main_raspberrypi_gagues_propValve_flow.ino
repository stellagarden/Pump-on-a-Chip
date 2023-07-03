// -----------------------------------------------------------
// FileName     : main_raspberrypi_gagues_propValve_flow.ino
// Author       : Hyemin Stella Lee
// Created      : 06/15/2023
// Description  : The whole system operating based on the protocol and manual mode
// -----------------------------------------------------------

#include <Wire.h>
#include <SoftwareI2C.h>
#include "sensirion-lf.h"

// Pinch
#define PROPOR_PIN 1
#define PUMP_PIN 6
// Gauge1 4,5 | Gauge2 2,3
#define SOLPINCH_PIN 7
#define PUMP_SOL_PIN 8
#define P_VENT_PIN 9
#define C_VENT_PIN 10
const float pResTarget = 3000.0;
const unsigned long cellLoadTimer = 2000;
const float ventTarget = 1010.0;        // Stop venting 1s after reaching this value
const unsigned long ventTimer = 5000;   // Or stop venting after this time
const unsigned long runningTimer = 20000;

// System
#define accumulatedMillis millis() - timerMillis
#define GAUGE_ADDR 56 // 0x38
#define OPEN HIGH
#define CLOSE LOW
unsigned long timerMillis;

SoftwareI2C softwarei2c;

uint16_t raw_resP = 0;
float resP = 0;
uint16_t raw_cellP = 0;
float cellP = 0;
float atP = 1000;
float flowrate = 0.0;
float prop_target = 0.0;
int prop_int = 0;
int mode;
float target;
unsigned char state;
enum {Ready, Admin_C, Admin_P, Admin_A, Admin_A_1s, Admin_B, Admin_B_1s,
    Pressurizing_vent, Pressurizing, CellLoading, Running_cellP, Running, Finish};

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
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(PUMP_SOL_PIN, CLOSE);
  digitalWrite(P_VENT_PIN, CLOSE);
  digitalWrite(C_VENT_PIN, CLOSE);
  analogWrite(PROPOR_PIN, 0);
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
                digitalWrite(P_VENT_PIN, OPEN);
                digitalWrite(C_VENT_PIN, OPEN);
                timerMillis = millis();
                state = Pressurizing_vent;
                break;
              case 'C':
                // Cell Loading Pinch valve
                digitalWrite(SOLPINCH_PIN, HIGH);
                timerMillis = millis();
                state = Admin_C;
                break;
              case 'P':
                // Pump
                digitalWrite(PUMP_PIN, HIGH);
                digitalWrite(PUMP_SOL_PIN, OPEN);
                state = Admin_P;
                break;
              case 'A':
                // Vent Pressure Reservoir
                digitalWrite(P_VENT_PIN, OPEN);
                timerMillis = millis();
                state = Admin_A;
                break;
              case 'B':
                // Vent Cell Reservoir
                digitalWrite(C_VENT_PIN, OPEN);
                timerMillis = millis();
                state = Admin_B;
                break;
              case 'V':
                // Proportional Valve
                prop_target = data.substring(1).toFloat();
                prop_int = prop_target*1023/100;
                Serial.println("*Set prop valve as "+String(prop_int));
                analogWrite(PROPOR_PIN, prop_int);
                break;
            }
      }
      break;
    case Admin_C:
      if (accumulatedMillis > cellLoadTimer) {
        digitalWrite(SOLPINCH_PIN, CLOSE);
        state = Ready;
      }
      break;
    case Admin_P:
      if (resP > pResTarget) {
        digitalWrite(PUMP_PIN, LOW);
        digitalWrite(PUMP_SOL_PIN, CLOSE);
        state = Ready;
      }
      break;
    case Admin_A:
      if (accumulatedMillis > ventTimer - 1.0 || resP < ventTarget) {
        timerMillis = millis();
        state = Admin_A_1s;
      }
      break;
    case Admin_A_1s:
      if (accumulatedMillis > 1) {
        digitalWrite(P_VENT_PIN, CLOSE);
        state = Ready;
      }
    case Admin_B:
      if (accumulatedMillis > ventTimer - 1.0 || cellP < ventTarget) {
        timerMillis = millis();
        state = Admin_B_1s;
      }
      break;
    case Admin_B_1s:
      if (accumulatedMillis > 1) {
        digitalWrite(C_VENT_PIN, CLOSE);
        state = Ready;
      }
      break;
    case Pressurizing_vent:
      if (accumulatedMillis > 1.0) {
        digitalWrite(P_VENT_PIN, CLOSE);
        digitalWrite(C_VENT_PIN, CLOSE);
        atP = cellP;
        Serial.println("*P_at: "+String(atP)+" mbar");
        digitalWrite(PUMP_PIN, HIGH);
        digitalWrite(PUMP_SOL_PIN, OPEN);
        state = Pressurizing;
      }
      break;
    case Pressurizing:
      if (resP > pResTarget){
        digitalWrite(PUMP_PIN, LOW);
        digitalWrite(PUMP_SOL_PIN, CLOSE);
        Serial.println("S2");
        digitalWrite(SOLPINCH_PIN, OPEN);
        state = CellLoading;
        timerMillis = millis();
      }
      break;
    case CellLoading:
      if (accumulatedMillis > cellLoadTimer){
        Serial.println("S3");
        digitalWrite(SOLPINCH_PIN, CLOSE);
        state = Running;
        timerMillis = millis();
      }
      break;
    case Running:
      if (accumulatedMillis > runningTimer){
        Serial.println("S4");
        // digitalWrite(P_VENT_PIN, OPEN);
        // digitalWrite(C_VENT_PIN, OPEN);
        state = Finish;
        timerMillis = millis();
      } else {
        int prop_int_nxt = 0;
        if (mode == 0) {    // Pressure
          // CALIBRATION NEEDED
          if (cellP - atP < target - 200) {
            prop_int_nxt = 590;
          } else if (cellP - atP < target - 30) {
            prop_int_nxt = 490;
          } else {
            prop_int_nxt = 0;
          }
          if (prop_int_nxt != prop_int){
            analogWrite(PROPOR_PIN, prop_int_nxt);
            Serial.println("*Set prop valve as "+String(prop_int_nxt));
            prop_int = prop_int_nxt;
          }
          // ////////////////////////
        }
        // To-do: Make the target flow rate in the cell reservoir
        else {
          
        }
        // ///////////////////////////////////////////////////
      }
      break;
    case Finish:
      if (accumulatedMillis > ventTimer){
        Serial.println("S0");
        // digitalWrite(P_VENT_PIN, CLOSE);
        // digitalWrite(C_VENT_PIN, CLOSE);
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