// -----------------------------------------------------------
// FileName     : main_raspberrypi_WinForm.ino
// Author       : Hyemin Stella Lee
// Created      : 07/26/2023
// Description  : Control the whole system with WinForm UI
// -----------------------------------------------------------

#include <Wire.h>
#include <SoftwareI2C.h>
#include "sensirion-lf.h"

#define PROPOR_PIN 1
#define PUMP_PIN 6
// Gauge1 4,5 | Gauge2 2,3
#define PROP_SOL_PIN 7
#define PUMP_SOL_PIN 8
#define P_VENT_PIN 9
#define C_VENT_PIN 10
const unsigned long ventTimer = 5000;
const unsigned long runningTimer = 100000;
const unsigned long pumpMaxTimer = 15000;

#define accumulatedMillis millis() - timerMillis
#define GAUGE_ADDR 56 // 0x38
#define OPEN HIGH
#define CLOSE LOW
unsigned long timerMillis;

SoftwareI2C softwarei2c;

float pResTarget = 1000.0;
float pCellTarget = 300.0;
uint16_t raw_resP = 0;
float resP = 0;
uint16_t raw_cellP = 0;
float cellP = 0;
float atP = 0;
float flowrate = 0.0;
int prop_target = 0;
int prop_int = 0;
int prop_int_nxt = 0;
int atP_measuring = 0;
unsigned char state;
enum {MeasurePat_vent, Ready, Admin_P, Admin_A, Admin_B,
    Pressurizing, ProvidePressure, Running, Finish};

void setup() {
  Serial.begin(9600);
  state = Ready;
  
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(PROP_SOL_PIN, OUTPUT);
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
  // // Flow rate
  // SLF3X.init();

  // Reset valves
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(PROP_SOL_PIN, CLOSE);
  digitalWrite(PUMP_SOL_PIN, CLOSE);
  digitalWrite(P_VENT_PIN, CLOSE);
  digitalWrite(C_VENT_PIN, CLOSE);
  prop_int = 0;
  prop_target = 0;
  analogWrite(PROPOR_PIN, prop_target);
  Serial.println("V"+String(prop_target));
}

void loop() {
  switch (state) {
    case Ready:
      if (Serial.available() > 0){
        String data = Serial.readString();
        switch (data.charAt(0)){
          case 'I':
            // Initializing + remeasure P_at
            atP_measuring = 1;
            digitalWrite(PUMP_PIN, LOW);
            digitalWrite(PROP_SOL_PIN, OPEN);
            digitalWrite(PUMP_SOL_PIN, OPEN);
            digitalWrite(P_VENT_PIN, OPEN);
            digitalWrite(C_VENT_PIN, OPEN);
            prop_int = 1023;
            prop_target = 100;
            analogWrite(PROPOR_PIN, prop_int);
            Serial.println("V"+String(prop_target));
            timerMillis = millis();
            state = MeasurePat_vent;
            break;
          case 'S':
            // Start button
            pCellTarget = data.substring(1).toFloat();
            digitalWrite(PUMP_PIN, HIGH);
            digitalWrite(PUMP_SOL_PIN, OPEN);
            timerMillis = millis();
            state = Pressurizing;
            break;
          case 'P':
            // Pump
            pResTarget = data.substring(1).toFloat();
            digitalWrite(PUMP_PIN, HIGH);
            digitalWrite(PUMP_SOL_PIN, OPEN);
            timerMillis = millis();
            state = Admin_P;
            break;
          case 'A':
            // Vent Pressure Reservoir
            digitalWrite(P_VENT_PIN, OPEN);
            digitalWrite(PUMP_SOL_PIN, OPEN);
            timerMillis = millis();
            state = Admin_A;
            break;
          case 'B':
            // Vent Cell Reservoir
            prop_int = 1023;
            prop_target = 100;
            analogWrite(PROPOR_PIN, prop_int);
            Serial.println("V"+String(prop_target));
            digitalWrite(C_VENT_PIN, OPEN);
            timerMillis = millis();
            state = Admin_B;
            break;
          case 'O':
            // Solenoid valve 4
            if (data.charAt(1) == 'O'){
            digitalWrite(PROP_SOL_PIN, OPEN);
            } else if (data.charAt(1) == 'X'){
            digitalWrite(PROP_SOL_PIN, CLOSE);
            }
            break;
          case 'V':
            // Proportional Valve
            prop_target = data.substring(1).toFloat();
            prop_int = prop_target*1023/100;
            analogWrite(PROPOR_PIN, prop_int);
            Serial.println("V"+String(prop_target));
            break;
          case 'T':
            // Update pResTarget and initialize pump and valves
            pResTarget = data.substring(1).toFloat();
            digitalWrite(PUMP_PIN, LOW);
            digitalWrite(PROP_SOL_PIN, CLOSE);
            digitalWrite(PUMP_SOL_PIN, CLOSE);
            digitalWrite(P_VENT_PIN, CLOSE);
            digitalWrite(C_VENT_PIN, CLOSE);
            prop_int = 0;
            prop_target = 0;
            analogWrite(PROPOR_PIN, prop_target);
            Serial.println("V"+String(prop_target));
            break;
        }
      }
      break;
    case MeasurePat_vent:
      if (accumulatedMillis > ventTimer) {
        atP = atP + cellP;
        digitalWrite(PROP_SOL_PIN, CLOSE);
        digitalWrite(PUMP_SOL_PIN, CLOSE);
        digitalWrite(P_VENT_PIN, CLOSE);
        digitalWrite(C_VENT_PIN, CLOSE);
        Serial.println("R"+String(atP));
        prop_int = 0;
        prop_target = 0;
        analogWrite(PROPOR_PIN, prop_int);
        Serial.println("V"+String(prop_target));
        atP_measuring = 0;
        state = Ready;
      }
      break;
    case Admin_P:
      if (accumulatedMillis > pumpMaxTimer || resP > pResTarget) {
        digitalWrite(PUMP_PIN, LOW);
        digitalWrite(PUMP_SOL_PIN, CLOSE);
        state = Ready;
      }
      break;
    case Admin_A:
      if (accumulatedMillis > ventTimer) {
        digitalWrite(P_VENT_PIN, CLOSE);
        digitalWrite(PUMP_SOL_PIN, CLOSE);
        state = Ready;
      }
      break;
    case Admin_B:
      if (accumulatedMillis > ventTimer) {
        prop_int = 0;
        prop_target = 0;
        analogWrite(PROPOR_PIN, prop_int);
        Serial.println("V"+String(prop_target));
        digitalWrite(C_VENT_PIN, CLOSE);
        state = Ready;
      }
      break;
    case Pressurizing:
      if (accumulatedMillis > pumpMaxTimer || resP > pResTarget + 100){
        digitalWrite(PUMP_PIN, LOW);
        digitalWrite(PUMP_SOL_PIN, CLOSE);
        digitalWrite(PROP_SOL_PIN, OPEN);
        Serial.println("S3");
        state = ProvidePressure;
        timerMillis = millis();
      }
      break;
    case ProvidePressure:
      if (accumulatedMillis > 500) {
        digitalWrite(PROP_SOL_PIN, CLOSE);
        prop_int_nxt = 440;
        state = Running;
        timerMillis = millis();
      }
    case Running:
      if (accumulatedMillis > runningTimer){
        Serial.println("S4");
        digitalWrite(PUMP_SOL_PIN, OPEN);
        digitalWrite(PROP_SOL_PIN, OPEN);
        digitalWrite(P_VENT_PIN, OPEN);
        digitalWrite(C_VENT_PIN, OPEN);
        prop_int = 1023;
        prop_target = 100;
        analogWrite(PROPOR_PIN, prop_int);
        Serial.println("V"+String(prop_target));
        timerMillis = millis();
        state = Finish;
      } else {
        // CALIBRATION NEEDED
        if (cellP < pCellTarget - 30) {
          prop_int_nxt = 500;
        } else if (cellP < pCellTarget) {
          prop_int_nxt = 440;
        } else {
          prop_int_nxt = 0;
        }
        if (prop_int_nxt != prop_int){
          prop_int = prop_int_nxt;
          prop_target = prop_int*100/1023;
          analogWrite(PROPOR_PIN, prop_int);
          Serial.println("V"+String(prop_target));
        }
      }
      break;
    case Finish:
      if (accumulatedMillis > ventTimer) {
        Serial.println("S0");
        digitalWrite(PROP_SOL_PIN, CLOSE);
        digitalWrite(PUMP_SOL_PIN, CLOSE);
        digitalWrite(P_VENT_PIN, CLOSE);
        digitalWrite(C_VENT_PIN, CLOSE);
        prop_int = 0;
        prop_target = 0;
        analogWrite(PROPOR_PIN, prop_int);
        Serial.println("V"+String(prop_target));
        state = Ready;
      }
      break;
  }

  // Gauge1
  Wire.requestFrom(GAUGE_ADDR, 2);
  if (Wire.available() > 0){
    raw_resP = Wire.read();
    raw_resP = raw_resP << 8;
    raw_resP |= Wire.read();
    resP = 0.316456*raw_resP-1590.23;
    resP = resP - atP;
  }  
  // Gauge2
  softwarei2c.requestFrom(GAUGE_ADDR, 2);
  if (softwarei2c.available() > 0){
    raw_cellP = softwarei2c.read();
    raw_cellP = raw_cellP << 8;
    raw_cellP |= softwarei2c.read();
    cellP = 0.316456*raw_cellP-1590.23;
    cellP = cellP - atP;
  }
  if (!atP_measuring){
    // Sensor values print
    Serial.println("P"+String(resP));
    Serial.println("C"+String(cellP));
  }

  // // Flow rate
  // if (SLF3X.readSample() == 0) {
  //   flowrate = SLF3X.getFlow();
    Serial.println("F"+String(flowrate));
  // }

  delay(100);
  
}