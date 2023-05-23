#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "sensirion-lf.h"
#include <movingAvg.h>   

const int MOTOR_PIN = 3;
const int DIR_PIN = 6;
const float TARGET_FLOW_RATE = 2;                     //mL per min
const float FLOW_RATE_TOL = TARGET_FLOW_RATE * 0.05;

int motorStepRate = 200;
int movingAveWindowLen = 10;
int runCounter = 0;

// delay between measurements
#define MEASURE_DELAY 100

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

movingAvg avgFlow(movingAveWindowLen);  


String animalName = "Test";
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  delay(MEASURE_DELAY);
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  delay(MEASURE_DELAY);
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  delay(MEASURE_DELAY);
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  delay(MEASURE_DELAY);
  digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
  delay(MEASURE_DELAY);
  Serial.begin(115200); // initialize serial communication

  SLF3X.init();
  avgFlow.begin();

  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);
  tone(MOTOR_PIN, motorStepRate);
  

  
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Animal ID:");
  lcd.setCursor(11,0);
  lcd.print(animalName);
  lcd.setCursor(0,1);
  lcd.print("Flow Rate:");
  lcd.setCursor(16,1);
  lcd.print("mL/m");
  lcd.setCursor(0,3);
  lcd.print("Run Time:");
  lcd.setCursor(0,2);
  lcd.print("Temperature:");
  lcd.setCursor(18,2);
  lcd.print((char)223);
  lcd.setCursor(19,2);
  lcd.print("C");
}
//double a = millis();
//int sec;
//int minutes;
//int hour;

int sec = 0;
int minute = 0;
int hour = 0;
// float spm = 60;
int i = 0;


String secStr;
String minStr;
String hourStr;

void running() {
  sec++;
  if (sec == 60) {
    minute ++;
    sec = 0;
  }

  if (minute == 60) {
    hour++;
    minute = 0;
  }
  ////////////////////////
  
  if (sec < 10) {
    secStr = "0" + String(sec);
  }
  else {
    secStr = String(sec);
  }
  ///////////////////////
  if (minute < 10) {
    minStr = "0" + String(minute);
  }
  else {
    minStr = String(minute);
  }
  ///////////////////////
  if (hour < 10) {
    hourStr = "0" + String(hour);
  }
  else {
    hourStr = String(hour);
  }
  
}




void loop() {
  int ret = SLF3X.readSample();
  if (ret == 0) {
    Serial.println(SLF3X.getFlow(), 2);
  } else {
    Serial.print("Error in SLF3X.readSample(): ");
    Serial.println(ret);
    SLF3X.init();
  }
  delay(MEASURE_DELAY); // delay between reads

 int flow_val_int = (int)(SLF3X.getFlow()*500);
 int average_flow = avgFlow.reading(flow_val_int);
 float average_flow_normalized = average_flow / 500.0;

 if (runCounter >= 0) {
  float flow_rate_error = average_flow_normalized - (TARGET_FLOW_RATE + FLOW_RATE_TOL);
  int deltaSign = flow_rate_error > 0 ? -1 : 1;
  
  if (fabs(flow_rate_error) > 2) {
    motorStepRate = motorStepRate + (deltaSign * 100);
    tone(MOTOR_PIN, motorStepRate);
  } else if (fabs(flow_rate_error) > 1 && fabs(flow_rate_error) <= 2) {
    motorStepRate = motorStepRate + deltaSign * (70 * fabs(flow_rate_error) - 40); 
    tone(MOTOR_PIN, motorStepRate);
  } else if (fabs(flow_rate_error) < 1 && fabs(flow_rate_error) > FLOW_RATE_TOL) {
    motorStepRate = motorStepRate + (deltaSign * 10);
    tone(MOTOR_PIN, motorStepRate);
  }
  

//  if (flow_rate_error < 0) {
//    motorS
//  }
 }

//  
//  if (flow_rate_error > 1) {
//    motorStepRate -= 70 * flow_rate_error - 40;
//  } else if (flow_rate_error < 1 && flow_rate_error > 0) {
//    motorStepRate -= 10;
//  } else if (flow_rate_error > 0

//  if (average_flow_normalized > TARGET_FLOW_RATE + FLOW_RATE_TOL) {
//    motorStepRate -= 100;
//    tone(MOTOR_PIN, motorStepRate);
//  }
//  else if (average_flow_normalized < TARGET_FLOW_RATE + FLOW_RATE_TOL) {
//    motorStepRate += 100;
//    tone(MOTOR_PIN, motorStepRate);
//  }
 
  
 lcd.setCursor(16, 3);
 lcd.print(secStr);
 lcd.setCursor(13, 3);
 lcd.print(minStr);
 lcd.setCursor(10, 3);
 lcd.print(hourStr);
 lcd.setCursor(12, 3);
 lcd.print(":");
 lcd.setCursor(15, 3);
 lcd.print(":");
 delay(880);

 lcd.setCursor(11, 1);
 //lcd.print(SLF3X.getFlow(), 2);
 lcd.print(average_flow_normalized);
 
 lcd.setCursor(13, 2);
 lcd.print(SLF3X.getTemp(), 1);

 running();
 runCounter++;
}
