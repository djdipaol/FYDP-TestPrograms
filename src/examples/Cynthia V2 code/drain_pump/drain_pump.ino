

// Pump for draining used water; use relay to control
// Code Source:https://www.circuitbasics.com/how-to-set-up-an-ultrasonic-range-finder-on-an-arduino/

const int RELAY_pumpENABLE = 12;  // need to change this number upon our design
int pump_status;

//Ultrasonic Sensor HC-SR04
const int trigPin = 9;// need to change this number upon our design
const int echoPin = 10;// need to change this number upon our design
long duration;
int distance;


void setup() {
  pinMode(RELAY_pumpENABLE,OUTPUT);
  Serial.begin(9600); //not sure what is this for, I think this is for relay setting 
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

}

void turnONpump() {
  digitalWrite(RELAY_pumpENABLE,HIGH); //turn on the pump for water drain
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
    if(distance = 25){
       digitalWrite(RELAY_pumpENABLE,LOW); // turn off pump
       pump_status=digitalRead(RELAY_pumpENABLE);
    }
   
  
    if(CheckPump) // after pump is off, move onto next state
    {
        //call the next state
    }
}


bool CheckPump(){
  if (pump_status == LOW){return true;}
  else {return false; }
  
}
