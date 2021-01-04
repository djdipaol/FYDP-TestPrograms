

// Pump for draining used water; use relay to control
// Code Source:https://github.com/EasyHomeMadeProjects/Water-Level-Monitor/blob/master/water_level_Monitor.ino

const int RELAY_pumpENABLE = 12;  // need to change this number upon our design
int pump_status;

int level_00 = A1; // lowest water level; need to change A1 upon our design
int level_01 = A2; //small load water level; need to change A2 upon our design
int level_02 = A3; //medium load water level; need to change A3 upon our design
int level_03 = A4; //high load water level; need to change A4 upon our design
int a; //lowest water level
int b; //small load water level
int c; //medium load water level
int d; //large load water level
int z=111; // Adjust this value from 100 to 1023 if your circuit do not show correct value. 
            //need to test this, set this number as the reading when the pin contact to water


void setup() {
  pinMode(RELAY_pumpENABLE,OUTPUT);
  Serial.begin(9600); //not sure what is this for, I think this is for relay setting 
  pinMode(level_00,INPUT);
  pinMode(level_01,INPUT);
  pinMode(level_02,INPUT);
  pinMode(level_03,INPUT);

}

void turnONpump() {
  digitalWrite(RELAY_pumpENABLE,HIGH); //turn on the pump for water drain
  //we need to test the Relay ON or OFF for opening the valve, in some cases, the relay works reversly: LOW is on, HIGH is Off.

}

void turnOFFpump() {
 
  a=analogRead(level_00);
  pump_status=digitalRead(RELAY_pumpENABLE);
  
  if(a<z && d<z && c<z && b>z ){
     digitalWrite(RELAY_pumpENABLE,LOW); // turn off pump
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
