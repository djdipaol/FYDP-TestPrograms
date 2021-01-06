
//door safety magnetic sensor
//when magnetic sensor cannot sense the magnet during the middle of cycles (when openning the lid, the motor stops
//when magnetic sensor can sense the magnet (lid is closed again), resume the program
//code source: https://www.youtube.com/watch?v=4eqi0G7uY_4


const int hallSesorPin = 2; // the number of the pushbutton pin
int doorState =0; 

void setup() {
  pinMode(hallSesorPin,INPUT);
}


void loop() {
  doorState = digitalRead(hallSesorPin);
  
  if (CheckLid){
    //if CheckLid returns true, which means the lid is open by user. Then the motor should stop 

    
  }
  
  
  else{
    //CheckLid returns false, which means the lid is close. Then the motor can run. 
  }

}


bool CheckLid(){
  if (doorState == HIGH){return true;}//cannot sense the magnet, lid opens
  else {return false; }//can sense the magnet, lid closed
  
}
