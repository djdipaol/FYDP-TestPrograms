#include "src/Arduino-PID-Library/PID_v1.h"

// Tuning Constants
byte incomingByte;
double kp = 0.1;
double ki = 5;
double kd = 0.00005;

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
  if (Serial.available() > 0) {
    Serial.println("hiddi");
    String command = Serial.readString();
    command.replace(" ", "");
    if (command.indexOf("p") >= 0) {
      Serial.println("hii");
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
  
  timeElapsed = timeElapsed + micros() - previousTimestamp;
  previousTimestamp = micros();
  speedTarget = 50 * sin(timeElapsed / 1000000);

  int polarity = 1;
  if (random(0, 1) == 1) {
    polarity = -1;
  }
  speedActual = pwmOutput * 0.9 + random(0, 2) - random(0, 2);

  if (myPID.Compute()) {
    Serial.print("Input:");
    Serial.print(speedTarget);
    Serial.print(",");
    
    Serial.print("Actual:");
    Serial.print(speedActual);
    Serial.print(",");
    
    Serial.print("PWM:");
    Serial.print(pwmOutput);
    Serial.print(",");
    
    Serial.print("kP"+(String)kp+":");
    Serial.print(0);
    Serial.print(",");

    Serial.print("ki"+(String)ki+":");
    Serial.print(0);
    Serial.print(",");

    Serial.print("kd"+(String)kd+":");
    Serial.println(0);
  }
}
