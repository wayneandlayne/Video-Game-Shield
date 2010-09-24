// Nunchuck library demo
// TODO standard header

#include <nunchuck.h>

Nunchuck nunchuck;

void setup() 
{
  nunchuck.begin();
  Serial.begin(9600);
}

void loop() 
{
  nunchuck.update();
  Serial.println("Nunchuck values:");
  Serial.println("Joystick:");  
  Serial.print("x: "); Serial.println(nunchuck.joy_x());
  Serial.print("y: "); Serial.println(nunchuck.joy_y());
  
  Serial.println("Accelerometer:");
  Serial.print("x: "); Serial.println(nunchuck.acc_x());
  Serial.print("y: "); Serial.println(nunchuck.acc_y());
  Serial.print("z: "); Serial.println(nunchuck.acc_z());

  Serial.println("Buttons:");
  
  Serial.print("c: ");
  if (nunchuck.button_c())
  {
    Serial.println("pushed"); 
  }
  else
  {
    Serial.println("not pushed"); 
  }
  
  Serial.print("z: ");
  if (nunchuck.button_z())
  {
        Serial.println("pushed"); 
  }
  else
  {
    Serial.println("not pushed"); 
  }
  
  delay(1000);
}

