//////////PID///////////
#include <PID_v1.h>
// PID
//#include "src/Arduino-PID-Library/PID_v1.h"
byte incomingByte;
double kp = 0.1;
double ki = 5;
double kd = 0.000005;

// Speed
double speedTarget;
double speedActual;
double pwmOutput;
PID myPID(&speedActual, &pwmOutput, &speedTarget, kp, ki, kd, DIRECT);
long currentTimestamp = 0;
long previousTimestamp = 0;
long timeElapsed = 0;

//arrays used to calculated moving average
double speeds[5] = {0,0,0,0,0};
double avgSpeed = 0;

//counters for arrays
int speedArrayCount=0;

//counting variable that will be used in the interrupt function
volatile int magnetCount = 0;

//tachometer hall effect pin number
const byte tacho_pin = 2;//50;

//PWM pin
const int PWM = 6;
const int DIR = 4;

//Speed setting
//int rpm = 60;

//time variables
unsigned long prevTime = 0;
unsigned long currTime = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  myPID.SetOutputLimits(-255, 255);
  myPID.SetMode(AUTOMATIC);
  
  pinMode(tacho_pin,INPUT);
  attachInterrupt(digitalPinToInterrupt(tacho_pin), rotation, FALLING);
  pinMode(PWM, OUTPUT);
  analogWrite(PWM,0);
  delay(1000);
  //analogWrite(PWM,rpm);
  pinMode(DIR, OUTPUT);
  digitalWrite(DIR,LOW);
  prevTime=micros();
  //Serial.println("End of Setup");
}

void loop() {
  // put your main code here, to run repeatedly:
  runMotorLoop();
  delay(800);
  //Serial.println("Loop");
}

void checkInput() {
  if (Serial.available() > 0) {
    String command = Serial.readString();
    command.replace(" ", "");
    if (command.indexOf("p") >= 0) {
      String textP = command.substring(command.indexOf("p") + 1, command.length());
      kp = textP.toDouble();
    }
    else if (command.indexOf("i") >= 0) {
      String textI = command.substring(command.indexOf("i") + 1, command.length());
      ki = textI.toDouble();
    }
    else if (command.indexOf("d") >= 0) {
      String textD = command.substring(command.indexOf("d") + 1, command.length());
      kd = textD.toDouble();
    }
    myPID.SetTunings(kp,ki,kd);
  }
}

/**
 * Adjusts motor speed depending on previous timestep
 */
void runMotorLoop() {
  // Check if user has changed  Kp, Ki, Kd
  checkInput();
  // Calculate time from start
  timeElapsed = timeElapsed + micros() - previousTimestamp;
  previousTimestamp = micros();
  // Set speed target based on random sinusoidal function
  speedTarget = 90; ///* sin(timeElapsed / 1000000);
  //speedTarget = log(timeElapsed / 1000000);
  // Simulate sensor as delayed PWM and random forcing
  speedActual = calcSpeed()/10.0;///pwmOutput * 0.9 + random(0, 20) - random(0, 20) + 0;
  if (myPID.Compute()) {
    Serial.print("Input:" + (String)speedTarget + ",");
    Serial.print("Actual:" + (String)speedActual + ",");
    Serial.print("PWM:" + (String)pwmOutput + ",");
    Serial.print("kP="+(String)kp+":0,");
    Serial.print("ki="+(String)ki+":0,");
    Serial.println("kd="+(String)kd+":0");
  }
  //Serial.println("running motor loop");
  analogWrite(PWM,pwmOutput);
}

double calcSpeed()
{
  currTime=micros();
  speeds[speedArrayCount]= (double)magnetCount/((currTime-prevTime)/1000000.0)*60.0;
  prevTime=currTime;
  magnetCount = 0;
  speedArrayCount = (speedArrayCount+1)%5;
  return (speeds[0]+speeds[1]+speeds[2]+speeds[3]+speeds[4])/5;
}

void rotation() {
  magnetCount++;
}
