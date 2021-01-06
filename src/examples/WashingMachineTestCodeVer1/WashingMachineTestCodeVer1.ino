//This is the preliminary coding design of the entire program.
//Specific sections of it should be tested individually before combining all functions

//////////PID///////////
#include <PID_v1.h>

//////////PID///////////
//Define variable 
double Setpoint = 0, Input = 0, Output = 0;
//Define the PID named myPID
PID myPID(&Input, &Output, &Setpoint, 1, 1, 0, DIRECT); //1,1,0 are the Kp, Ki, and Kd values (in that order)
const int pwm = 12; //make sure this is a valid pwm pin
const int dirControl = 40; //This value will need to change. This is a digital input used to control the motor direction
//Motor controller should be set to signed magnitude mode

//This PID code may need to change in the future

/////////LCD////////////////
// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; //this pin numbers will need to change
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

////////Joystick////////
const int joystickX = A0; //pin value may change
const int joystickY = A1; //pin value may change
const int joystickButton = 22; //pin value may change

/////////Lid Hall Effect////////
const int lidPin = 30; // pin value may change

////////Tachometer Hall Effect////////
const int hallPin = 50;

////////Relays////////
const int RELAY_valveENABLE = 2;  // need to change this number upon our design
int valve_status;

const int RELAY_pumpENABLE = 12;  // need to change this number upon our design
int pump_status;

//Ultrasonic Sensor HC-SR04
const int trigPin = 9;// need to change this number upon our design
const int echoPin = 10;// need to change this number upon our design
long duration;
int distance;

////////Times////////
const int shortTime=30; //cycle times in minutes
const int medTime=40;
const int largeTime=50;



void setup() {
  // put your setup code here, to run once:
  //set up the LCD screen
  lcd.begin(20,4);

  //set the joystick pins to the proper setting
  pinMode(joystickX,INPUT);
  pinMode(joystickY,INPUT);
  pinMode(joystickButton,INPUT);

  //Set up the relay pins
  pinMode(RELAY_valveENABLE, OUTPUT); //Need to add pins for water level sensor
  pinMode(RELAY_pumpENABLE,OUTPUT);

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
      break;
    //Options select with joystick
    case 1:
      xPos=analogRead(joystickX); //not sure if this is actually used...
      yPos=analogRead(joystickY);
      if(digitalRead(joystickButton)==HIGH){
        if(cursorLoc==1){
          progTime=shortTime; //if first line is selected, the program will use the short time
        }
        else if(cursorLoc==2){
          progTime=medTime; //if second line is selected, the program will use the medium time
        }
        else if(cursorLoc==3){
          progTime=largeTime; //if third line is selected, the program will use the long time
        }
        programState = 2;
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
        }
      }
      break;
    //Run the motor for the spinning cycle and display time
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
        if(finalTime == 0)
        {
          startTime = millis();
          finalTime = ((progTime*60UL)*1000) + startTime;
        }
        displayTime(finalTime); //display the time remaining for this cycle, this function may need tweeking to be displayed properly
        Setpoint = 60; //want our speed to be 60 rpm
        Input = getSpeed();
        myPID.Compute();
        analogWrite(pwm,Output);
        if(counter == 0)
        {
          counter = millis();
        }
        if(counter-millis() > 10000)
        {
          analogWrite(pwm,0); //need to check if stopping it like this is reasonable and safe
          Output = 0; //this may be unnecessary
          if(digitalRead(dirControl) == LOW)
            digitalWrite(dirControl,HIGH); //change the motor direction
          else if(digitalRead(dirControl) == HIGH)
            digitalWrite(dirControl,LOW); //change the motor direction

          counter = 0;
        }
        if(millis() > finalTime)
        {
          programState = 6; //If the required time has passed, move to the next state to stop the motor
          finalTime = 0; //Reset timer
        }
      }
      break;
    //Stop motor
    case 6:
      lcd.clear();
      analogWrite(pwm,0); //turn off the motor
      Output = 0;
      programState = 7;
      break;
    //Start drain pump
    case 7:
      turnONpump();
      programState=8;
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
          }
          else
          {
            programState=3;
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
        if(finalTime == 0)
        {
          startTime = millis();
          finalTime = ((progTime*60UL)*1000) + startTime;
        }
        displayTime(finalTime); //display the time remaining for this cycle, this function may need tweeking to be displayed properly
        Setpoint = 800; //want our speed to be 60 rpm
        Input = getSpeed();
        myPID.Compute();
        analogWrite(pwm,Output);
        //Does spinning require changes in direction?
        if(millis() > finalTime)
        {
          programState = 10; //If the required time has passed, move to the next state to stop the motor
        }
      }
      break;
    //Stop motor
    case 10:
      lcd.clear();
      analogWrite(pwm,0); //turn off the motor, check if this is safe
      Output = 0; //check if this is necessary
      programState=11;
      break;
    //Display end message, then reset
    case 11:
      lcd.clear();
      lcd.print("Cycle complete");
      while(digitalRead(joystickButton==LOW)){}
      programState=0;
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
      //Add all stopping functions
      break;
  }

  
}
///////////////////////////////////////////////////////

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
  lcd.setCursor(12,cursorPos);
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

//Used to get the motor speed. May need to be changed based on how accurate it is when testing
//Datatype may need to change, and threshold may need to change depending of how long it takes
double getSpeed()
{
  // preallocate values for tach
  double hall_count = 1.0;
  double start = micros();
  bool on_state = false;
  // counting number of times the hall sensor is tripped
  // but without double counting during the same trip
  while(true){
    if (digitalRead(hallPin)==0){
      if (on_state==false){
        on_state = true;
        hall_count+=1.0;
      }
    } 
    else{
      on_state = false;
    }
    
    if (hall_count>=hall_thresh){
      break; //maybe should change this
    }
  }
  
  //get information about Time and RPM
  double end_time = micros(); //data type may need to change to unsigned long
  double time_passed = ((end_time-start)/1000000.0);
  double rpm_val = (hall_count/time_passed)*60.0;
  return rpm_val;
}

//Double check this
void setPWM()
{
  Input = rpm;
  myPID.Compute();
  analogWrite(pwm, Output);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Cynthia's Code

//renamed some variables and changed formatting a bit
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
