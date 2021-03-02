// PID
//#include "src/Arduino-PID-Library/PID_v1.h"
#include <PID_v1.h>
byte incomingByte;
double kp = 0.4;
double ki = 3;
double kd = 0;

// Speed
double speedTarget = 60;
double speedActual;
double pwmOutput;
PID myPID(&speedActual, &pwmOutput, &speedTarget, kp, ki, kd, DIRECT);
long currentTimestamp = 0;
long previousTimestamp = 0;
long timeElapsed = 0;

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
const int LID = 7;

// Tachometer
const int TACHOMETER = 2; //interrupt pins for mega are 2,3,18,19,20,21
unsigned long RPM = 0;
unsigned long tachometerRotationCount = 0; //used to track time for reversing the motor direction
char remainingTime[8]; //used to print the remaining time
float hall_thresh = 100.0; //used to track how many times the hall sensor for the tachometer has measured the magnet

//Tachometer interrupt implementation
volatile int magnetCount = 0; //used to count the times the magnet passes the Hall effect sensor
double speeds[5] = {0,0,0,0,0}; //array used to calculated moving average
int speedArrayCount=0; //counters for arrays
unsigned long prevTime = 0; //time variable for speed calc
unsigned long currTime = 0; //time variable for speed calc

// Solenoid Valve and Pump Relays
const int VALVE_IN_ENABLE = 2;
const int PUMP_OUT_ENABLE = 13;
int valveStatus;
int pumpStatus;

// Ultrasonic: Water Level, HC-SR04
const int ULTRASONIC_TRIGGER_PIN = 9;
const int ULTRASONIC_ECHO_PIN = 10;
float duration;
float distance;
float initialDist = -1; //used to check if the water level changes
float emptyLevel = 35; //this is the distance of the drum when there is not water
float minMeasurement = 10; //the measure level should not fall below this value, if it does there is an issue
bool isRinsed = false;
char levelString[8];
float levelValue = 27;
unsigned long drainTimer = 0;

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

int failureCode = 0; //int used for identifying different failure states
int stoppedCount = 0; //used to check that the motor starts to move
int overshootCount = 0; //used to check that the motor eventually settles to within a tolerance
int speedTol = 30; //this value will define the allowable speed tolerance in rpm
unsigned long caseStartTime = 0; //used to track when each case starts, used for failure state
