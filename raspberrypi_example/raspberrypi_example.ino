// -----------------------------------------------------------
// FileName     : raspberrypi_example.ino
// Author       : Hyemin Stella Lee
// Created      : 06/14/2023
// Description  : Communicate with Raspberry Pi using Serial
// -----------------------------------------------------------

float resP = 0.0;
float cellP = 0.0;
float flowrate = 0.0;
float prop = 0.0;
unsigned long previousMillis = 0;
const long interval = 1000;
String data;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();

  // if (resP < 2500) resP += 100.67;
  // else resP = 0;

  // if (cellP < 300) cellP += 10.43;
  // else cellP = 0;

  // if (flowrate < 5) flowrate += 0.27;
  // else flowrate = 0;

  // if (prop < 100) prop += 6.49;
  // else prop = 0;

  // Serial.println("P"+String(resP));
  // Serial.println("C"+String(cellP));
  // Serial.println("F"+String(flowrate));
  // Serial.println("V"+String(prop));
  
  delay(50);

  if (Serial.available() > 0){
    data = Serial.readString();
    switch (data.charAt(0)){
      case 'S':
        if (data.charAt(1) == '0') {
          // Start with Pressure
          Serial.println("S2");
        }
        else {
          // Start with Flow rate
          Serial.println("S2");
        }
        break;
      case 'C':
        Serial.println("C received");
        break;
    }
    // Serial.println(data);
  }

}