/////////LCD////////////////
// include the library code:
//http://wiki.sunfounder.cc/index.php?title=I2C_LCD2004
//copy and pasted from Big LCD test program
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);
//this uses I2C with pins 20 and 21 to control the LCD screen

const int pwm = 5; //make sure this is a valid pwm pin
const int dirControl = 4; //This value will need to change. This is a digital input used to control the motor direction
//Motor controller should be set to signed magnitude mode

////////Joystick////////
const int joystickX = A0; //pin value may change
const int joystickY = A1; //pin value may change
const int joystickButton = 11; //pin value may change

/////////Lid Hall Effect////////
const int lidPin = 30; // pin value may change

////////Tachometer Hall Effect////////
const int hallPin = 50;

////////Relays////////
const int RELAY_valveENABLE = 2;  // need to change this number upon our design
int valve_status;

const int RELAY_pumpENABLE = 13;  // need to change this number upon our design
int pump_status;

//Ultrasonic Sensor HC-SR04
const int trigPin = 9;// need to change this number upon our design
const int echoPin = 10;// need to change this number upon our design
long duration;
int distance;

////////Times////////
const int shortTime=5; //cycle times in minutes
const int medTime=10;
const int largeTime=15;


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  //set up the LCD screen
  lcd.init();
  lcd.clear();
  lcd.backlight();

  //set the joystick pins to the proper setting
  pinMode(joystickX,INPUT);
  pinMode(joystickY,INPUT);
  pinMode(joystickButton,INPUT);

  //Set up the relay pins
  pinMode(RELAY_valveENABLE, OUTPUT); //Need to add pins for water level sensor
  pinMode(RELAY_pumpENABLE,OUTPUT);
  digitalWrite(RELAY_valveENABLE, HIGH); //Need to add pins for water level sensor
  digitalWrite(RELAY_pumpENABLE,HIGH);
ww
  //Set up ultrasonic sensor pins
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  //Set up motor control pins
  pinMode(pwm, OUTPUT);
  pinMode(dirControl, OUTPUT);
  digitalWrite(dirControl, LOW);
}

int programState = 0; //used to track what state the program is in
int cursorLoc = 1; //used to track what line of the LCD screen the cursor is on

int xPos = -1; //x position of the joystick
int yPos = -1; //y position of the joystick

int progTime = -1; //the time that the program will run for, this may have to change in how it is handled in the code

int returnState = 0; //this is used to determine where to return to after the pause state

unsigned long startTime = 0; //used to contain the start time for the timer
unsigned long finalTime = 0; //used to contain the end time for the timer
unsigned long remainingTime = 0;

bool hasRinsed=false; //used to track if the program needs to run the rinsing cycle

unsigned long counter = 0; //used to track time for reversing the motor direction

char timeString[8]; //used to print the remaining time

float hall_thresh = 100.0; //used to track how many times the hall sensor for the tachometer has measured the magnet

unsigned long rpm = 0; //used to record the rpm for the PID controller

void loop() {
  // put your main code here, to run repeatedly:
  switch(programState)
  {
    //Draw start screen
    case 0:
      displaySelection(); //print the opening screen that lists all of the selection options
      programState=1;
      Serial.println("End of state 0");
      break;
    case 1:
      xPos=analogRead(joystickX); //not sure if this is actually used...
      yPos=analogRead(joystickY);
      if(digitalRead(joystickButton)==HIGH){
        if(cursorLoc==1){
          progTime=shortTime; //if first line is selected, the program will use the short time
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Short Time Selected");
          delay(1000);
        }
        else if(cursorLoc==2){
          progTime=medTime; //if second line is selected, the program will use the medium time
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Medium Time Selected");
          delay(1000);
        }
        else if(cursorLoc==3){
          progTime=largeTime; //if third line is selected, the program will use the long time
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Long Time Selected");
          delay(1000);
        }
        programState = 2;
        Serial.println("End of state 1");
        lcd.noCursor(); //hide cursor after selection is made
      }
      else{
        if(yPos == 1023){
          cursorLoc+=1; //may need to switch + to -, this moves the cursor up
          if(cursorLoc>3){
            cursorLoc=1; //check overflow
          }
        }
        else if(yPos == 0){
          cursorLoc-=1; //this moves the cursor down
          if(cursorLoc<1){
            cursorLoc=3; //check overflow
          }
        }
        lcd.setCursor(12,cursorLoc); //may need to change 12 to another number, display cursor at the end of the lines
        lcd.cursor(); //this displays the cursor on the screen
      }
      break;
    //Check door is closed
    case 2:
      //check that the door is closed
      if(!checkLid()){
        programState=3; //if checkLid is false, then the program will continue
        Serial.println("End of state 2");
      }
      else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Lid is not closed");
        delay(1000);
        programState=0; //if checkLid is true, then the program will go back to the selection state
      }
      break;
    //Open inlet valve
    case 3:
      openValve(); //This function should open the solenoid valve
      programState=4;
      Serial.println("End of state 3");
      break;
    //Check water level and close inlet valve
    case 4:
      if(digitalRead(joystickButton)==HIGH || checkLid())//if the button is pressed, the program should change to the pause state
      {
        while(digitalRead(joystickButton)==HIGH){}//wait until the button is released
        programState=12; //go to the pause state
        returnState=3; //once the pause is finished, return to the previous state to open the valve again
      }
      else
      {
        closeValve();
        if(CheckValve())
        {
         programState=5;
         Serial.println("End of state 4");
        }
      }
      break;
    //Run the motor for the washing cycle and display time
    case 5:
      if(digitalRead(joystickButton)==HIGH || checkLid())//if the button is pressed, the program should change to the pause state
      {
        while(digitalRead(joystickButton)==HIGH){}//wait until the button is released
        programState=12; //go to the pause state
        returnState=5; //once the pause is finished, return to this state
        remainingTime=finalTime-millis();
      }
      else
      {
        analogWrite(pwm,60);
        delay(2000);
        analogWrite(pwm,0);
        programState=6;
        Serial.println("End of state 5");
      }
      break;
    //Stop motor
    case 6:
      lcd.clear();
      analogWrite(pwm,0); //turn off the motor
      programState = 7;
      Serial.println("End of state 6");
      break;
    //Start drain pump
    case 7:
      turnONpump();
      programState=8;
      Serial.println("End of state 7");
      break;
    //Check water level
    //Stop drain pump
    case 8:
      if(digitalRead(joystickButton)==HIGH || checkLid())//if the button is pressed, the program should change to the pause state
      {
        while(digitalRead(joystickButton)==HIGH){}//wait until the button is released
        programState=12; //go to the pause state
        returnState=8; //once the pause is finished, return to the previous state to open the valve again
      }
      else
      {
        turnOFFpump();
        if(CheckPump())
        {
          if(hasRinsed)
          {
           programState=9;
           Serial.println("End of state 8 for second time");
          }
          else
          {
            programState=3;
            Serial.println("End of state 8 for first time");
            hasRinsed=true;
          }
        }
      }
      break;
    //Run motor at high speed
    case 9:
      if(digitalRead(joystickButton)==HIGH || checkLid())//if the button is pressed, the program should change to the pause state
      {
        while(digitalRead(joystickButton)==HIGH){}//wait until the button is released
        programState=12; //go to the pause state
        returnState=9; //once the pause is finished, return to this state
      }
      else
      {
        analogWrite(pwm,110);
        delay(2000);
        analogWrite(pwm,0);
        programState=10;
        Serial.println("End of state 9");
      }
      break;
    //Stop motor
    case 10:
      lcd.clear();
      analogWrite(pwm,0); //turn off the motor, check if this is safe
      
      programState=11;
      Serial.println("End of state 10");
      break;
    //Display end message, then reset
    case 11:
      lcd.clear();
      lcd.print("Cycle complete");
      while(digitalRead(joystickButton==LOW)){}
      programState=0;
      Serial.println("End of state 11");
      break;
    //Pause state - all devices should be turned off and a message should be displayed
    //pressing the pause button again should resume to the previous state
    case 12:
      analogWrite(pwm,0); //turn off the motor, this value may need to change
      closeValve();
      turnOFFpump();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Paused");
      while(digitalRead(joystickButton)==HIGH){} //this may not be necessary, right now it is in case the button is left pressed down when pausing
      while(digitalRead(joystickButton==LOW)){}
      if(remainingTime!=0)
      {
        finalTime = ((remainingTime*60UL)*1000) + millis(); //update final time to account for the pause that has occurred
        remainingTime = 0; //reset the remainingTime variable
      }
      programState=returnState;
      break;
    //default state used as an error state
    default:
      lcd.setCursor(0,0);
      lcd.print("Error please restart");
      delay(1000);
      programState=0;
      //Add all stopping functions
      break;
  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function used to draw the selection screen
void displaySelection(){
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
  lcd.setCursor(12,1);
}

//Print the remaining time to the LCD screen
void displayTime(unsigned long finalTime)
{
  lcd.setCursor(1,0);

  int minute = ((finalTime-millis())/1000/60)%60;
  int second = ((finalTime-millis())/1000)%60;
  sprintf(timeString, "%0.2d:%0.2d", minute, second);
  lcd.print(timeString);
}

bool checkLid(){
  if(digitalRead(lidPin)!=0){
    return true; //lid is open
  }
  else{
    return false; //lid is closed
  }
}


void openValve(){
  digitalWrite(RELAY_valveENABLE,HIGH); //open the valve for water inlet
  //we need to test the Relay ON or OFF for opening the valve, in some cases, the relay works reversly: LOW is on, HIGH is Off.
  Serial.println("opening  Valve");
}

void closeValve(){  
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) * 0.0344;
 
  if (distance >= 400 || distance <= 2){
    Serial.print("Distance = ");
    Serial.println("Out of range");
  }
  else {

    if(cursorLoc == 3){//if large load program selected, run following
      if(distance == 13.2){
        digitalWrite(RELAY_valveENABLE,LOW); //Close the valve to stop water inlet
          valve_status=digitalRead(RELAY_valveENABLE);
      }
    }
    
    else if(cursorLoc == 2){//if medium load program selected, run following
        if(distance == 17.2){
        digitalWrite(RELAY_valveENABLE,LOW); //Close the valve to stop water inlet
          valve_status=digitalRead(RELAY_valveENABLE);
        }
    }
    
    
    else if(cursorLoc == 1){//if small load program selected, run following
      if(distance == 21){
        digitalWrite(RELAY_valveENABLE,LOW); //Close the valve to stop water inlet
          valve_status=digitalRead(RELAY_valveENABLE);
      }
    }
  
  }
  //removed CheckValve() call and put it in the main loop 
}

//Check if the valve is closed
bool CheckValve(){
  if (valve_status == LOW){return true;}
  else {return false; }
  
}

//These functions may need to change slightly
void turnONpump() {
  Serial.println("ON");
  digitalWrite(RELAY_pumpENABLE,HIGH); //turn on the pump for water drain
  //we need to test the Relay ON or OFF for opening the valve, in some cases, the relay works reversly: LOW is on, HIGH is Off.

}

void turnOFFpump() {
 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) * 0.0344;
  
  if (distance >= 400 || distance <= 2){
    Serial.print("Distance = ");
    Serial.println("Out of range");
  }
  else {
    if(distance == 25){
       digitalWrite(RELAY_pumpENABLE,LOW); // turn off pump
       pump_status=digitalRead(RELAY_pumpENABLE);
    }
    //removed check pump statement and put it in main loop
  }
}

//Check if the pump is off
bool CheckPump(){
  if (pump_status == LOW){return true;}
  else {return false; }
  
}
