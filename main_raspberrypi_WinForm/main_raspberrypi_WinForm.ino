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
#define PUMP_SOL_PIN 8
#define P_VENT_PIN 9
#define C_VENT_PIN 10
const unsigned long ventTimer = 7000;
const unsigned long runningTimer = 15000;

#define accumulatedMillis millis() - timerMillis
#define GAUGE_ADDR 56 // 0x38
#define OPEN HIGH
#define CLOSE LOW
unsigned long timerMillis;

SoftwareI2C softwarei2c;

float pResTarget = 3100.0;
float pCellTarget = 300.0;
uint16_t raw_resP = 0;
float resP = 3110;
uint16_t raw_cellP = 0;
float cellP = 302;
float atP = 1005;
float flowrate = 0.0;
int prop_target = 0;
int prop_int = 0;
int atP_measuring = 0;
unsigned char state;
enum {MeasurePat_vent, MeasurePat_vent_2s, Ready, Admin_C, Admin_P, Admin_A, Admin_A_2s, Admin_B, Admin_B_2s,
    Pressurizing, Running, Finish, Finish_2s};
int i = 2000;
int j = 1000;
int k = 0;

void setup() {
  Serial.begin(9600);
  state = Ready;
  
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(PUMP_SOL_PIN, OUTPUT);
  pinMode(P_VENT_PIN, OUTPUT);
  pinMode(C_VENT_PIN, OUTPUT);
  pinMode(PROPOR_PIN, OUTPUT);
  analogWriteResolution(10);
  // // Gauge1
  // Wire.begin();
  // Wire.beginTransmission(GAUGE_ADDR);
  // Wire.endTransmission();
  // // Gauge2
  // softwarei2c.begin(2,3);
  // softwarei2c.beginTransmission(GAUGE_ADDR);
  // softwarei2c.endTransmission();
  // // Flow rate
  // SLF3X.init();

  // Reset valves
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(PUMP_SOL_PIN, CLOSE);
  digitalWrite(P_VENT_PIN, CLOSE);
  digitalWrite(C_VENT_PIN, CLOSE);
  analogWrite(PROPOR_PIN, 0);
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
            digitalWrite(P_VENT_PIN, OPEN);
            digitalWrite(PUMP_SOL_PIN, OPEN);
            digitalWrite(C_VENT_PIN, OPEN);
            analogWrite(PROPOR_PIN, 0);
            Serial.println("V"+String(prop_target));
            timerMillis = millis();
            state = MeasurePat_vent;
            break;
          case 'S':
            // Start button
            pCellTarget = data.substring(1).toFloat();
            digitalWrite(PUMP_PIN, HIGH);
            digitalWrite(PUMP_SOL_PIN, OPEN);
            state = Pressurizing;
            break;
          case 'P':
            // Pump
            pResTarget = data.substring(1).toFloat();
            digitalWrite(PUMP_PIN, HIGH);
            digitalWrite(PUMP_SOL_PIN, OPEN);
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
            digitalWrite(C_VENT_PIN, OPEN);
            timerMillis = millis();
            state = Admin_B;
            break;
          case 'V':
            // Proportional Valve
            prop_target = data.substring(1).toFloat();
            prop_int = prop_target*1023/100;
            analogWrite(PROPOR_PIN, prop_int);
            Serial.println("V"+String(prop_target));
            Serial.println("*Set prop valve as "+String(prop_int));
            break;
          case 'T':
            // Update pResTarget
            pResTarget = data.substring(1).toFloat();
            break;
        }
      }
      break;
    case MeasurePat_vent:
      if (accumulatedMillis > ventTimer - 2.0 || resP < atP) {
        timerMillis = millis();
        state = MeasurePat_vent_2s;
      }
      break;
    case MeasurePat_vent_2s:
      if (accumulatedMillis > 2) {
        atP = cellP;
        digitalWrite(P_VENT_PIN, CLOSE);
        digitalWrite(PUMP_SOL_PIN, CLOSE);
        digitalWrite(C_VENT_PIN, CLOSE);
        Serial.println("R"+String(atP));
        atP_measuring = 0;
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
      if (accumulatedMillis > ventTimer - 2.0 || resP < atP) {
        timerMillis = millis();
        state = Admin_A_2s;
      }
      break;
    case Admin_A_2s:
      if (accumulatedMillis > 2) {
        digitalWrite(P_VENT_PIN, CLOSE);
        digitalWrite(PUMP_SOL_PIN, CLOSE);
        state = Ready;
      }
    case Admin_B:
      if (accumulatedMillis > ventTimer - 2.0 || cellP < atP) {
        timerMillis = millis();
        state = Admin_B_2s;
      }
      break;
    case Admin_B_2s:
      if (accumulatedMillis > 2) {
        digitalWrite(C_VENT_PIN, CLOSE);
        state = Ready;
      }
      break;
    case Pressurizing:
      if (resP > pResTarget){
        digitalWrite(PUMP_PIN, LOW);
        digitalWrite(PUMP_SOL_PIN, CLOSE);
        Serial.println("S3");
        state = Running;
        timerMillis = millis();
      }
      break;
    case Running:
      if (accumulatedMillis > runningTimer){
        Serial.println("S4");
        digitalWrite(P_VENT_PIN, OPEN);
        digitalWrite(PUMP_SOL_PIN, OPEN);
        digitalWrite(C_VENT_PIN, OPEN);
        state = Finish;
        timerMillis = millis();
      } else {
        int prop_int_nxt = 0;
        // CALIBRATION NEEDED
        if (cellP - atP < pCellTarget - 200) {
          prop_int_nxt = 590;
        } else if (cellP - atP < pCellTarget - 30) {
          prop_int_nxt = 490;
        } else {
          prop_int_nxt = 0;
        }
        if (prop_int_nxt != prop_int){
          prop_int = prop_int_nxt;
          prop_target = prop_int*100/1023;
          analogWrite(PROPOR_PIN, prop_int);
          Serial.println("V"+String(prop_target));
          Serial.println("*Set prop valve as "+String(prop_int_nxt));
        }
      }
      break;
    case Finish:
      if (accumulatedMillis > ventTimer - 2.0 || resP < atP){
        timerMillis = millis();
        state = Finish_2s;
      }
      break;
    case Finish_2s:
      if (accumulatedMillis > 2) {
        Serial.println("S0");
        digitalWrite(P_VENT_PIN, CLOSE);
        digitalWrite(PUMP_SOL_PIN, CLOSE);
        digitalWrite(C_VENT_PIN, CLOSE);
        state = Ready;
      }
      break;
  }

  if (!atP_measuring){
    // // Sensor values print
    // // Gauge1
    // Wire.requestFrom(GAUGE_ADDR, 2);
    // if (Wire.available() > 0){
    //   raw_resP = Wire.read();
    //   raw_resP = raw_resP << 8;
    //   raw_resP |= Wire.read();
    //   resP = 0.316456*raw_resP-1590.23;
    //   Serial.println("P"+String(resP));
    // }  
    if (i > 4000){
      i = 2000;
    } else {
      i = i+1;
    }
    resP = i - atP;
    Serial.println("P"+String(resP));

    // // Gauge2
    // softwarei2c.requestFrom(GAUGE_ADDR, 2);
    // if (softwarei2c.available() > 0){
    //   raw_cellP = softwarei2c.read();
    //   raw_cellP = raw_cellP << 8;
    //   raw_cellP |= softwarei2c.read();
    //   cellP = 0.316456*raw_cellP-1590.23;
    //   Serial.println("C"+String(cellP));
    // }
    if (j > 2000){
      j = 1000;
    } else {
      j = j+1;
    }
    cellP = j - atP;
    Serial.println("C"+String(cellP));
  } else {

  }

  // // Flow rate
  // if (SLF3X.readSample() == 0) {
  //   flowrate = SLF3X.getFlow();
  //   Serial.println("F"+String(flowrate));
  // }  
  if (k > 10){
    k = 0;
  } else {
    k = k+1;
  }
  Serial.println("F"+String(k));

  delay(100);
  
}