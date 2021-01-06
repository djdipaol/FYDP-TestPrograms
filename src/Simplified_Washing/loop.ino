void setup() {
  Serial.begin(9600);
  // LCD
  lcd.init();
  lcd.clear();
  lcd.backlight();

  // Joystick
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  pinMode(JOYSTICK_PRESS, INPUT);

  // Solenoid Valve and Pump Relay
  pinMode(VALVE_IN_ENABLE, OUTPUT);
  pinMode(PUMP_OUT_ENABLE, OUTPUT);
  digitalWrite(VALVE_IN_ENABLE, HIGH);
  digitalWrite(PUMP_OUT_ENABLE, LOW);

  // Ultrasonic: Water Level
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);

  // Motor
  pinMode(PWM, OUTPUT);
  pinMode(directionControl, OUTPUT);
  digitalWrite(directionControl, LOW);
}

void loop() {
  switch (programState) {
    case 0: { // Start screen
        initializeScreen();
        startNextCycle();
        break;
      }
    case 1: { // User Selection
        xPos = analogRead(JOYSTICK_X);
        yPos = analogRead(JOYSTICK_Y);
        delay(1000);
        if (digitalRead(JOYSTICK_PRESS) == HIGH) {
          if (currentCursorLine == 1) {
            cycleRunTime = SMALL_CYCLE_TIME;
            printString2(0, 0, "Short Time Selected");
          }
          else if (currentCursorLine == 2) {
            cycleRunTime = MEDIUM_TIME;
            printString2(0, 0, "Medium Time Selected");
          }
          else if (currentCursorLine == 3) {
            cycleRunTime = LARGE_CYCLE_TIME;
            printString2(0, 0, "Long Time Selected");
          }
          startNextCycle();
          lcd.noCursor();
        }
        else {
          if (yPos  ==  1023) {
            currentCursorLine += 1;
            if (currentCursorLine > 3) {
              currentCursorLine = 1; //check overflow
            }
          }
          else if (yPos  ==  0) {
            currentCursorLine -= 1; //this moves the cursor down
            if (currentCursorLine < 1) {
              currentCursorLine = 3; //check overflow
            }
          }
          lcd.setCursor(12, currentCursorLine);
          lcd.cursor();
        }
        break;
      }
    case 2: { // Check door is closed
        if (!checkLid()) {
          startNextCycle();
        }
        else {
          printString2(0, 0, "Lid is not closed");
          delay(1000);
          programState = 0;
        }
        break;
      }
    case 3: { // Open inlet valve
        openValve();
        startNextCycle();
        break;
      }
    case 4: { // Check water level and close inlet valve
        checkForPause(3);
        closeWater();
        if (checkValve()) {
          startNextCycle();
        }
        break;
      }
    case 5: { // Run the motor for the washing cycle and display time
        remainingTimerTime = endTimerTime - millis();
        checkForPause();
        setMotorSpeed(60);
        delay(2000);
        setMotorSpeed(0);
        startNextCycle();
        break;
      }
    case 6: { //Stop motor
        lcd.clear();
        setMotorSpeed(0);
        startNextCycle();
        break;
      }
    case 7: { //Start drain pump
        turnOnPump();
        startNextCycle();
        break;
      }
    case 8: { //Check water level missing, Stop drain pump
        checkForPause();
        turnOffPump();
        if (checkPump) {
          if (isRinsed) {
            startNextCycle();
          }
          else {
            programState = 3;
            Serial.println("End of state 8 for first time");
            isRinsed = true;
          }
        }
        break;
      }
    case 9: { // Run motor at high speed
        checkForPause();
        setMotorSpeed(110);
        delay(2000);
        setMotorSpeed(0);
        startNextCycle();

        break;
      }
    case 10: { //Stop motor
        lcd.clear();
        setMotorSpeed(0);
        startNextCycle();
        break;
      }
    case 11: { //Display end message, then reset
        printString2(0, 0, "Cycle Complete");
        while (digitalRead(JOYSTICK_PRESS == LOW)) {}
        programState = 0;
        Serial.println("End of state 11");
        break;
      }
    case 12: { // Pause
        setMotorSpeed(0);
        closeWater();
        turnOffPump();
        printString2(0, 0, "Paused");
        while (digitalRead(JOYSTICK_PRESS) == HIGH) {}
        while (digitalRead(JOYSTICK_PRESS == LOW)) {}
        if (remainingTimerTime != 0) {
          endTimerTime = ((remainingTimerTime * 60UL) * 1000) + millis(); //update final time to account for the pause that has occurred
          remainingTimerTime = 0; //reset the remainingTimerTime variable
        }
        programState = lastSavedState;
        break;
      default: { //default state used as an error state
          printString2(0, 0, "Error code 100");
          delay(1000);
          programState = 0;
          break;
        }
      }
  }
}
