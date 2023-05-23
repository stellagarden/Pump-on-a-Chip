// -----------------------------------------------------------
// FileName     : I2C_flowrate_example.ino
// Author       : Hyemin Stella Lee
// Created      : 05/18/2023
// Description  : Read flowrate from the sensor and print it as ml/min
//                via serial monitor in PC
// -----------------------------------------------------------

#include "sensirion-lf.h"
#include <movingAvg.h>

// comment / uncomment this to apply moving average on sensor value
#define APPLY_MOVING_AVG

#define MEASURE_DELAY       100
#define MOVING_AVG_WIN_LEN  10

float flowrate;
movingAvg avgFlow(MOVING_AVG_WIN_LEN);

void setup() {
  Serial.begin(9600);
  SLF3X.init();
  avgFlow.begin();
}

void loop() {
  int ret = SLF3X.readSample();
  if (ret == 0) {
#ifdef APPLY_MOVING_AVG
    int flow_val_int = (int)(SLF3X.getFlow()*500);
    int average_flow = avgFlow.reading(flow_val_int);
    flowrate = average_flow / 500.0;
#else
    flowrate = SLF3X.getFlow();
#endif
    Serial.print("Flow: ");
    Serial.print(flowrate, 2);
    Serial.println(" ml/min");
  } else {
    Serial.print("Error in SLF3X.readSample(): ");
    Serial.println(ret);
    SLF3X.init();
  }

  delay(MEASURE_DELAY); // delay between reads
}