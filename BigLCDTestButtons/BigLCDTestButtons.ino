//Big LCD test program
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);
//this uses I2C with pins 20 and 21 to control the LCD screen

int cursorPos = 1;

int upButton = 13;
int downButton = 2;
int selectButton = 7;

void setup() {

  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  pinMode(selectButton, INPUT);
  
  //set up the LCD screen
  lcd.init();
  lcd.clear();
  lcd.backlight();

  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Choose a program");
  lcd.setCursor(0,1);
  lcd.print("Small load");
  lcd.setCursor(0,2);
  lcd.print("Medium load"); //set cursor location to 12
  lcd.setCursor(0,3);
  lcd.print("Large load");

  lcd.cursor(); //this displays the cursor on the screen
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(12,cursorPos); //display cursor at the end of the lines

  //buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (digitalRead(upButton) == HIGH) {
    //decrease the value of the cursor position, if less than 0, set to 3
    cursorPos = cursorPos-1;
    if(cursorPos < 0)
    {
      cursorPos = 3;
    }
  } 
  if (digitalRead(downButton) == HIGH) {
    //increase the value of the cursor position, if less than 0, set to 3
    cursorPos = cursorPos+1;
    if(cursorPos > 3)
    {
      cursorPos = 0;
    }
  } 
  if (digitalRead(selectButton) == HIGH) {
    //print a message to the screen, the clear and replace with original message
    
    if(cursorPos == 1)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Small load set");
      delay(3000);
    }
    if(cursorPos == 2)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Med load set");
      delay(3000);
    }
    if(cursorPos == 3)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Large load set");
      delay(3000);
    }
    if(cursorPos!=0)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Choose a program");
      lcd.setCursor(0,1);
      lcd.print("Small load");
      lcd.setCursor(0,2);
      lcd.print("Medium load"); //set cursor location to 12
      lcd.setCursor(0,3);
      lcd.print("Large load");
    }
  }
 
  delay(100); //small delay so cursor can be moved and user can react
}
