/*
   Arduino sketch to demonstrate interfacing with a Nintendo Wii Nunchuck. It will print the nunchuck information from P1 every second, to the serial terminal at 9600 baud.
   Created by Adam Wolf and Matthew Beckler
   Wayne and Layne, LLC - http://wayneandlayne.com/projects/video-game-shield/
   Recent updates:
        August 18, 2012 - Changed file extension to .ino for Arduino 1.0

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include <i2cmaster.h>
#include <nunchuck.h>

Nunchuck nunchuck;

void setup() 
{
  nunchuck.begin(NUNCHUCK_PLAYER_1);
  Serial.begin(9600);
}

void loop() 
{
  nunchuck.update();
  Serial.println("Nunchuck values:");
  Serial.println("Joystick:");  
  Serial.print("x: "); Serial.println(nunchuck.joy_x(), DEC);
  Serial.print("y: "); Serial.println(nunchuck.joy_y(), DEC);
  
  Serial.println("Accelerometer:");
  Serial.print("x: "); Serial.println(nunchuck.acc_x(), DEC);
  Serial.print("y: "); Serial.println(nunchuck.acc_y(), DEC);
  Serial.print("z: "); Serial.println(nunchuck.acc_z(), DEC);

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

