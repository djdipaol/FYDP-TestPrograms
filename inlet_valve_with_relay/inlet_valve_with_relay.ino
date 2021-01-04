
// Inlet Solenoid Valve with relay: use relay to open & close valve
// Code Source:https://www.youtube.com/watch?v=ioSYlxHlYdI
// and https://github.com/EasyHomeMadeProjects/Water-Level-Monitor/blob/master/water_level_Monitor.ino

const int RELAY_valveENABLE = 2;  // need to change this number upon our design
int valve_status;

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
  pinMode(RELAY_valveENABLE, OUTPUT);
  Serial.begin(9600); //not sure what is this for, I think this is for relay setting 
  pinMode(level_00,INPUT);
  pinMode(level_01,INPUT);
  pinMode(level_02,INPUT);
  pinMode(level_03,INPUT);

}

void openValve(){
  digitalWrite(RELAY_valveENABLE,HIGH); //open the valve for water inlet
  //we need to test the Relay ON or OFF for opening the valve, in some cases, the relay works reversly: LOW is on, HIGH is Off.
}

void closeValve(){
  b=analogRead(level_01);
  c=analogRead(level_02);
  d=analogRead(level_03);
  valve_status=digitalRead(RELAY_valveENABLE);

if(............){//if large load program selected, run following

  if(d>z && c>z && b>z && a>z){ //large load: when the highest water level pin contacts water
    digitalWrite(RELAY_valveENABLE,LOW); //Close the valve to stop water inlet
  }
}

if(............){//if medium load program selected, run following
  if(d<z && c>z && b>z && a>z){ //medium load: when the second highest water level pin contacts water
    digitalWrite(RELAY_valveENABLE,LOW); //Close the valve to stop water inlet
  }
}


if(............){//if small load program selected, run following
  if(d<z && c<z && b>z && a>z){ //small load: when the third highest water level pin contacts water
    digitalWrite(RELAY_valveENABLE,LOW); //Close the valve to stop water inlet
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
