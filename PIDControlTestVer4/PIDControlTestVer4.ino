//This is the preliminary coding design of the entire program.
//Specific sections of it will be tested individually before combining all functions

//////////PID///////////
#include <PID_v1.h>

//Define variable
double Setpoint = 800, Input = 45, Output = 0;
//Define the PID named myPID
int Kp = 1.0; //These values can change based on testing
int Ki = 0;
int Kd = 0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
const int pwm = 12; //PWM pins are pins 2-13
const int dirControl = 22; //This is a digital input used to control the motor direction
//Motor controller should be set to signed magnitude mode

double currSpeed = 0;

unsigned long counter = 0; //used to track time for reversing the motor direction

float hall_thresh = 100.0; //used to track threshold for the tachometer

////////Tachometer Hall Effect////////
const int hallPin = 45;

void setup() {
  // put your setup code here, to run once:
  pinMode(hallPin, INPUT);

  //Set up motor control pins
  pinMode(pwm, OUTPUT);
  pinMode(dirControl, OUTPUT);
  digitalWrite(dirControl, LOW);
  analogWrite(pwm, Input);

  Serial.begin (9600);

  myPID.SetMode(AUTOMATIC);

  Setpoint=800; //set to a fairly low speed
}

void loop() {
  // put your main code here, to run repeatedly:
  //Setpoint = 100; //want our speed to be 800 rpm
  currSpeed=getSpeed();
  if(currSpeed-Setpoint>20)
  {
    Input--;
    analogWrite(pwm, Input);
  }
  else if(Setpoint-currSpeed>20)
  {
    Input++;
    analogWrite(pwm, Input);
  }
  Serial.print("Speed: ");
  Serial.println(currSpeed);
  Serial.println(Input);
  /*if(millis()-counter > 10000) //change direction every 10 seconds
  {
    analogWrite(pwm,0);
    Output = 0; 
    if(digitalRead(dirControl) == LOW)
      digitalWrite(dirControl,HIGH); //change the motor direction
    else if(digitalRead(dirControl) == HIGH)
      digitalWrite(dirControl,LOW); //change the motor direction

    counter = 0; //reset counter
  }*/
}

//Used to get the motor speed. Based on code from MakerPortal
//https://tinyurl.com/y6sfnlmt
double getSpeed()
{
  //lets try something else
  double start = micros();
  double hall_count = 0.0;
  bool on_state = false;
  while(true){
    if(digitalRead(hallPin)==0){
      if(on_state==false){
        on_state = true;
        hall_count+=1.0;
      }
    }
    else{
      on_state=false;
    }

    if(micros()-start>5000000.0){ //check if stopped, may need to reduce time
      break;
    }
  }
  //get information about Time and RPM
  double end_time = micros();
  double time_passed = ((end_time-start)/1000000.0);
  double rpm_val = (hall_count/time_passed)*60.0;
  return rpm_val;
  /*
  // preallocate values for tachometer
  double hall_count = 1.0;
  double start = micros();
  bool on_state = false;
  // counting number of times the hall sensor is tripped
  // but without double counting during the same trip
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
    
    if (hall_count>=hall_thresh){ //threshold may need to change based on tests
      break;
    }
    if(micros()-start>10000000.0){ //check if stopped, may need to reduce time
      break;
    }
  }
  
  //get information about Time and RPM
  double end_time = micros();
  double time_passed = ((end_time-start)/1000000.0);
  double rpm_val = (hall_count/time_passed)*60.0;
  return rpm_val;
  */
}

//Double check this
/*void setPWM(double rpm)
{
  Input = rpm;
  myPID.Compute(); //compute the output voltage from the selected PID constants
  double setting = (Output+146.08)/18.537;
  analogWrite(pwm, setting);
}*/
