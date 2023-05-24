// -----------------------------------------------------------
// FileName     : main_gauges_uLCD_propValve_flow.ino
// Author       : Hyemin Stella Lee
// Created      : 04/24/2023
// Description  : Prints gauges' value and flow rate on uLCD
//                while controlling proportional valve
// -----------------------------------------------------------

#include <Wire.h>
#include <SoftwareI2C.h>
#include "Goldelox_Serial_4DLib.h"
#include "Goldelox_Const4D.h"
#include "sensirion-lf.h"
#include <movingAvg.h>

// Gauge1
#define GAUGE_ADDR 56 // 0x38
uint16_t rawdata1 = 0;
float pressure1 = 0;
// Gauge2
SoftwareI2C softwarei2c;
uint16_t rawdata2 = 0;
float pressure2 = 0;

// uLCD
#define DisplaySerial Serial1
#define LCD_Reset 8
Goldelox_Serial_4DLib Display(&DisplaySerial);

// Potentiometer
#define POT_PIN 10

// Proportional Valve
#define PROPOR_PIN A0

// Flow rate
// #define APPLY_MOVING_AVG    // comment / uncomment this to apply moving average on sensor value
#define MEASURE_DELAY       100
#define MOVING_AVG_WIN_LEN  10
float flowrate;
movingAvg avgFlow(MOVING_AVG_WIN_LEN);


void LCD_print1(float p1) {
  Display.txt_MoveCursor(1,0) ;
  Display.print(p1,2);
  Display.print("   ");
}

void LCD_print2(float p2) {
  Display.txt_MoveCursor(3,0) ;
  Display.print(p2,2);
  Display.print("   ");
}

void LCD_print3(float p3) {
  Display.txt_MoveCursor(5,0) ;
  Display.print(p3,2);
  Display.print("   ");
}


void setup() {
  // Gauge1
  Wire.begin();
  Wire.beginTransmission(GAUGE_ADDR); //Send a request to begin communication with the device at the specified address
  Wire.endTransmission();
  // Gauge2
  softwarei2c.begin(2,3);
  softwarei2c.beginTransmission(GAUGE_ADDR); //Send a request to begin communication with the device at the specified address
  softwarei2c.endTransmission();

  // uLCD
  Serial1.begin(9600);
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
  Display.putstr("Pump") ;
  Display.putstr("\n\nChip");
  Display.putstr("\n\nFlowrate");

  // Potentiometer
  pinMode(POT_PIN,INPUT);

  // Proportional Valve
  analogWriteResolution(10);

  // Flow rate
  SLF3X.init();
  avgFlow.begin();
}

void loop() {
  // Gauge1
  Wire.requestFrom(GAUGE_ADDR, 2);  // requests 2 bytes from the specified address
  if (Wire.available() > 0){
    rawdata1 = Wire.read();
    rawdata1 = rawdata1 << 8;
    rawdata1 |= Wire.read();
    pressure1 = 0.316456*rawdata1-1590.23;  // Convert raw data to actual pressure
    LCD_print1(pressure1);
  }  

  // Gauge2
  softwarei2c.requestFrom(GAUGE_ADDR, 2);  // requests 2 bytes from the specified address
  if (softwarei2c.available() > 0){
    rawdata2 = softwarei2c.read();
    rawdata2 = rawdata2 << 8;
    rawdata2 |= softwarei2c.read();
    pressure2 = 0.316456*rawdata2-1590.23;  // Convert raw data to actual pressure
    LCD_print2(pressure2);
  }

  // Proportional Valve
  analogWrite(PROPOR_PIN, analogRead(POT_PIN));
  Display.txt_MoveCursor(8,0) ;
  Display.print(analogRead(POT_PIN));

  // Flow rate
  if (SLF3X.readSample() == 0) {
#ifdef APPLY_MOVING_AVG
    int flow_val_int = (int)(SLF3X.getFlow()*500);
    int average_flow = avgFlow.reading(flow_val_int);
    flowrate = average_flow / 500.0;
#else
    flowrate = SLF3X.getFlow();
#endif
    LCD_print3(flowrate);
    // LCD_print3(rawdata1);
  }

  delay(MEASURE_DELAY);
}