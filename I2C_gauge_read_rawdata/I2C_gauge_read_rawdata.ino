#include <Wire.h>

int address_sensor= 56; // 0x38
int readVal = 0;
uint8_t readByte = 0;

void setup(){
  Serial.begin(9600);
  Wire.begin();

  Wire.beginTransmission(address_sensor); //Send a request to begin communication with the device at the specified address
  Wire.endTransmission();
}

void loop(){
  
  Wire.requestFrom(address_sensor, 10);// requests 1 byte from the specified address

  Serial.print("Available: ");
  Serial.println(Wire.available());
  // if (Wire.available() > 0){
    readByte = Wire.read();    // Receive a byte as character
    Serial.print("Read1: ");
    Serial.println(readByte);         // Print the character
    readByte = Wire.read();    // Receive a byte as character
    Serial.print("Read2: ");
    Serial.println(readByte);         // Print the character
    readByte = Wire.read();    // Receive a byte as character
    Serial.print("Read3: ");
    Serial.println(readByte);         // Print the character
    readByte = Wire.read();    // Receive a byte as character
    Serial.print("Read4: ");
    Serial.println(readByte);         // Print the character
    readByte = Wire.read();    // Receive a byte as character
    Serial.print("Read5: ");
    Serial.println(readByte);         // Print the character
    readByte = Wire.read();    // Receive a byte as character
    Serial.print("Read6: ");
    Serial.println(readByte);         // Print the character
    readByte = Wire.read();    // Receive a byte as character
    Serial.print("Read7: ");
    Serial.println(readByte);         // Print the character
    readByte = Wire.read();    // Receive a byte as character
    Serial.print("Read8: ");
    Serial.println(readByte);         // Print the character
    readByte = Wire.read();    // Receive a byte as character
    Serial.print("Read9: ");
    Serial.println(readByte);         // Print the character
    readByte = Wire.read();    // Receive a byte as character
    Serial.print("Read10: ");
    Serial.println(readByte);         // Print the character
    
  // }  

  delay(100);
}