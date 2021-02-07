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

<<<<<<< HEAD
  // PID
  myPID.SetOutputLimits(-255, 255);
  myPID.SetMode(AUTOMATIC);
  previousTimestamp = micros();
=======
  //Tachometer
  pinMode(TACHOMETER,INPUT);
  attachInterrupt(digitalPinToInterrupt(TACHOMETER), rotation, FALLING);
>>>>>>> d635b3dfdfa117ae4be70327f1aa0fe7ac125ccc
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
        delay(500);
        if (digitalRead(JOYSTICK_PRESS) == LOW) {
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
        //Failure code 1 notes: if water level does not change, set error code
        checkForPause();
        closeWater();
        if(failureCode != 0)
        {
          programState = 13;
        }
        else if (checkValve()) {
          startNextCycle();
        }
        break;
      }
    case 5: { // Run the motor for the washing cycle and display time
        //Failure code 2 notes: if water level does not stop, set error code
        if(failureCode != 0)
        {
          programState = 13;
        }
        if(distance - measureWaterLevel() > 4)//this value will need some tweaking
        {
          failureCode = 2;
        }
        //Failure code 3 notes: if the motor speed does not change, set error code
        checkForPause();
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
        magnetCount=0;
        startNextCycle();
        break;
      }
    case 7: { //Start drain pump
        //Failure code 6 notes: if the water level does not lower, set error code
        turnOnPump();
        startNextCycle();
        break;
      }
    case 8: { //Check water level missing, Stop drain pump
        checkForPause();
        //Failure code 4 notes: if the motor speed does not fall to zero, set error code
        if(failureCode != 0)
        {
          programState = 13;
        }
        if(magnetCount != 0)//this value will need some tweaking
        {
          failureCode = 4;
        }
        turnOffPump();
        if(failureCode != 0)
        {
          programState = 13;
        }
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
        //Failure code 3 notes: if the motor speed does not change, set error code
        checkForPause();
        setMotorSpeed(110);
        delay(2000);
        setMotorSpeed(0);
        startNextCycle();

        break;
      }
    case 10: { //Stop motor
        //Failure code 4 notes: if the motor speed does not fall to zero, set error code
        lcd.clear();
        setMotorSpeed(0);
        magnetCount=0;
        delay(2000); //give motor 2 seconds to stop
        if(magnetCount != 0)
        {
          failureCode = 4;
        }
        if(failureCode != 0)//this value will need some tweaking
        {
          programState = 13;
        }
        startNextCycle();
        break;
      }
    case 11: { //Display end message, then reset
        printString2(0, 0, "Cycle Complete");
        while (digitalRead(JOYSTICK_PRESS == HIGH)) {}
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
     case 13: {
        //Failure Code State, start by turning high power device off
        setMotorSpeed(0);
        closeWater();
        turnOffPump();
        //using an if statement for now, can possible make a switch statement
        if(failureCode==0)
        {
          printString2(0,0,"Undefined Failure");
        }
        else if(failureCode==1)
        {
          printString2(0,0,"Water valve open failure");
        }
        else if(failureCode==2)
        {
          printString2(0,0,"Water valve close failure");
        }
        else if(failureCode==3)
        {
          printString2(0,0,"Motor start failure");
        }
        else if(failureCode==4)
        {
          printString2(0,0,"Motor stop failure");
        }
        else if(failureCode==5)
        {
          printString2(0,0,"Motor speed failure");
        }
        else if(failureCode==6)
        {
          printString2(0,0,"Pump start failure");
        }
        else if(failureCode==7)
        {
          printString2(0,0,"US range failure");
        }
        else
        {
          printString2(0,0,"Fail code overflow");
        }
        break;
     }
      default: { //default state used as an error state
          printString2(0, 0, "Error code 100");
          delay(1000);
          programState = 0;
          break;
        }
      }
  }
}
