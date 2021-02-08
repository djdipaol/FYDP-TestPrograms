//Hall effect sensor test code

int input = 7;


void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
  pinMode(input, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(input)==HIGH){
    Serial.println("No Magnet");
  }
  else{
    Serial.println("Magnet");
  }
}
