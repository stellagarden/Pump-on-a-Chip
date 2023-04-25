// -----------------------------------------------------------
// FileName     : softwareI2C_read_address.ino
// Author       : Hyemin Stella Lee
// Created      : 04/24/2023
// Description  : Read address from softwareI2C device
// -----------------------------------------------------------

#include "SoftwareI2C.h"

SoftwareI2C softwarei2c;

void setup()
{
  softwarei2c.begin(2,3);       // sda, scl
  Serial.begin(115200);
}

void loop()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    softwarei2c.beginTransmission(address);
    error = softwarei2c.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");

      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");

      Serial.println(address,HEX);
    }
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("done");

  delay(5000); // wait 5 seconds for next scan
}
