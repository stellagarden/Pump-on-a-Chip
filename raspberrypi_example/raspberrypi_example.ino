// -----------------------------------------------------------
// FileName     : raspberrypi_example.ino
// Author       : Hyemin Stella Lee
// Created      : 06/14/2023
// Description  : Communicate with Raspberry Pi using Serial
// -----------------------------------------------------------

const unsigned long sensorsTimer = 50;
unsigned long timerMillis;

#define accumulatedMillis millis() - timerMillis

float resP = 0.0;
float cellP = 0.0;
float flowrate = 0.0;
float prop = 0.0;
bool start_flag = false;

enum {Ready, Pressurizing, CellLoading, Running, Finish};
unsigned char state;

void setup() {
  Serial.begin(9600);
  state = Ready;
}

void loop() {
  // Status control
  switch (state) {
    case Ready:
      if (start_flag){
        state = Pressurizing;
        start_flag = false;
        timerMillis = millis();
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

  // Administrator Mode
  if (Serial.available() > 0){
        String data = Serial.readString();
        switch (data.charAt(0)){
          case 'S':
            // Start button
            if (data.charAt(1) == '0'){
              // Pressure input
              double pressure_target = data.substring(2).toDouble();
              Serial.println("*Set pressure target as "+String(pressure_target));
            } else {
              // Flow rate input
              double flowrate_target = data.substring(2).toDouble();
              Serial.println("*Set flow rate target as "+String(flowrate_target));
            }
            start_flag = true;
            break;
          case 'C':
            // Cell Loading Pinch valve
            Serial.println("*Open pinch valve");
            break;
          case 'P':
            // Pump
            Serial.println("*Run pump");
            break;
          case 'A':
            // Vent Pressure Reservoir
            Serial.println("*Open solenoid valve");
            break;
          case 'B':
            // Vent Cell Reservoir
            Serial.println("*Open solenoid valve");
            break;
          case 'V':
            // Proportional Valve
            double prop_target = data.substring(1).toDouble();
            // Set the proportional valve value
            Serial.println("*Set proportional valve as "+String(prop_target));
            break;
        }
  }

  // Sensor values print
  if (resP < 2500) resP += 100.67;
  else resP = 0;

  if (cellP < 300) cellP += 10.43;
  else cellP = 0;

  if (flowrate < 5) flowrate += 0.27;
  else flowrate = 0;

  if (prop < 100) prop += 6.49;
  else prop = 0;

  Serial.println("P"+String(resP));
  Serial.println("C"+String(cellP));
  Serial.println("F"+String(flowrate));
  Serial.println("V"+String(prop));
  
  delay(50);
}