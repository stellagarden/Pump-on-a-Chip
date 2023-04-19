#include <Wire.h>

int address_sensor= 56; // 0x38
uint16_t rawdata = 0;
float pressure = 0;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  Wire.begin();

  Wire.beginTransmission(address_sensor); //Send a request to begin communication with the device at the specified address
  Wire.endTransmission();
}

void loop(){
  
  Wire.requestFrom(address_sensor, 2);// requests 1 byte from the specified address

  if (Wire.available() > 0){
    rawdata = Wire.read();
    rawdata = rawdata << 8;
    rawdata |= Wire.read();
    // Convert rawdata to actual pressure
    pressure = 0.316456*rawdata-1580.74;
    Serial.println(pressure);
    Serial1.println(pressure);
  }  

  delay(100);
}