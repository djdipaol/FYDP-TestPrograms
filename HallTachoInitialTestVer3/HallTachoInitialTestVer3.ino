//Initial test code for using the hall effect sensor as a tachometer

int hallPin = 2;

// set number of hall trips for RPM reading (higher improves accuracy)
float hall_thresh = 3.0;

int speedPin = 5;
int dirPin = 4;

int value = 60;

bool stopped=true;

void setup() {
  // put your setup code here, to run once:
  pinMode(hallPin, INPUT);
  
  //pinMode(speedPin, OUTPUT); this may be causing the speed error at the start
  pinMode(dirPin, OUTPUT);

  analogWrite(speedPin, 0);
  digitalWrite(dirPin,0);

  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  //Serial.setTimeout(10); // change default (1000ms) to have faster response time
  Serial.println("Running hall effect tachometer test code:");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!stopped)
  {
    analogWrite(speedPin, 0);
    stopped=true;
  }
  if(Serial.available()>0)
  {
    String valInput = Serial.readString();
    Serial.print("I received: ");
    Serial.print(valInput);

    switch(valInput[0]){
      case 'w':
        analogWrite(speedPin, value);
  
  // preallocate values for tach
  float hall_count = 1.0;
  float start = micros();
  bool on_state = false;
  // counting number of times the hall sensor is tripped
  // but without double counting during the same trip
  Serial.println("Point 1");
  while(true){
    if (digitalRead(hallPin)==0){
      if (on_state==false){
        on_state = true;
        hall_count+=1.0;
      }
    } 
    else{
      on_state = false;
    }
    
    if (hall_count>=hall_thresh){
      break; //maybe should change this
    }
  }
  Serial.println("Point 2");
  
  // print information about Time and RPM
  float end_time = micros(); //data type may need to change to unsigned long
  float time_passed = ((end_time-start)/1000000.0);
  Serial.print("Time Passed: ");
  Serial.print(time_passed);
  Serial.println("s");
  float rpm_val = (hall_count/time_passed)*60.0;
  Serial.print(rpm_val);
  Serial.println(" RPM");
  delay(1);        // delay in between reads for stability
  stopped=false;
        break;
      
      default:
        break;
    }
  }
  
  
}
