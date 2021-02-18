/**
   Set motor speed to PWM channel
   @params motorSpeed integer for PWM signal to motor controller
   @returns
*/
void setMotorSpeed(int motorSpeed) {
  analogWrite(PWM , motorSpeed);
}
/**
  Change the motor direction from low to high or vice versa
  @params
  @returns
*/
void changeDirection() {
  if(digitalRead(directionControl) == LOW)
  {
    digitalWrite(directionControl, HIGH);
  }
  else
  {
    digitalWrite(directionControl, LOW);
  }
}

/**
  Interrupt function used to track the rotations of the pulley
  @params
  @returns
*/
void rotation() {
  magnetCount++;
}

/**
  Calculate the speed of the pulley
  @params
  @returns
*/
double calcSpeed()
{
  if(micros()-prevTime >= 1100000)
  {
    currTime=micros();
    speeds[speedArrayCount]= (double)magnetCount/((currTime-prevTime)/1000000.0)*60.0;
    //double currSpeed = (double)magnetCount/((currTime-prevTime)/1000000.0)*60.0;
    prevTime=currTime;
    magnetCount = 0;
    speedArrayCount = (speedArrayCount+1)%5;
    return (speeds[0]+speeds[1]+speeds[2]+speeds[3]+speeds[4])/5;
    //return currSpeed;
  }
  else
  {
    return -1;
  }
}

/**
   Initialize LCD screen for user selection
   @params
   @returns
*/
void initializeScreen() {
  lcd.clear();
  printString(0, 0, "Choose a program:");
  printString(0, 1, "Small load");
  printString(0, 2, "Medium load");
  printString(0, 3, "Large Load");
  lcd.cursor();
  lcd.setCursor(12, 1);
}

/**
   Prints a string to the LCD
   @params column Column number
   @params row Row number
   @params text Text to print to screen
   @returns
*/
void printString(int column, int row, String text) {
  lcd.setCursor(column, row);
  lcd.print(text);
}

/**
   Clears LCD screen, then prints a string to the LCD
   @params column Column number
   @params row Row number
   @params text Text to print to screen
   @returns
*/
void printString2(int column, int row, String text) {
  lcd.clear();
  lcd.setCursor(column, row);
  lcd.print(text);
}

/**
   Prints the remaining time to LCD screen
   @params endTimerTime Time to print to screen
   @returns
*/
void displayTime(unsigned long endTime) {
  int minute = ((endTime - millis()) / 1000 / 60) % 60;
  int second = ((endTime - millis()) / 1000) % 60;
  sprintf(remainingTime, "%0.2d:%0.2d", minute, second);
  printString(1, 0, remainingTime);
}

/**
   Checks if lid is open or closed
   @params
   @returns TRUE if open, FALSE if closed
*/
bool checkLid() {
  if (digitalRead(LID) != 0) {
    return true;
  }
  else {
    return false;
  }
}

/**
   Check if user wants to pause cycle or lid is open
   If either is true, pause cycle and save previous state
   @params
   @returns
*/
void checkForPause() {
  if (digitalRead(JOYSTICK_PRESS) == LOW || checkLid()) {
    Serial.print("Pausing cycle for ");
    if(checkLid())
    {
      Serial.println("Lid ");
    }
    if(digitalRead(JOYSTICK_PRESS) == LOW)
    {
      Serial.println("Joystick ");
    }
    while (digitalRead(JOYSTICK_PRESS) == LOW) {} // Wait for user to release button
    lastSavedState = programState;
    programState = 12;
  }
}

/** OVERRIDE
   Check if user wants to pause cycle or lid is open
   If either is true, pause cycle and save previous state
   @Overrides the last saved state to user input integer
   @params state State that user returns to after pause is cancelled
   @returns
*/
void checkForPause(int state) {
  if (digitalRead(JOYSTICK_PRESS) == HIGH || checkLid()) {
    Serial.println("Pausing cycle");
    while (digitalRead(JOYSTICK_PRESS) == HIGH) {} // Wait for user to release button
    lastSavedState = state;
    programState = 12;
  }
}

/**
   Increase state by 1, print state to serial
   @params
   @returns
*/
void startNextCycle() {
  Serial.println("End of state " + (String)programState);
  programState = programState + 1;
  caseStartTime=millis(); //added as a way to record when each state starts
}

/**
   Opens the valve of the water inlet
   @params
   @returns
*/
void openValve() {
  digitalWrite(VALVE_IN_ENABLE, HIGH); //open the valve for water inlet
  //we need to test the Relay ON or OFF for opening the valve, in some cases, the relay works reversly: LOW is on, HIGH is Off.
  Serial.println("opening  Valve");
}

/**
   Closes the valve of the water inlet
   @params
   @returns
*/
void closeWater() {
  if(caseStartTime == 0) //this if may be unnecessary
  {
    caseStartTime = millis();
  }
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
  duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  distance = (duration / 2) * 0.0344;
  if (distance >= 400 || distance <= 2) {
    Serial.print("Distance = ");
    Serial.println("Out of range");
    failureCode=7;
  }
  else {
    //Preliminary failure code 
    ////////////////////////////////////////////////////////////
    if(initialDist == -1)
    {
      initialDist = distance;
    }
    if(millis()-caseStartTime>60000 && fabs(distance-initialDist)<0.5)
    {
      failureCode=1;
      digitalWrite(VALVE_IN_ENABLE, LOW); //Close the valve to stop water inlet
      valveStatus = digitalRead(VALVE_IN_ENABLE);
    }
    /////////////////////////////////////////////////////////////
    if (currentCursorLine  ==  LARGE_CYCLE) { //if large load program selected, run following
      if (distance  <  13.2) {
        digitalWrite(VALVE_IN_ENABLE, LOW); //Close the valve to stop water inlet
        valveStatus = digitalRead(VALVE_IN_ENABLE);
      }
    }
    else if (currentCursorLine  ==  MEDIUM_CYCLE) {//if medium load program selected, run following
      if (distance  <  17.2) {
        digitalWrite(VALVE_IN_ENABLE, LOW); //Close the valve to stop water inlet
        valveStatus = digitalRead(VALVE_IN_ENABLE);
      }
    }
    else if (currentCursorLine  ==  SMALL_CYCLE) {//if small load program selected, run following
      if (distance  <  21) {
        digitalWrite(VALVE_IN_ENABLE, LOW); //Close the valve to stop water inlet
        valveStatus = digitalRead(VALVE_IN_ENABLE);
      }
    }
  }
}

/**
   Opens the valve of the water inlet
   @params
   @returns
*/
bool checkValve() {
  if (valveStatus  ==  LOW) {
    return true;
  }
  else {
    return false;
  }
}

/**
   Turns on pump for water discharge
   @params
   @returns
*/
void turnOnPump() {
  Serial.println("ON");
  digitalWrite(PUMP_OUT_ENABLE, HIGH); //turn on the pump for water drain
  //we need to test the Relay ON or OFF for opening the valve, in some cases, the relay works reversly: LOW is on, HIGH is Off.
}

/**
   Turns off pump and stop water discharge
   @params
   @returns
*/
void turnOffPump() {
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
  duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  distance = (duration / 2) * 0.0344;
  if (distance >= 400 || distance <= 2) {
    Serial.print("Distance = ");
    Serial.println("Out of range");
    failureCode=7;
  }
  
  else if (distance == 25) {
    digitalWrite(PUMP_OUT_ENABLE, LOW);
    pumpStatus = digitalRead(PUMP_OUT_ENABLE);
  }

  //Preliminary failure code 
  ////////////////////////////////////////////////////////////
  if(initialDist == -1)
  {
    initialDist = distance;
  }
  if(millis()-caseStartTime>60000 && fabs(distance-initialDist)<0.5)
  {
    failureCode=6;
    digitalWrite(PUMP_OUT_ENABLE, LOW); //Close the valve to stop water inlet
    pumpStatus = digitalRead(PUMP_OUT_ENABLE);
  }
  /////////////////////////////////////////////////////////////
}

/**
   Turns off pump and stop water discharge
   @params
   @returns true if pump is off
*/
bool checkPump() {
  if (pumpStatus  ==  LOW) {
    return true;
  }
  else {
    return false;
  }
}

/**
<<<<<<< HEAD
   Checks for user input into PID using Serial Plotter Graph Input
   @params 
   @returns 
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
    else if (command.indexOf("s") >= 0) {
      String textD = command.substring(command.indexOf("s") + 1, command.length());
      speedTarget = textD.toDouble();
    }
    myPID.SetTunings(kp,ki,kd);
  }
}

/**
   Runs one step in the PID Motor Control algorithm
   @params 
   @returns 
*/
void runMotorLoop() {
// Check if user has changed  Kp, Ki, Kd
  if(micros()-prevTime >= 1100000)//need min of 1 sec if the lowest speed is 60rpm
  {
    checkInput();
    // Calculate time from start
    timeElapsed = timeElapsed + micros() - previousTimestamp;
    previousTimestamp = micros();
    // Set speed target based on random sinusoidal function
    //speedTarget = 200; //* sin(timeElapsed / 1000000);
    //speedTarget = log(timeElapsed / 1000000);
    // Simulate sensor as delayed PWM and random forcing
    speedActual = calcSpeed();//pwmOutput * 0.9 + random(0, 20) - random(0, 20) + 0;
  
    if (myPID.Compute()) {
      Serial.print("Input:" + (String)speedTarget + ",");
      Serial.print("Actual:" + (String)speedActual + ",");
      Serial.print("PWM:" + (String)(0.5*pwmOutput) + ",");
      Serial.print("kP="+(String)kp+":0,");
      Serial.print("ki="+(String)ki+":0,");
      Serial.println("kd="+(String)kd+":0");
    }
    analogWrite(PWM,0.5*pwmOutput);
  }
}
/*=======
   Check water level
   @params
   @returns measurement of water level in cm
*/
float measureWaterLevel()
{
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
  float dur = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  float dist = (dur / 2) * 0.0344;
  if (dist >= 400 || dist <= 2) {
    Serial.print("Distance = ");
    Serial.println("Out of range");
    failureCode=7;
    dist = -1;
  }
  return dist;
//>>>>>>> d635b3dfdfa117ae4be70327f1aa0fe7ac125ccc
}
