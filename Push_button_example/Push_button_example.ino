#define BUTTON_PIN 2
byte buttonState = LOW;

void buttonTog() {
  buttonState = !buttonState;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonTog, RISING);
}

void loop() {

  if (buttonState == LOW) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}