void setup() 
{
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}
 
#define Neutral 0
#define Press 1
#define Up 2
#define Down 3
#define Right 4
#define Left 5
 
// Check the joystick position
int CheckJoystick()
{
  int joystickState = analogRead(3);
  
  if (joystickState < 50) return Left;
  if (joystickState < 150) return Down;
  if (joystickState < 250) return Press;
  if (joystickState < 500) return Right;
  if (joystickState < 650) return Up;
  return Neutral;
}
 
void loop() 
{
  delay(1000);
  int joy = CheckJoystick();
  switch (joy)
  {
    case Left:
      Serial.println("Left");
      break;
    case Right:
      Serial.println("Right");
      break;
    case Up:
      Serial.println("Up");
      break;
    case Down:
      Serial.println("Down");
      break;
    case Press:
      Serial.println("Press");
      break;
  }
}
