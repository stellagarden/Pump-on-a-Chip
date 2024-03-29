// -----------------------------------------------------------
// FileName     : cell_sampling.ino
// Author       : Hyemin Stella Lee
// Created      : 07/28/2023
// Description  : Control the cell sampling system
// -----------------------------------------------------------

#define PUMP2_PIN 0
#define PINCH1_PIN 1
#define PINCH2_PIN 2
#define PINCH3_PIN 3
const unsigned long cellLoadTimer = 10000;
const unsigned long discardTimer = 3000;

#define accumulatedMillis millis() - timerMillis
#define OPEN HIGH
#define CLOSE LOW
unsigned long timerMillis;

unsigned char state;
enum {Ready, Draw, Discard_cellwaste, Discard_cell};

void setup() {
  Serial.begin(9600);
  state = Ready;

  pinMode(PUMP2_PIN, OUTPUT);
  pinMode(PINCH1_PIN, OUTPUT);
  pinMode(PINCH2_PIN, OUTPUT);
  pinMode(PINCH3_PIN, OUTPUT);

  digitalWrite(PUMP2_PIN, LOW);
  digitalWrite(PINCH1_PIN, CLOSE);
  digitalWrite(PINCH2_PIN, CLOSE);
  digitalWrite(PINCH3_PIN, CLOSE);
}

void loop() {
  switch (state) {
    case Ready:
      if (Serial.available() > 0){
        String data = Serial.readString();
        switch (data.charAt(0)){
          case 'S':
            // Start button
            digitalWrite(PINCH1_PIN, OPEN);
            digitalWrite(PINCH2_PIN, CLOSE);
            digitalWrite(PINCH3_PIN, OPEN);
            digitalWrite(PUMP2_PIN, HIGH);
            timerMillis = millis();
            state = Draw;
            break;
          case 'U':
            // Change to user mode = Initialize all the pump and valves
            digitalWrite(PUMP2_PIN, LOW);
            digitalWrite(PINCH1_PIN, CLOSE);
            digitalWrite(PINCH2_PIN, CLOSE);
            digitalWrite(PINCH3_PIN, CLOSE);
          case 'P':
            if (data.charAt(1) == 'O'){
              digitalWrite(PUMP2_PIN, HIGH);
            }
            else if (data.charAt(1) == 'X'){
              digitalWrite(PUMP2_PIN, LOW);
            }
            break;
          case '1':
            if (data.charAt(1) == 'O'){
              digitalWrite(PINCH1_PIN, HIGH);
            }
            else if (data.charAt(1) == 'X'){
              digitalWrite(PINCH1_PIN, LOW);
            }
            break;
          case '2':
            if (data.charAt(1) == 'O'){
              digitalWrite(PINCH2_PIN, HIGH);
            }
            else if (data.charAt(1) == 'X'){
              digitalWrite(PINCH2_PIN, LOW);
            }
            break;
          case '3':
            if (data.charAt(1) == 'O'){
              digitalWrite(PINCH3_PIN, HIGH);
            }
            else if (data.charAt(1) == 'X'){
              digitalWrite(PINCH3_PIN, LOW);
            }
            break;
        }
      }
      break;
    case Draw:
      if (accumulatedMillis > cellLoadTimer) {
        digitalWrite(PINCH1_PIN, CLOSE);
        digitalWrite(PINCH3_PIN, CLOSE);
        digitalWrite(PINCH2_PIN, OPEN);
        timerMillis = millis();
        state = Discard_cellwaste;
      }
      break;
    case Discard_cellwaste:
      if (accumulatedMillis > discardTimer) {
        digitalWrite(PINCH3_PIN, OPEN);
        timerMillis = millis();
        state = Discard_cell;
      }
      break;
    case Discard_cell:
      if (accumulatedMillis > discardTimer) {
        digitalWrite(PINCH2_PIN, CLOSE);
        digitalWrite(PINCH3_PIN, CLOSE);
        digitalWrite(PUMP2_PIN, LOW);
        Serial.println("D");
        state = Ready;
      }
      break;
  }

  delay(10);
}
