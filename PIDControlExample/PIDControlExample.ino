//////////PID///////////
#include <PID_v1.h>

//////////Display///////////
#include <LiquidCrystal_I2C.h>

//////////PID///////////
//Define variable
double Setpoint = 0, Input = 0, Output = 0;
//Define the PID named myPID
PID myPID(&Input, &Output, &Setpoint, 1, 1, 0, DIRECT); //1,1,0 are the Kp, Ki, and Kd values (in that order)

//////////Display///////////
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
unsigned long t3 = 0;
unsigned long update_time = 200;

/////Sensor////
int LDR_Pin = A0;
unsigned long t1, t2;
unsigned long braketime = 300000;
unsigned long dt = 0;
unsigned long rpm = 0;
int wait_time = 2;


/////Driver////
//channels
const int pwm = 9;
const int in_1 = 5;
const int in_2 = 6;
const int led_pin = 13;

//////Poti/////
int Pot_pin = A2;

void setup() {

  lcd.init();                      // initialize the lcd
  lcd.backlight();

  myPID.SetMode(AUTOMATIC);       //turn the PID on, AUTOMATIC means on, MANUAL means off

  Serial.begin(9600);
  
  pinMode(pwm, OUTPUT);           //define channels
  pinMode(in_1, OUTPUT);
  pinMode(in_2, OUTPUT);
  pinMode(led_pin, OUTPUT);

  digitalWrite(led_pin, HIGH);    //turn led on
}

void loop() {

  //Define the Setpoint
  //Setpoint = (sin(3.0 * millis() / (1000.0 * 2 * 3.14)) + 2.0 ) * 500.0;
  //Setpoint = 0;
  Setpoint = analogRead(Pot_pin) * 5.0;

  ////////measure and calculate rpm////////
  /////////////////////////////////////////
  //detecting the time dt which passes between two holes
  
  //1. starting at an unknown state and wait until the photoresistor indicates that the light barrier is blocked
  t2 = micros();                          //t2 indicates the starting time of the loop
  while (analogRead(LDR_Pin) < 850) {     //The loop runs until the LDR signal is lower than the threshold of 850
    if (micros() - t2 > braketime) {      //If the loop runs longer than braketime e.g. the rotor did not move the loop stopped
      break;
    }
  }
  delay(wait_time);                       //this delay time is needed due to the noise in the signal
  //2. wait until the next hole appears and save this time to the variable t1
  t2 = micros();                        
  while (analogRead(LDR_Pin) > 850) {     //The loop runs until the LDR signal is higher than the threshold of 850
    if (micros() - t2 > braketime) {      
      break;
    }
  }
  t1 = micros();                          //save the time when the first hole appear to t1
  delay(wait_time);                       //this delay time is needed due to the noise in the signal
  //3. wait until the hole disappears
  t2 = micros();                        
  while (analogRead(LDR_Pin) < 850) {     //The loop runs until the LDR signal is lower than the trashhold of 850
    if (micros() - t2 > braketime) {      
      break;
    }
  }
  delay(wait_time);                       //this delay time is needed due to the noise in the signal

  //4.wait until the next hole appears
  t2 = micros();                          
  while (analogRead(LDR_Pin) > 850) {     //The loop runs until the LDR signal is higher than the trashhold of 850
    if (micros() - t2 > braketime) {
      break;
    }
  }
  dt = micros() - t1;                     //calculate the time between the two holes
  delay(wait_time);         
  rpm = (1.0 / (dt / 60000000.0)) / 2.0;  //calculate the rpm 

//All the above code may be unnecessary if we can get the hall effect to read speed properly

  //use the PID controller 
  Input = rpm;
  myPID.Compute();
  digitalWrite(in_1, HIGH);               //Defining the turning direktion
  digitalWrite(in_2, LOW);
  analogWrite(pwm, Output);               //apply the Output to the DC motor

  Serial.print(rpm - Setpoint);           
  Serial.print(",");
  Serial.print(rpm);
  Serial.print(",");
  Serial.print(Setpoint);
  Serial.print(",");
  Serial.println(Output);

  //show results at the LCD
  if (millis() - t3 > update_time)
  {
    t3 = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("set rpm");
    lcd.setCursor(8, 0);
    lcd.print(int(Setpoint));
    lcd.setCursor(13, 0);
    lcd.print("U/m");
    lcd.setCursor(0, 1);
    lcd.print("act rpm");
    lcd.setCursor(8, 1);
    lcd.print(int(rpm));
    lcd.setCursor(13, 1);
    lcd.print("U/m");
  }
}
