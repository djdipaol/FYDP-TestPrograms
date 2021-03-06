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
  attachInterrupt(digitalPinToInterrupt(TACHOMETER), rotation, CHANGE);
//>>>>>>> d635b3dfdfa117ae4be70327f1aa0fe7ac125ccc
  programTime - millis();
}

void loop() {
  /*Serial.print("Current state: "); //remove later
  Serial.println(programState);
  Serial.print("Failure Code: "); //remove later
  Serial.println(failureCode);*/
  switch (programState) {
    case 0: { // Start screen
        initializeScreen();
        isRinsed = false;
        maxSpeed = 60;
        startNextCycle();
        cycleTime = 30000;
        //programState=6;
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
        startNextCycle();
        delay(2000);
        break;
      }
    case 3: { //Measure water level for user
        /*if(failureCode != 0) //this transition should only occur if the Ultrasonic range is out of bounds
        {
          lastSavedState = programState;
          programState = 13;
        }
        distance = measureWaterLevel();
        //sprintf(levelString, "%f + cm",distance);//-levelValue); //may need to change levelValue above
        printString(0, 1, "Increase level by:");
        displayDist = levelValue-distance;
        if(displayDist < 0)
        {
          displayDist = 0;
        }
        printString(0, 2, (String)(displayDist));
        delay(100);
        Serial.println(waterLevelVerificationCounter);
        if(distance < levelValue && distance > minMeasurement && !checkLid())
        {
          waterLevelVerificationCounter++;
        }
        else {
          if (waterLevelVerificationCounter > 0) {
           waterLevelVerificationCounter--;
          }
        }
        if (waterLevelVerificationCounter > 4)
        {
          printString2(0,0, "Press to start");
          while(digitalRead(JOYSTICK_PRESS) == HIGH){} //When joystick is pressed = LOW
          startNextCycle();
        }*/
        //startNextCycle();//REMOVE

        distance = measureWaterLevel();
        printString(0, 2, (String)(distance-levelValue));
        if(digitalRead(JOYSTICK_PRESS) == LOW)
        {
          startNextCycle();
        }
        delay(500);
        break;
      }
    case 4: { // Clear screen and set time
        lcd.clear();
        delay(50);
        startNextCycle();
        endTimerTime=caseStartTime+cycleTime;
        break;
      }
    case 5: { // Run the motor for the washing cycle and display time
        if(failureCode != 0) // No failure if 0
        {
          lastSavedState = programState;
          programState = 13;
        }
        else if(stoppedCount > 15)//Failure code 3 notes: if the motor speed does not change, set error code
        {
          failureCode = 3;
          stoppedCount = 0;
        }
        else if(abs(speedActual-speedTarget) > speedTol && millis()-speedTimer > 10000)//Failure code 5: the motor speed does not settle
        {
          overshootCount++;
          speedTimer = millis();
          if(overshootCount > overshootLimit)
          {
            failureCode = 5;
            overshootCount = 0;
          }
        }
        else
        {
          checkForPause();
          switch(profileStep)
          {
            case 0:
              /*speedTarget += speedIncrement;
              if(speedTarget >= maxSpeed)
              {
                speedTarget = maxSpeed; 
                plateauTime = millis();
                profileStep = 1;
              }*/
              speedTarget = maxSpeed; 
              plateauTime = millis();
              profileStep = 1;
              break;
            case 1:
              if(millis()-plateauTime > 30000)
              {
                profileStep = 2;
              }
              break;
            case 2:
              /*speedTarget -= speedIncrement;
              if(speedTarget <= 0)
              {
                speedTarget = 0;
                plateauTime = millis();
                profileStep=3;
              }*/
              speedTarget = 0;
              plateauTime = millis();
              profileStep=3;
              break;
            case 3:
              if(millis()-plateauTime > 10000)
              {
                profileStep = 0;
                changeDirection();
              }
              break;
            default:
              //Serial.println("Speed profile error");
              break;
          }
          
          runMotorLoop();
          displayTime(endTimerTime);
          if(speedActual == 0 && millis()-stoppedTimer > 10000)
          {
            stoppedCount++;
            stoppedTimer = millis();
          }
          else if(speedActual != 0)
          {
            stoppedCount = 0;
          }
          
          if(millis()-caseStartTime > cycleTime) //Stop motor after time ends
          {
            setMotorSpeed(0);
            startNextCycle();
            profileStep = 0;
            speedTarget = 0;
            delay(500);
          }
        }
        break;
      }
    case 6: {//Turn on pump
      /*checkForPause();
      calcSpeed();
      if(failureCode != 0) //check for failure codes
      {
        lastSavedState = programState;
        programState = 13;
      }
      else if(speeds[4]>0) //check that the motor stops, using most recent speed
      {
        failureCode = 4;
      }
      else //run case as expected
      {
        if(checkPump())
        {
          turnOnPump();
          initialDist = measureWaterLevel();
        }
        if(abs(distance - emptyLevel)<2.5)
        {
          if(drainTimer == 0)
          {
            drainTimer = millis();
          }
          if(millis()-drainTimer > 5000)
          {
            digitalWrite(PUMP_OUT_ENABLE, LOW);
            drainTimer = 0;
            startNextCycle();
          }
        }
        else
        {
          distance = measureWaterLevel();
          //sprintf(levelString, "%f + cm",distance);
          //printString2(0, 0, levelString);
          printString(0, 2, (String)(distance));
          if((millis() - caseStartTime > 15000)&&(abs(initialDist-distance)<1))
          {
            failureCode = 6; //uncomment this
          }
        }
      }*/
      digitalWrite(PUMP_OUT_ENABLE, HIGH);
      delay(5000);
      distance = measureWaterLevel();
      printString(0, 2, (String)(distance-levelValue));
      if(digitalRead(JOYSTICK_PRESS) == LOW)
      {
        digitalWrite(PUMP_OUT_ENABLE, LOW);
        startNextCycle();
      }
      break;
    }
    case 7: {
      if(failureCode != 0) //check for failure codes
      {
        lastSavedState = programState;
        programState = 13;
      }
      else if(!isRinsed)
      {
        programState = 2;
        isRinsed = true;
        cycleTime = 30000;
      }
      else
      {
        //maxSpeed=200;
        speedTarget = 200;
        lcd.clear();
        delay(500);
        startNextCycle();
        endTimerTime=caseStartTime+cycleTime;
      }
      
      break;
    }
    case 8: {
      if(failureCode != 0)
        {
          lastSavedState = programState;
          programState = 13;
        }
        else if(stoppedCount > 15)//Failure code 3 notes: if the motor speed does not change, set error code
        {
          failureCode = 3;
          stoppedCount = 0;
        }
        else if(abs(speedActual-speedTarget) > speedTol && millis() - speedTimer > 10000)//Failure code 5: the motor speed does not settle
        {
          overshootCount++;
          speedTimer = millis();
          if(overshootCount > overshootLimit)
          {
            failureCode = 5;
            overshootCount =0;
          }
        }
        else
        {
          checkForPause();

          /*switch(profileStep)
          {
            case 0:
              speedTarget += speedIncrement;
              if(speedTarget >= maxSpeed)
              {
                speedTarget = maxSpeed; 
                plateauTime = millis();
                profileStep = 1;
              }
              break;
            case 1:
              if(millis()-plateauTime > 30000)
              {
                profileStep = 2;
              }
              break;
            case 2:
              speedTarget -= speedIncrement;
              if(speedTarget <= 0)
              {
                speedTarget = 0;
                plateauTime = millis();
                profileStep=3;
              }
              break;
            case 3:
              if(millis()-plateauTime > 1000)
              {
                profileStep = 0;
                changeDirection();
              }
              break;
            default:
              //Serial.println("Speed profile error");
              break;
          }*/
          
          runMotorLoop();
          displayTime(endTimerTime);
          if(speedActual == 0)
          {
            stoppedCount++;
          }
          else if(speedActual != 0)
          {
            stoppedCount = 0;
          }
          
          if(millis()-caseStartTime > cycleTime) //Stop motor after time ends
          {
            setMotorSpeed(0);
            startNextCycle();
            speedTarget = 0;
            profileStep = 0;
            delay(500);
          }
        }
      break;
    }
    case 9: {
      printString2(0,0,"Completed");
      while(!checkLid()){}
      programState = 0;
      printString(0, 2, (String)(millis()-programTime));
      //Serial.print("Total Time for cycle: ");
      //Serial.println(millis()-programTime);
      break;
    }
    case 12: { // Pause
        setMotorSpeed(0);
        waterLevelVerificationCounter = 0;
        closeWater(); //without valve, this function does not do anything
        //turnOffPump(); we may want to rework this function
        digitalWrite(PUMP_OUT_ENABLE, LOW); //used for now in place of turnOffPump()
        printString2(0, 0, "Paused");
        while (digitalRead(JOYSTICK_PRESS) == HIGH) {}
        while (digitalRead(JOYSTICK_PRESS) == LOW) {}
        if (remainingTimerTime != 0) {
          endTimerTime = ((remainingTimerTime * 60UL) * 1000) + millis(); //update final time to account for the pause that has occurred
          remainingTimerTime = 0; //reset the remainingTimerTime variable
        }
        programState = lastSavedState;
        delay(500);
        lcd.clear();
        break;
    }
     case 13: {
        //Failure Code State, start by turning high power device off
        setMotorSpeed(0);
        closeWater();
        //turnOffPump();
        digitalWrite(PUMP_OUT_ENABLE, LOW); //used for now in place of turnOffPump()
        //using an if statement for now, can possible make a switch statement
        if(failureCode==0)
        {
          printString2(0,0,"Undefined Failure");
        }
        else if(failureCode==1) //this should not appear right now as the valve is unused
        {
          printString2(0,0,"Water valve open failure");
        }
        else if(failureCode==2) //this should not appear right now as the valve is unused
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
        else if(failureCode==7) //this failure code is incorporated into the measureWaterLevelFunction
        {
          printString2(0,0,"US range failure");
        }
        else
        {
          printString2(0,0,"Fail code overflow");
        }

        while (digitalRead(JOYSTICK_PRESS) == HIGH) {}
        while (digitalRead(JOYSTICK_PRESS) == LOW) {}
        failureCode = 0; //reset failure code before returning to previous state
        programState = lastSavedState;
        break;
     }
      default: { //default state used as an error state
          printString2(0, 0, "Default case");
          delay(1000);
          programState = 0;
          break;
        }
      }
  }
//}
