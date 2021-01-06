//Motor Controller Test Code

int speedPin = 5;

int dirPin = 4;

int value = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(speedPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  analogWrite(speedPin, value);
  digitalWrite(dirPin,0);

  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  Serial.setTimeout(10); // change default (1000ms) to have faster response time
  Serial.println("Running motor controller test code:");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()>0)
  {
    String valInput = Serial.readString();
    Serial.print("I received: ");
    Serial.print(valInput);

    switch(valInput[0]){
      case 'w':
        value-=5;
        if(value<0)
        {
          value = 0;
        }
        break;

      case 's':
        value+=5;
        if(value>255)
        {
          value = 255;
        }
        break;
      
      case 'd':
        digitalWrite(dirPin,0);
        break;

      case 'a':
        digitalWrite(dirPin,1);
        break;
      
      default:
        break;
    }
    analogWrite(speedPin, value);
    Serial.println(value);
  }
}
