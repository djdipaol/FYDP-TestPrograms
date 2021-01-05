/**
 * Displays selection on LCD screen.
 * @params 
 * @returns
 */
void displaySelection(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Choose a program");
  lcd.setCursor(0,1);
  lcd.print("Small load");
  lcd.setCursor(0,2);
  lcd.print("Medium load"); //set cursor location to 12
  lcd.setCursor(0,3);
  lcd.print("Large load");
  lcd.cursor(); //this displays the cursor on the screen
  lcd.setCursor(12,1);
}

/**
 * Prints the remaining time to LCD screen
 * @params finalTime Time to print to screen
 * @returns
 */
void displayTime(unsigned long finalTime) {
  lcd.setCursor(1,0);
  int minute = ((finalTime-millis())/1000/60)%60;
  int second = ((finalTime-millis())/1000)%60;
  sprintf(timeString, "%0.2d:%0.2d", minute, second);
  lcd.print(timeString);
}

/**
 * Checks if LID is open or closed
 * @params
 * @returns TRUE if open, FALSE if closed
 */
bool checkLID() {
  if(digitalRead(LIDPin)!=0){return true;}
  else {return false;}
}

/**
 * Checks if LID is open or closed
 * @params
 * @returns TRUE if open, FALSE if closed
 */
void openValve(){
  digitalWrite(VALVE_IN_ENABLE,HIGH); //open the valve for water inlet
  //we need to test the Relay ON or OFF for opening the valve, in some cases, the relay works reversly: LOW is on, HIGH is Off.
  Serial.println("opening  Valve");
}

void closeValve(){  
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) * 0.0344;
 
  if (distance >= 400 || distance <= 2){
    Serial.print("Distance = ");
    Serial.println("Out of range");
  }
  else {
    if(cursorLoc == 3){//if large load program selected, run following
      if(distance == 13.2) {
        digitalWrite(VALVE_IN_ENABLE,LOW); //Close the valve to stop water inlet
          valve_status=digitalRead(VALVE_IN_ENABLE);
      }
    }
    else if(cursorLoc == 2){//if medium load program selected, run following
        if(distance == 17.2){
        digitalWrite(VALVE_IN_ENABLE,LOW); //Close the valve to stop water inlet
          valve_status=digitalRead(VALVE_IN_ENABLE);
        }
    }
    else if(cursorLoc == 1){//if small load program selected, run following
      if(distance == 21){
        digitalWrite(VALVE_IN_ENABLE,LOW); //Close the valve to stop water inlet
          valve_status=digitalRead(VALVE_IN_ENABLE);
      }
    }
  
  }
  //removed CheckValve() call and put it in the main loop 
}

//Check if the valve is closed
bool CheckValve(){
  if (valve_status == LOW){return true;}
  else {return false; }
  
}

//These functions may need to change slightly
void turnONpump() {
  Serial.println("ON");
  digitalWrite(PUMP_OUT_ENABLE,HIGH); //turn on the pump for water drain
  //we need to test the Relay ON or OFF for opening the valve, in some cases, the relay works reversly: LOW is on, HIGH is Off.

}

void turnOFFpump() {
 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) * 0.0344;
  
  if (distance >= 400 || distance <= 2){
    Serial.print("Distance = ");
    Serial.println("Out of range");
  }
  else {
    if(distance == 25){
       digitalWrite(PUMP_OUT_ENABLE,LOW); // turn off pump
       pump_status=digitalRead(PUMP_OUT_ENABLE);
    }
    //removed check pump statement and put it in main loop
  }
}

//Check if the pump is off
bool CheckPump() {
  if (pump_status == LOW){return true;}
  else {return false; }
}
