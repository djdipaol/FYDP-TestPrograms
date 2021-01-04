//Motor button control test
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);
//this uses I2C with pins 20 and 21 to control the LCD screen

//button stuff
int upButton = 13;
int downButton = 2;
//int selectButton = 7;

int speedPin = 5;
int dirPin = 4;

int value = 0;

void setup() {
  //pinMode(speedPin, OUTPUT); this may be causing the speed error at the start
  pinMode(dirPin, OUTPUT);

  analogWrite(speedPin, 0);
  digitalWrite(dirPin,0);

  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  //pinMode(selectButton, INPUT);
  
  //set up the LCD screen
  lcd.init();
  lcd.clear();
  lcd.backlight();

  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(value);
}

void loop() {
  if (digitalRead(upButton) == HIGH) {
    //increase the speed value
    value=value+1;
    if(value > 255)
    {
      value = 255;
    }
    analogWrite(speedPin, value);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(value);
    delay(800);
  } 
  if (digitalRead(downButton) == HIGH) {
    //decrease the speed value
    value=value-1;
    if(value < 0)
    {
      value = 0;
    }
    analogWrite(speedPin, value);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(value);
    delay(800);
  }
  
}
