// LCD: http://wiki.sunfounder.cc/index.php?title=I2C_LCD2004
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,PIN_WIRE_SDA,PIN_WIRE_SCL);

// Motor
const int PWM  = 5; //make sure this is a vaLID PWM  pin
const int directionControl = 4; 

// Joystick
const int JOYSTICK_X = A0; 
const int JOYSTICK_Y = A1;
const int JOYSTICK_PRESS = 11; 

// LID
const int LIDPin = 30; 

// Tachometer 
const int hallPin = 50;

// Solenoid Valve and Pump Relays
int valve_status;
int pump_status;
const int VALVE_IN_ENABLE = 2;
const int PUMP_OUT_ENABLE = 13;  

// Ultrasonic: Water Level, HC-SR04
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance;

// Time
const int shortTime=5; //cycle times in minutes
const int medTime=10;
const int largeTime=15;

//
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
