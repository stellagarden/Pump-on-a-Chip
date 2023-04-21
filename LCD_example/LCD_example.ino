// -----------------------------------------------------------
// FileName     : LCD_example.ino
// Author       : Hyemin Stella Lee
// Created      : 04/20/2023
// Description  : Print "Hello World" on LCD screen
// -----------------------------------------------------------

#include <SPI.h>

#include "Goldelox_Serial_4DLib.h"
#include "Goldelox_Const4D.h"

#define DisplaySerial Serial

Goldelox_Serial_4DLib Display(&DisplaySerial);

void setup() {
  DisplaySerial.begin(9600);
  Serial.begin(9600); // Start the serial communication at 9600 baud
  SLCD.begin(); // Initialize the uLCD module
  SLCD.clear(); // Clear the screen
}

void loop() {
  SLCD.gfx_Cls(); // Clear the graphics buffer
  SLCD.gfx_RectangleFilled(10, 10, 50, 50, SLCD.RGB(255, 0, 0)); // Draw a red rectangle
  SLCD.gfx_CircleFilled(100, 50, 25, SLCD.RGB(0, 255, 0)); // Draw a green circle
  SLCD.gfx_Text(10, 100, "Hello World!", SLCD.RGB(0, 0, 255), 2); // Draw blue text
  SLCD.gfx_Update(); // Update the display with the graphics buffer
  delay(1000); // Delay for one second
}
