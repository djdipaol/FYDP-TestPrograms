/**
   Set motor speed to PWM channel
   @params motorSpeed integer for PWM signal to motor controller
   @returns
*/
void setMotorSpeed(int motorSpeed) {
  analogWrite(PWM , motorSpeed);
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
void displayTime(unsigned long endTimerTime) {
  int minute = ((endTimerTime - millis()) / 1000 / 60) % 60;
  int second = ((endTimerTime - millis()) / 1000) % 60;
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
  if (digitalRead(JOYSTICK_PRESS) == HIGH || checkLid()) {
    Serial.println("Pausing cycle");
    while (digitalRead(JOYSTICK_PRESS) == HIGH) {} // Wait for user to release button
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
  }
  else {
    if (currentCursorLine  ==  LARGE_CYCLE) { //if large load program selected, run following
      if (distance  ==  13.2) {
        digitalWrite(VALVE_IN_ENABLE, LOW); //Close the valve to stop water inlet
        valveStatus = digitalRead(VALVE_IN_ENABLE);
      }
    }
    else if (currentCursorLine  ==  MEDIUM_CYCLE) {//if medium load program selected, run following
      if (distance  ==  17.2) {
        digitalWrite(VALVE_IN_ENABLE, LOW); //Close the valve to stop water inlet
        valveStatus = digitalRead(VALVE_IN_ENABLE);
      }
    }
    else if (currentCursorLine  ==  SMALL_CYCLE) {//if small load program selected, run following
      if (distance  ==  21) {
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
  }
  else if (distance == 25) {
    digitalWrite(PUMP_OUT_ENABLE, LOW);
    pumpStatus = digitalRead(PUMP_OUT_ENABLE);
  }
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
