#include "Goldelox_Serial_4DLib.h"
#include "Goldelox_Const4D.h"

#define DisplaySerial Serial1
#define LCD_Reset 8

Goldelox_Serial_4DLib Display(&DisplaySerial);

void LCD_print(float a, float b) {
  Display.txt_MoveCursor(2,0) ;
  Display.print(a);
  // Display.putstr(" mbar   ");
  Display.txt_MoveCursor(6,0) ;
  Display.print(b);
  // Display.putstr(" mbar   ");
}

void setup() {
  pinMode(LCD_Reset, OUTPUT);
  digitalWrite(LCD_Reset, 0);
  delay(100);
  digitalWrite(LCD_Reset, 1);
  delay(5000);
  Display.TimeLimit4D = 5000 ; // 5 second timeout on all commands
  //Display.Callback4D = Callback ; // NULL ;
  DisplaySerial.begin(9600);
  Display.txt_FGcolour(WHITE);
  Display.gfx_Cls() ;
  Display.txt_Height(2) ;
  Display.txt_Width(2) ;
  Display.putstr("L:") ;
  Display.putstr("\n\n\n\nR:");
}

void loop() {
  // put your main code here, to run repeatedly:
  static float x = 1000.0;
  x += 0.1;
  LCD_print(x, 2000.0-x);
}
