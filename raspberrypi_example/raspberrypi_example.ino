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

void setup() {
  Serial.begin(9600);
}

void loop() {
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
  
  delay(500);
}