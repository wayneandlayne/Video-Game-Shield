/*
   Arduino sketch to demonstrate interfacing with a Nintendo Wii Nunchuck.
   Created by Adam Wolf and Matthew Beckler
   Wayne and Layne, LLC - http://wayneandlayne.com/projects/video-game-shield/
   Last updated: October 31, 2010

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

