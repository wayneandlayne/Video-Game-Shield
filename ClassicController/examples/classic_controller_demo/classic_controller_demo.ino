/*
   Arduino sketch to demonstrate interfacing with a Nintendo Wii Classic Controller.
   Created by Adam Wolf and Matthew Beckler
   Wayne and Layne, LLC - http://wayneandlayne.com/projects/video-game-shield/
   Recent updates:
        August 18, 2012 - Updated filename extension to .ino for arduino 1.0 compatability
        January 16, 2011 - Initial release

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
   
   Some useful information provided by:
   http://www.arduino.cc/playground/Main/WiiClassicController
*/

#include <i2cmaster.h>
#include <ClassicController.h>

ClassicController cc;

void setup()
{
  Serial.begin(9600);  
  cc.begin(WII_PLAYER_1);
  cc.joy_left_set_scaled_min_max(0, 99, 0, 99);
  cc.joy_right_set_scaled_min_max(0, 99, 0, 99);
}

// The built-in arduino print functions don't have padding options.
// IE: 5 is printed in binary as "101", not "00000101", which I wanted 
// to use here, to debug button locations and joystick values.
// This function prints a byte as a full 8 bits in binary.
void fixed_width_print_bin(byte value)
{
  for (char i = 7; i >= 0; i--)
  {
    if (value & (1 << i))
      Serial.print("1");
    else
      Serial.print("0");
  }
}

void loop()
{
  cc.update();
  
  /*
  // raw binary printing
  byte* data = cc.get_data();
  for (byte i = 0; i < 6; i++)
  {
    fixed_width_print_bin(data[i]);
    Serial.print("  ");
  }
  Serial.println("");
  */
  
  // using accessor functions
  
  Serial.print("Buttons: ");
  
  // Buttons
  if (cc.button_a())
    Serial.print("A ");
  if (cc.button_b())
    Serial.print("B ");
  if (cc.button_x())
    Serial.print("X ");
  if (cc.button_y())
    Serial.print("Y ");
    
  // Top buttons
  if (cc.button_zr())
    Serial.print("ZR ");
  if (cc.button_zl())
    Serial.print("ZL ");
  if (cc.button_r())
    Serial.print("R ");
  if (cc.button_l())
    Serial.print("L ");

  // D-pad buttons
  if (cc.button_up())
    Serial.print("UP ");
  if (cc.button_down())
    Serial.print("DOWN ");
  if (cc.button_left())
    Serial.print("LEFT ");
  if (cc.button_right())
    Serial.print("RIGHT ");
    
  // Middle buttons
  if (cc.button_plus_start())
    Serial.print("START ");
  if (cc.button_home())
    Serial.print("HOME ");
  if (cc.button_minus_select())
    Serial.print("SELECT ");
    
  Serial.println("");
    
  // Left and Right joysticks
  Serial.print("  Left: ");
  Serial.print(cc.joy_left_x(), DEC);
  Serial.print(", ");
  Serial.print(cc.joy_left_y(), DEC);
  Serial.println("");

  Serial.print("  Right: ");
  Serial.print(cc.joy_right_x(), DEC);
  Serial.print(", ");
  Serial.print(cc.joy_right_y(), DEC);
  Serial.println("");
  
  // Left and right shoulder button pressure
  Serial.print("  Left shoulder: ");
  Serial.println(cc.shoulder_left(), DEC);
  Serial.print("  Right shoulder: ");
  Serial.println(cc.shoulder_right(), DEC);
  
  Serial.print("  Left joystick boolean values: ");
  if (cc.joy_left_left())
    Serial.print("left ");
  if (cc.joy_left_right())
    Serial.print("right ");
  if (cc.joy_left_up())
    Serial.print("up ");
  if (cc.joy_left_down())
    Serial.print("down ");
  if (cc.joy_left_center())
    Serial.print("center ");
  Serial.println("");
  
  Serial.print("  Right joystick boolean values: ");
  if (cc.joy_right_left())
    Serial.print("left ");
  if (cc.joy_right_right())
    Serial.print("right ");
  if (cc.joy_right_up())
    Serial.print("up ");
  if (cc.joy_right_down())
    Serial.print("down ");
  if (cc.joy_right_center())
    Serial.print("center ");
  Serial.println("");
  
  Serial.print("  Left joystick scaled to 0-99: (");
  Serial.print(cc.joy_left_x_scaled(), DEC);
  Serial.print(", ");
  Serial.print(cc.joy_left_y_scaled(), DEC);
  Serial.println(")");
  
  Serial.print("  Right joystick scaled to 0-99: (");
  Serial.print(cc.joy_right_x_scaled(), DEC);
  Serial.print(", ");
  Serial.print(cc.joy_right_y_scaled(), DEC);
  Serial.println(")");
    
  Serial.println("");
  
  
  delay(1000);
}


