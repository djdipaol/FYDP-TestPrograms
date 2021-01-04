
// Inlet Solenoid Valve with relay: use relay to open & close valve
// Code Source:https://www.youtube.com/watch?v=ioSYlxHlYdI
// and https://www.circuitbasics.com/how-to-set-up-an-ultrasonic-range-finder-on-an-arduino/

//Relay Module
const int RELAY_valveENABLE = 2;  // need to change this number upon our design
int valve_status;

//Ultrasonic Sensor HC-SR04
const int trigPin = 9;// need to change this number upon our design
const int echoPin = 10;// need to change this number upon our design
long duration;
int distance;


void setup() {
  pinMode(RELAY_valveENABLE, OUTPUT);
  Serial.begin(9600); 
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
}

void WaterLevel(){

 
}

void openValve(){
  digitalWrite(RELAY_valveENABLE,HIGH); //open the valve for water inlet
  //we need to test the Relay ON or OFF for opening the valve, in some cases, the relay works reversly: LOW is on, HIGH is Off.
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

    if(...........){//if large load program selected, run following
      if(distance = 13.2){
        digitalWrite(RELAY_valveENABLE,LOW); //Close the valve to stop water inlet
          valve_status=digitalRead(RELAY_valveENABLE);
      }
    }
    
    else if(..........){//if medium load program selected, run following
        if(distance = 17.2){
        digitalWrite(RELAY_valveENABLE,LOW); //Close the valve to stop water inlet
          valve_status=digitalRead(RELAY_valveENABLE);
        }
    }
    
    
    else if(.............){//if small load program selected, run following
      if(distance = 21){
        digitalWrite(RELAY_valveENABLE,LOW); //Close the valve to stop water inlet
          valve_status=digitalRead(RELAY_valveENABLE);
      }
    }
  
  }
 
  
  if(CheckValve) // after valve is off, move onto next state
  {
      //call the next state
  }
  
  
}


bool CheckValve(){
  if (valve_status == LOW){return true;}
  else {return false; }
  
}
