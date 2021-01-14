#include "src/Arduino-PID-Library/PID_v1.h"

// Tuning Constants
byte incomingByte;
double kp = 0.1;
double ki = 5;
double kd = 0.000005;

// Speed
double speedTarget;
double speedActual;
double pwmOutput;

// Time
long currentTimestamp = 0;
long previousTimestamp = 0;
long timeElapsed = 0;

PID myPID(&speedActual, &pwmOutput, &speedTarget, kp, ki, kd, DIRECT);

void setup() {
  Serial.begin(115200);
  myPID.SetOutputLimits(-255, 255);
  myPID.SetMode(AUTOMATIC);
  previousTimestamp = micros();
}

void loop() {
  // Check if user has changed  Kp, Ki, Kd
  checkInput();
  
  // Calculate time from start
  timeElapsed = timeElapsed + micros() - previousTimestamp;
  previousTimestamp = micros();
  
  // Set speed target based on random sinusoidal function
  speedTarget = 50 * sin(timeElapsed / 1000000);
  //speedTarget = log(timeElapsed / 1000000);

  // Simulate sensor as delayed PWM and random forcing
  speedActual = pwmOutput * 0.9 + random(0, 20) - random(0, 20) + 0;

  if (myPID.Compute()) {
    Serial.print("Input:" + (String)speedTarget + ",");
    Serial.print("Actual:" + (String)speedActual + ",");
    Serial.print("PWM:" + (String)pwmOutput + ",");
    Serial.print("kP="+(String)kp+":0,");
    Serial.print("ki="+(String)ki+":0,");
    Serial.println("kd="+(String)kd+":0");
  }
}

/**
 * Checks serial input stream for user input.  Will set kp, ki, kd according to user.
 * Use Cases:
 * Example 1: "p 10" means user sets kp to 10
 * Example 2: "i 10" means user sets ki to 10
 * Example 3: "d 5" means user sets kd to 5
 */
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
