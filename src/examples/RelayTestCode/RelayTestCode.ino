//Relay test code
//Setup shown here: https://howtomechatronics.com/tutorials/arduino/control-high-voltage-devices-arduino-relay-tutorial/

//int in1 = 7;

//const int VALVE_IN_ENABLE = 3;
const int PUMP_OUT_ENABLE = 13;
const int PWM = 5;

void setup() {
  Serial.begin (9600);
  //pinMode(VALVE_IN_ENABLE, OUTPUT);
  //pinMode(PUMP_OUT_ENABLE, OUTPUT);
  //digitalWrite(VALVE_IN_ENABLE, LOW);
  digitalWrite(PUMP_OUT_ENABLE,LOW);
  analogWrite(PWM, 0);
  delay(1000);
}
void loop() {
  //digitalWrite(VALVE_IN_ENABLE, HIGH);
  //Serial.println("HIGH VALVE");
  //delay(1000);
  //digitalWrite(VALVE_IN_ENABLE, LOW);
  //Serial.println("LOW Valve");
  //delay(1000);
  digitalWrite(PUMP_OUT_ENABLE, HIGH);
  Serial.println("HIGH pump");
  delay(500);
  digitalWrite(PUMP_OUT_ENABLE,LOW);
  delay(500);
  //digitalWrite(PUMP_OUT_ENABLE, LOW);
  //Serial.println("LOW pump");
  //delay(1000);
}
