// LCD: http://wiki.sunfounder.cc/index.php?title=I2C_LCD2004
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, PIN_WIRE_SDA, PIN_WIRE_SCL);
int currentCursorLine = 1; //used to track what line of the LCD screen the cursor is on

// Motor
const int PWM  = 5; //make sure this is a vaLID PWM  pin
const int directionControl = 4;

// Joystick
const int JOYSTICK_X = A0;
const int JOYSTICK_Y = A1;
const int JOYSTICK_PRESS = 11;
int xPos = -1; //x position of the joystick
int yPos = -1; //y position of the joystick

// LID
const int LID = 30;

// Tachometer
const int TACHOMETER = 50;
unsigned long RPM = 0;
unsigned long tachometerRotationCount = 0; //used to track time for reversing the motor direction
char remainingTime[8]; //used to print the remaining time
float hall_thresh = 100.0; //used to track how many times the hall sensor for the tachometer has measured the magnet

// Solenoid Valve and Pump Relays
const int VALVE_IN_ENABLE = 2;
const int PUMP_OUT_ENABLE = 13;
int valveStatus;
int pumpStatus;

// Ultrasonic: Water Level, HC-SR04
const int ULTRASONIC_TRIGGER_PIN = 9;
const int ULTRASONIC_ECHO_PIN = 10;
long duration;
int distance;
bool isRinsed = false;

// Cycle States & Timing
int programState = 0;
const int SMALL_CYCLE = 1;
const int MEDIUM_CYCLE = 2;
const int LARGE_CYCLE = 3;
const int SMALL_CYCLE_TIME = 5; //cycle times in minutes
const int MEDIUM_TIME = 10;
const int LARGE_CYCLE_TIME = 15;
int cycleRunTime = -1; //the time that the program will run for, this may have to change in how it is handled in the code
int lastSavedState = 0; //this is used to determine where to return to after the pause state
unsigned long startTimerTime = 0;
unsigned long endTimerTime = 0;
unsigned long remainingTimerTime = 0;
