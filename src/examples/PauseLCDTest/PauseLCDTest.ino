/*
  LiquidCrystal Library - display() and noDisplay()

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD and uses the
 display() and noDisplay() functions to turn on and off
 the display.

 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalDisplay

*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int buttonPin=8;


void setup() {
  //Serial.begin(9600);
  pinMode(buttonPin,INPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  delay(5000);
  lcd.clear();
}

int timemin = 35;
int timesec = 0;

unsigned long startTime = millis();
unsigned long finalTime = ((timemin*60UL+timesec)*1000) + startTime;
//UL is to make sure it performs arithmetic using longs so that the value does not overflow.

unsigned long remainingTime;

char timeString[8];

int paused = -1;

//general idea is working, will need to change in the final program.
void loop() {
  while(finalTime>millis()){
    if(digitalRead(buttonPin)==HIGH)
    {
      paused=paused*-1;
      //while(digitalRead(buttonPin)==HIGH){}
      if(paused == 1)
      {
        remainingTime = finalTime-millis();
      }
      else
      {
        finalTime=remainingTime+millis();
        lcd.clear();
      }
        
    }
    if(paused<0)
    {
      lcd.setCursor(0,0);

      int minute = ((finalTime-millis())/1000/60)%60;
      int second = ((finalTime-millis())/1000)%60;
      sprintf(timeString, "%0.2d:%0.2d", minute, second);
      lcd.print(timeString);
    }
    else
    {
      lcd.setCursor(0,0);
      lcd.print("Paused");
    }
    
    
  }
  lcd.clear();
  lcd.print("Finished");
  delay(1000);
  
}
