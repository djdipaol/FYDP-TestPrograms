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
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  // Motor
  pinMode(PWM, OUTPUT);
  pinMode(directionControl, OUTPUT);
  digitalWrite(directionControl, LOW);
}

void loop() {
  switch(programState)
  {
    //Draw start screen
    case 0:
      displaySelection(); //print the opening screen that lists all of the selection options
      programState=1;
      Serial.println("End of state 0");
      break;
    case 1:
      xPos=analogRead(JOYSTICK_X); //not sure if this is actually used...
      yPos=analogRead(JOYSTICK_Y);
      if(digitalRead(JOYSTICK_PRESS)==HIGH){
        if(cursorLoc==1){
          progTime=shortTime; //if first line is selected, the program will use the short time
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Short Time Selected");
          delay(1000);
        }
        else if(cursorLoc==2){
          progTime=medTime; //if second line is selected, the program will use the medium time
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Medium Time Selected");
          delay(1000);
        }
        else if(cursorLoc==3){
          progTime=largeTime; //if third line is selected, the program will use the long time
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Long Time Selected");
          delay(1000);
        }
        programState = 2;
        Serial.println("End of state 1");
        lcd.noCursor(); //hide cursor after selection is made
      }
      else{
        if(yPos == 1023){
          cursorLoc+=1; //may need to switch + to -, this moves the cursor up
          if(cursorLoc>3){
            cursorLoc=1; //check overflow
          }
        }
        else if(yPos == 0){
          cursorLoc-=1; //this moves the cursor down
          if(cursorLoc<1){
            cursorLoc=3; //check overflow
          }
        }
        lcd.setCursor(12,cursorLoc); //may need to change 12 to another number, display cursor at the end of the lines
        lcd.cursor(); //this displays the cursor on the screen
      }
      break;
    //Check door is closed
    case 2:
      //check that the door is closed
      if(!checkLID()){
        programState=3; //if checkLID is false, then the program will continue
        Serial.println("End of state 2");
      }
      else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("LID is not closed");
        delay(1000);
        programState=0; //if checkLID is true, then the program will go back to the selection state
      }
      break;
    //Open inlet valve
    case 3:
      openValve(); //This function should open the solenoid valve
      programState=4;
      Serial.println("End of state 3");
      break;
    //Check water level and close inlet valve
    case 4:
      if(digitalRead(JOYSTICK_PRESS)==HIGH || checkLID())//if the button is pressed, the program should change to the pause state
      {
        while(digitalRead(JOYSTICK_PRESS)==HIGH){}//wait until the button is released
        programState=12; //go to the pause state
        returnState=3; //once the pause is finished, return to the previous state to open the valve again
      }
      else
      {
        closeValve();
        if(CheckValve())
        {
         programState=5;
         Serial.println("End of state 4");
        }
      }
      break;
    //Run the motor for the washing cycle and display time
    case 5:
      if(digitalRead(JOYSTICK_PRESS)==HIGH || checkLID())//if the button is pressed, the program should change to the pause state
      {
        while(digitalRead(JOYSTICK_PRESS)==HIGH){}//wait until the button is released
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
    //Stop motor
    case 6:
      lcd.clear();
      analogWrite(PWM ,0); //turn off the motor
      programState = 7;
      Serial.println("End of state 6");
      break;
    //Start drain pump
    case 7:
      turnONpump();
      programState=8;
      Serial.println("End of state 7");
      break;
    //Check water level
    //Stop drain pump
    case 8:
      if(digitalRead(JOYSTICK_PRESS)==HIGH || checkLID())//if the button is pressed, the program should change to the pause state
      {
        while(digitalRead(JOYSTICK_PRESS)==HIGH){}//wait until the button is released
        programState=12; //go to the pause state
        returnState=8; //once the pause is finished, return to the previous state to open the valve again
      }
      else
      {
        turnOFFpump();
        if(CheckPump())
        {
          if(hasRinsed)
          {
           programState=9;
           Serial.println("End of state 8 for second time");
          }
          else
          {
            programState=3;
            Serial.println("End of state 8 for first time");
            hasRinsed=true;
          }
        }
      }
      break;
    //Run motor at high speed
    case 9:
      if(digitalRead(JOYSTICK_PRESS)==HIGH || checkLID())//if the button is pressed, the program should change to the pause state
      {
        while(digitalRead(JOYSTICK_PRESS)==HIGH){}//wait until the button is released
        programState=12; //go to the pause state
        returnState=9; //once the pause is finished, return to this state
      }
      else
      {
        analogWrite(PWM ,110);
        delay(2000);
        analogWrite(PWM ,0);
        programState=10;
        Serial.println("End of state 9");
      }
      break;
    //Stop motor
    case 10:
      lcd.clear();
      analogWrite(PWM ,0); //turn off the motor, check if this is safe
      
      programState=11;
      Serial.println("End of state 10");
      break;
    //Display end message, then reset
    case 11:
      lcd.clear();
      lcd.print("Cycle complete");
      while(digitalRead(JOYSTICK_PRESS==LOW)){}
      programState=0;
      Serial.println("End of state 11");
      break;
    //Pause state - all devices should be turned off and a message should be displayed
    //pressing the pause button again should resume to the previous state
    case 12:
      analogWrite(PWM ,0); //turn off the motor, this value may need to change
      closeValve();
      turnOFFpump();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Paused");
      while(digitalRead(JOYSTICK_PRESS)==HIGH){} //this may not be necessary, right now it is in case the button is left pressed down when pausing
      while(digitalRead(JOYSTICK_PRESS==LOW)){}
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
