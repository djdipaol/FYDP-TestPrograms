//Relay test code
//Setup shown here: https://howtomechatronics.com/tutorials/arduino/control-high-voltage-devices-arduino-relay-tutorial/

int in1 = 7;
void setup() {
  Serial.begin (9600);
  pinMode(in1, OUTPUT);
  digitalWrite(in1, HIGH);
}
void loop() {
  digitalWrite(in1, LOW);
  Serial.println("LOW");
  delay(6000);
  //digitalWrite(in1, HIGH);
  //Serial.println("HIGH");
  //delay(3000);
}
