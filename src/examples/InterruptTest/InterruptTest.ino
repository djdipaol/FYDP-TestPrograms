//Reference pages:
//https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
//https://www.allaboutcircuits.com/technical-articles/using-interrupts-on-arduino/

//Some concept code for using an interupt to measure speed without needing a loop
//arrays used to calculated moving average
double speeds[5] = {0,0,0,0,0};
double avgSpeed = 0;

//counters for arrays
int speedCount=0;

//counting variable that will be used in the interrupt function
volatile int count = 0;

//tachometer hall effect pin number
const byte tacho_pin = 2;//50;

//PWM pin
const int PWM = 5;

//Speed setting
int rpm = 60;

//time variables
unsigned long prevTime = 0;
unsigned long currTime = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(tacho_pin,INPUT);
  attachInterrupt(digitalPinToInterrupt(tacho_pin), rotation, CHANGE);
  pinMode(PWM, OUTPUT);
  analogWrite(PWM,rpm);
  prevTime=micros();
}

void loop() {
  // put your main code here, to run repeatedly:
  currTime=micros();

  speeds[speedCount]=count/(currTime-prevTime);

  prevTime=currTime;

  count = 0;

  speedCount = (speedCount+1)%5;

  avgSpeed = (speeds[0]+speeds[1]+speeds[2]+speeds[3]+speeds[4])/5;
  
  Serial.println("Average Speed: "+ (String)avgSpeed);
}

void rotation() {
  count++;
}
