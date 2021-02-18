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

//<<<<<<< HEAD
  // PID
  myPID.SetOutputLimits(-255, 255);
  myPID.SetMode(AUTOMATIC);
  previousTimestamp = micros();
//=======
  //Tachometer
  pinMode(TACHOMETER,INPUT);
  attachInterrupt(digitalPinToInterrupt(TACHOMETER), rotation, FALLING);
//>>>>>>> d635b3dfdfa117ae4be70327f1aa0fe7ac125ccc
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
        delay(200);
        if (digitalRead(JOYSTICK_PRESS) == LOW) {
          if (currentCursorLine == 1) {
            cycleRunTime = SMALL_CYCLE_TIME;
            levelValue = 21;
            printString2(0, 0, "Short Time Selected");
          }
          else if (currentCursorLine == 2) {
            cycleRunTime = MEDIUM_TIME;
            levelValue = 17.2;
            printString2(0, 0, "Medium Time Selected");
          }
          else if (currentCursorLine == 3) {
            cycleRunTime = LARGE_CYCLE_TIME;
            levelValue = 13.2;
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
    case 2: { //Update LCD screen to inform user about water level
        printString2(0, 0, "Please add water");
        delay(2000);
        startNextCycle();
        break;
      }
    case 3: { //Measure water level for user
        distance = measureWaterLevel();
        //sprintf(levelString, "%f + cm",distance);//-levelValue); //may need to change levelValue above
        printString(0, 1, "Increase level by:");
        printString(0, 2, (String)distance);
        delay(100);
        if(distance < levelValue)
        {
          printString2(0,0, "Press to start");
          while(digitalRead(JOYSTICK_PRESS) == HIGH){} //When joystick is pressed = LOW
          startNextCycle();
        }
        break;
      }
    case 4: { // Clear screen and set time
        //Failure code 1 notes: if water level does not change, set error code
        /*checkForPause();
        closeWater();
        if(failureCode != 0)
        {
          programState = 13;
        }
        else if (checkValve()) {
          startNextCycle();
        }
        break;*/
        lcd.clear();
        delay(1500);
        startNextCycle();
        endTimerTime=caseStartTime+300000;
        break;
      }
    case 5: { // Run the motor for the washing cycle and display time
        //Failure code 2 notes: if water level does not stop, set error code
        /*if(failureCode != 0)
        {
          programState = 13;
        }
        if(distance - measureWaterLevel() > 4)//this value will need some tweaking
        {
          failureCode = 2;
        }*/
        //Failure code 3 notes: if the motor speed does not change, set error code
        checkForPause();
        /*remainingTimerTime = endTimerTime - millis();
        checkForPause();
        setMotorSpeed(60);
        delay(2000);
        setMotorSpeed(0);
        startNextCycle();*/
        runMotorLoop();
        displayTime(endTimerTime);
        if(millis()-caseStartTime > 300000) //Stop motor after time ends
        {
          setMotorSpeed(0);
          startNextCycle();
        }
        break;
      }
    case 6: {//Turn on pump
      checkForPause();
      if(checkPump())
      {
        turnOnPump();
      }
      if(abs(distance - emptyLevel)<0.1)
      {
        if(drainTimer = 0)
        {
          drainTimer = millis();
        }
        if(millis()-drainTimer > 120000)
        {
          digitalWrite(PUMP_OUT_ENABLE, LOW);
          startNextCycle();
        }
      }
      else
      {
        distance = measureWaterLevel();
        sprintf(levelString, "%f + cm",distance);
        printString2(0, 0, levelString);
      }
      
      break;
    }
    case 7: {
      if(!isRinsed)
      {
        programState = 2;
        isRinsed = true;
      }
      else
      {
        speedTarget=200;
        lcd.clear();
        delay(500);
        startNextCycle();
        endTimerTime=caseStartTime+300000;
      }
      
      break;
    }
    case 8: {
      checkForPause();
      runMotorLoop();
      displayTime(endTimerTime);
      if(millis()-caseStartTime > 300000) //Stop motor after time ends
      {
        setMotorSpeed(0);
        startNextCycle();
      }
      break;
    }
    case 9: {
      printString2(0,0,"Completed");
      while(!checkLid()){}
      programState = 0;
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
    }
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
//}
