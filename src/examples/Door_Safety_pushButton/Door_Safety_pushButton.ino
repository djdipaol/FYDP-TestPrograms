
//door safety push button
//when the push button is pushed by the lid during the middle of cycles (when openning the lid, the motor stops
//when the push button is released (lid is closed again), resume the program
//code source: https://www.arduino.cc/en/tutorial/button


const int button = 2; // the number of the pushbutton pin
int buttonState =0; 

void setup() {
  pinMode(button,INPUT);

}


void loop() {
  buttonState = digitalRead(button);
  
  if (CheckLid){
    //if CheckLid returns true, which means the lid is open by user. Then the motor should stop 

    
  }
  
  
  else{
    //CheckLid returns false, which means the lid is close. Then the motor can run. 
  }

}


bool CheckLid(){
  if (buttonState == HIGH){return true;}
  else {return false; }
  
}
