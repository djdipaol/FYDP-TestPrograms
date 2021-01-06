void setup() {
  Serial.begin(9600);
  // LCD
  lcd.init();
  lcd.clear();
  lcd.backlight();

  // Joystick
  pinMode(JOYSTICK_X,INPUT);
  pinMode(JOYSTICK_Y,INPUT);
  pinMode(JOYSTICK_PRESS,INPUT);

  // Solenoid Valve and Pump Relay
  pinMode(VALVE_IN_ENABLE, OUTPUT); 
  pinMode(PUMP_OUT_ENABLE,OUTPUT);
  digitalWrite(VALVE_IN_ENABLE, HIGH); 
  digitalWrite(PUMP_OUT_ENABLE,LOW);

  // Ultrasonic: Water Level
  pinMode(ULTRASONIC_TRIGGER_PIN,OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN,INPUT);

  // Motor
  pinMode(PWM, OUTPUT);
  pinMode(directionControl, OUTPUT);
  digitalWrite(directionControl, LOW);
}

void loop() {
  switch(programState) {
    case 0: { // Start screen
      lcd.clear();
      displayString(0,0,"Choose a program:");
      displayString(0,1,"Small load");
      displayString(0,2,"Medium load");
      displayString(0,3,"Large Load");
      lcd.cursor();
      lcd.setCursor(12,1);
      programState = 1;
      Serial.println("End of state 0");
      break;
    }
    case 1: { // User Selection
      xPos = analogRead(JOYSTICK_X); //not sure if this is actually used...
      yPos = analogRead(JOYSTICK_Y);
      delay(1000);
      if(digitalRead(JOYSTICK_PRESS) == HIGH){
        lcd.clear();
        if(currentCursorLine == 1) {
          progTime = shortTime; //if first line is selected, the program will use the short time
          displayString(0,0,"Short Time Selected");
        }
        else if(currentCursorLine == 2){
          progTime = medTime; //if second line is selected, the program will use the medium time
          displayString(0,0,"Medium Time Selected");
        }
        else if(currentCursorLine == 3){
          progTime=largeTime; //if third line is selected, the program will use the long time
          displayString(0,0,"Long Time Selected");
        }
        programState = 2;
        Serial.println("End of state 1");
        lcd.noCursor(); //hide cursor after selection is made
      }
      else{
        if(yPos  ==  1023){
          currentCursorLine+=1; //may need to switch + to -, this moves the cursor up
          if(currentCursorLine>3){
            currentCursorLine=1; //check overflow
          }
        }
        else if(yPos  ==  0){
          currentCursorLine-=1; //this moves the cursor down
          if(currentCursorLine<1){
            currentCursorLine=3; //check overflow
          }
        }
        lcd.setCursor(12,currentCursorLine); //may need to change 12 to another number, display cursor at the end of the lines
        lcd.cursor(); //this displays the cursor on the screen
      }
      break;
    }
    case 2: { // Check door is closed
      if(!checkLID()){
        programState=3; //if checkLID is false, then the program will continue
        Serial.println("End of state 2");
      }
      else{
        lcd.clear();
        displayString(0,0,"LID is not closed");
        delay(1000);
        programState=0; //if checkLID is true, then the program will go back to the selection state
      }
      break;
    }
    case 3: { // Open inlet valve
      openValve(); //This function should open the solenoid valve
      programState=4;
      Serial.println("End of state 3");
      break;
    }
    case 4: { // Check water level and close inlet valve
      if(digitalRead(JOYSTICK_PRESS) == HIGH || checkLID()) {//if the button is pressed, the program should change to the pause state
        while(digitalRead(JOYSTICK_PRESS) == HIGH){} //wait until the button is released
        programState=12; //go to the pause state
        returnState=3; //once the pause is finished, return to the previous state to open the valve again
      }
      else {
        closeValve();
        if(CheckValve()) {
         programState=5;
         Serial.println("End of state 4");
        }
      }
      break;
    }
    case 5: { // Run the motor for the washing cycle and display time
      if(digitalRead(JOYSTICK_PRESS) == HIGH || checkLID())//if the button is pressed, the program should change to the pause state
      {
        while(digitalRead(JOYSTICK_PRESS) == HIGH){}//wait until the button is released
        programState=12; //go to the pause state
        returnState=5; //once the pause is finished, return to this state
        remainingTime=finalTime-millis();
      }
      else
      {
        analogWrite(PWM ,60);
        delay(2000);
        analogWrite(PWM ,0);
        programState=6;
        Serial.println("End of state 5");
      }
      break;
    }
    case 6: { //Stop motor
      lcd.clear();
      analogWrite(PWM ,0); //turn off the motor
      programState = 7;
      Serial.println("End of state 6");
      break;
    }
    case 7: { //Start drain pump
      turnONpump();
      programState=8;
      Serial.println("End of state 7");
      break;
    }
    case 8: { //Check water level missing, Stop drain pump
      if(digitalRead(JOYSTICK_PRESS) == HIGH || checkLID())//if the button is pressed, the program should change to the pause state
      {
        while(digitalRead(JOYSTICK_PRESS) == HIGH){}//wait until the button is released
        programState=12; //go to the pause state
        returnState=8; //once the pause is finished, return to the previous state to open the valve again
      }
      else {
        turnOFFpump();
        if(CheckPump()) {
          if(hasRinsed) {
           programState=9;
           Serial.println("End of state 8 for second time");
          }
          else {
            programState=3;
            Serial.println("End of state 8 for first time");
            hasRinsed=true;
          }
        }
      }
      break;
    }
    case 9: { // Run motor at high speed
      if(digitalRead(JOYSTICK_PRESS) == HIGH || checkLID()) {//if the button is pressed, the program should change to the pause state
        while(digitalRead(JOYSTICK_PRESS) == HIGH){}//wait until the button is released
        programState=12; //go to the pause state
        returnState=9; //once the pause is finished, return to this state
      }
      else {
        analogWrite(PWM ,110);
        delay(2000);
        analogWrite(PWM ,0);
        programState=10;
        Serial.println("End of state 9");
      }
      break;
    }
    case 10: { //Stop motor
      lcd.clear();
      analogWrite(PWM ,0); //turn off the motor, check if this is safe
      programState=11;
      Serial.println("End of state 10");
      break;
    }
    case 11: { //Display end message, then reset
      lcd.clear();
      lcd.print("Cycle complete");
      while(digitalRead(JOYSTICK_PRESS == LOW)){}
      programState=0;
      Serial.println("End of state 11");
      break;
    }
    case 12: { // Pause
      analogWrite(PWM ,0); //turn off the motor, this value may need to change
      closeValve();
      turnOFFpump();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Paused");
      while(digitalRead(JOYSTICK_PRESS) == HIGH){} //this may not be necessary, right now it is in case the button is left pressed down when pausing
      while(digitalRead(JOYSTICK_PRESS == LOW)){}
      if(remainingTime!=0)
      {
        finalTime = ((remainingTime*60UL)*1000) + millis(); //update final time to account for the pause that has occurred
        remainingTime = 0; //reset the remainingTime variable
      }
      programState=returnState;
      break;
    //default state used as an error state
    default:
      lcd.setCursor(0,0);
      lcd.print("Error please restart");
      delay(1000);
      programState=0;
      //Add all stopping functions
      break;
    }
  }
}
