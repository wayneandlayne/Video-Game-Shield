/*
   ClassicController.cpp - Library for the Nintendo Wii Classic Controller
   Created by Adam Wolf and Matthew Beckler of Wayne and Layne, LLC
   http://wayneandlayne.com/projects/video-game-shield/
   Recent updates:
    August 11, 2012 - Changed initialization bytes to support knock-off nunchucks
    January 18, 2012 - Arduino 1.0 compatability fix

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

   Some information taken from http://www.arduino.cc/playground/Main/WiiClassicController
 */

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "ClassicController.h"

#include <i2cmaster.h>

// returns 0=success, 1=could not access device
// Provide a 0 or 1 to which_controller to specify which player this is
//   0 means "Player 1", 1 means "Player 2"
unsigned char ClassicController::begin(unsigned char which_controller)
{
    // set default values
    joy_left_left_max = CLASSICCONTROLLER_JOY_LEFT_MAX;
    joy_left_right_min = CLASSICCONTROLLER_JOY_RIGHT_MIN;
    joy_left_down_max = CLASSICCONTROLLER_JOY_DOWN_MAX;
    joy_left_up_min = CLASSICCONTROLLER_JOY_UP_MIN;
    joy_left_center_radius = CLASSICCONTROLLER_JOY_CENTER_RADIUS;

    joy_left_x_min = CLASSICCONTROLLER_JOY_X_MIN;
    joy_left_x_max = CLASSICCONTROLLER_JOY_X_MAX;
    joy_left_y_min = CLASSICCONTROLLER_JOY_Y_MIN;
    joy_left_y_max = CLASSICCONTROLLER_JOY_Y_MAX;

    joy_left_x_scaled_min = CLASSICCONTROLLER_JOY_X_SCALED_MIN;
    joy_left_x_scaled_max = CLASSICCONTROLLER_JOY_X_SCALED_MAX;
    joy_left_y_scaled_min = CLASSICCONTROLLER_JOY_Y_SCALED_MIN;
    joy_left_y_scaled_max = CLASSICCONTROLLER_JOY_Y_SCALED_MAX;

    // set default values
    joy_right_left_max = CLASSICCONTROLLER_JOY_LEFT_MAX;
    joy_right_right_min = CLASSICCONTROLLER_JOY_RIGHT_MIN;
    joy_right_down_max = CLASSICCONTROLLER_JOY_DOWN_MAX;
    joy_right_up_min = CLASSICCONTROLLER_JOY_UP_MIN;
    joy_right_center_radius = CLASSICCONTROLLER_JOY_CENTER_RADIUS;

    joy_right_x_min = CLASSICCONTROLLER_JOY_X_MIN;
    joy_right_x_max = CLASSICCONTROLLER_JOY_X_MAX;
    joy_right_y_min = CLASSICCONTROLLER_JOY_Y_MIN;
    joy_right_y_max = CLASSICCONTROLLER_JOY_Y_MAX;

    joy_right_x_scaled_min = CLASSICCONTROLLER_JOY_X_SCALED_MIN;
    joy_right_x_scaled_max = CLASSICCONTROLLER_JOY_X_SCALED_MAX;
    joy_right_y_scaled_min = CLASSICCONTROLLER_JOY_Y_SCALED_MIN;
    joy_right_y_scaled_max = CLASSICCONTROLLER_JOY_Y_SCALED_MAX;


    WII_SELECT_SET_OUTPUT_DIR;
    this_controller = which_controller;

    if (this_controller == WII_PLAYER_2)
        WII_SELECT_P2;
    else
        WII_SELECT_P1;

    i2cmaster::i2c_init(); // this is non-blocking, it just sets up the baud rate generator and internal pullups
    if (i2cmaster::i2c_start_wait(WII_I2C_ADDR + I2C_WRITE, 10) == 1)
    {
        // could not access device
        return 1;
    }

    // this new init bytes come from this arduino.cc forum post:
    // http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1264805255/4#4
    i2cmaster::i2c_write(0xF0);
    i2cmaster::i2c_write(0x55);
    i2cmaster::i2c_stop();
    delay(1);

    i2cmaster::i2c_start_wait(WII_I2C_ADDR + I2C_WRITE, 10);
    i2cmaster::i2c_write(0xFB);
    i2cmaster::i2c_write(0x00);
    i2cmaster::i2c_stop();
    delay(1);

    return 0;
}

void ClassicController::update()
{
    if (this_controller == WII_PLAYER_2)
        WII_SELECT_P2;
    else
        WII_SELECT_P1;

    i2cmaster::i2c_start_wait(WII_I2C_ADDR + I2C_WRITE, 0xFFFF);
    i2cmaster::i2c_write(0x00);
    i2cmaster::i2c_stop();

    delayMicroseconds(1000); // TODO is this needed?

    i2cmaster::i2c_start_wait(WII_I2C_ADDR + I2C_READ, 0xFFFF);
    for (unsigned char i = 0; i < 5; i++)
    {
        data[i] = i2cmaster::i2c_readAck();
    }
    data[5] = i2cmaster::i2c_readNak();
    i2cmaster::i2c_stop();
}

unsigned char* ClassicController::get_data()
{
    return data;
}

#define check_button(byte, bit) !((data[(byte)] >> (bit)) & 1)

// Buttons
bool ClassicController::button_a()
{
    return check_button(5, 4);
}
bool ClassicController::button_b()
{
    return check_button(5, 6);
}
bool ClassicController::button_y()
{
    return check_button(5, 5);
}
bool ClassicController::button_x()
{
    return check_button(5, 3);
}

// Top buttons
bool ClassicController::button_zr()
{
    return check_button(5, 2);
}
bool ClassicController::button_zl()
{
    return check_button(5, 7);
}
bool ClassicController::button_r()
{
    return check_button(4, 1);
}
bool ClassicController::button_l()
{
    return check_button(4, 5);
}

// D-pad buttons
bool ClassicController::button_up()
{
    return check_button(5, 0);
}
bool ClassicController::button_left()
{
    return check_button(5, 1);
}
bool ClassicController::button_down()
{
    return check_button(4, 6);
}
bool ClassicController::button_right()
{
    return check_button(4, 7);
}

// Middle buttons
bool ClassicController::button_plus_start()
{
    return check_button(4, 2);
}
bool ClassicController::button_home()
{
    return check_button(4, 3);
}
bool ClassicController::button_minus_select()
{
    return check_button(4, 4);
}

// Left and right joysticks
unsigned char ClassicController::joy_left_x()
{
    return (data[0] & 0x3F);
}
unsigned char ClassicController::joy_left_y()
{
    return (data[1] & 0x3F);
}
unsigned char ClassicController::joy_right_x()
{
    return ((data[0] & 0xC0) >> 2) + 
           ((data[1] & 0xC0) >> 3) + 
           ((data[2] & 0x80) >> 6);
}
unsigned char ClassicController::joy_right_y()
{
    return (data[2] & 0x1F) << 1;
}

// Left and right shoulder button pressure
unsigned char ClassicController::shoulder_left()
{
    return ((data[2] & 0x60) >> 2) + 
           ((data[3] & 0xE0) >> 5);
}
unsigned char ClassicController::shoulder_right()
{
    return (data[3] & 0x1F);
}

// higher level functions
void ClassicController::joy_left_set_threshold(unsigned char left_max, unsigned char right_min, unsigned char down_max, unsigned char up_min, unsigned char center_radius)
{
    joy_left_left_max = left_max;
    joy_left_right_min = right_min;
    joy_left_down_max = down_max;
    joy_left_up_min = up_min;
    joy_left_center_radius = center_radius;
}
bool ClassicController::joy_left_left()
{
    return joy_left_x() <= joy_left_left_max;
}
bool ClassicController::joy_left_right()
{
    return joy_left_x() >= joy_left_right_min;
}
bool ClassicController::joy_left_down()
{
    return joy_left_y() <= joy_left_down_max;
}
bool ClassicController::joy_left_up()
{
    return joy_left_y() >= joy_left_up_min;
}
bool ClassicController::joy_left_center()
{
    return ( (joy_left_x() >= 32 - joy_left_center_radius) &&
             (joy_left_x() <= 32 + joy_left_center_radius) &&
             (joy_left_y() >= 32 - joy_left_center_radius) &&
             (joy_left_y() <= 32 + joy_left_center_radius) );
}
void ClassicController::joy_right_set_threshold(unsigned char left_max, unsigned char right_min, unsigned char down_max, unsigned char up_min, unsigned char center_radius)
{
    joy_right_left_max = left_max;
    joy_right_right_min = right_min;
    joy_right_down_max = down_max;
    joy_right_up_min = up_min;
    joy_right_center_radius = center_radius;
}
bool ClassicController::joy_right_left()
{
    return joy_right_x() <= joy_right_left_max;
}
bool ClassicController::joy_right_right()
{
    return joy_right_x() >= joy_right_right_min;
}
bool ClassicController::joy_right_down()
{
    return joy_right_y() <= joy_right_down_max;
}
bool ClassicController::joy_right_up()
{
    return joy_right_y() >= joy_right_up_min;
}
bool ClassicController::joy_right_center()
{
    return ( (joy_right_x() >= 32 - joy_right_center_radius) &&
             (joy_right_x() <= 32 + joy_right_center_radius) &&
             (joy_right_y() >= 32 - joy_right_center_radius) &&
             (joy_right_y() <= 32 + joy_right_center_radius) );
}

// scaled joystick functions
void ClassicController::joy_left_set_min_max(unsigned char x_min, unsigned char x_max, unsigned char y_min, unsigned char y_max)
{
    joy_left_x_min = x_min;
    joy_left_x_max = x_max;
    joy_left_y_min = y_min;
    joy_left_y_max = y_max;
}
void ClassicController::joy_left_set_scaled_min_max(unsigned char x_scaled_min, unsigned char x_scaled_max, unsigned char y_scaled_min, unsigned char y_scaled_max)
{
    joy_left_x_scaled_min = x_scaled_min;
    joy_left_x_scaled_max = x_scaled_max;
    joy_left_y_scaled_min = y_scaled_min;
    joy_left_y_scaled_max = y_scaled_max;
}
unsigned char ClassicController::joy_left_x_scaled()
{
    return map(constrain(joy_left_x(), joy_left_x_min, joy_left_x_max), joy_left_x_min, joy_left_x_max, joy_left_x_scaled_min, joy_left_x_scaled_max);
}
unsigned char ClassicController::joy_left_y_scaled()
{
    return map(constrain(joy_left_y(), joy_left_y_min, joy_left_y_max), joy_left_y_min, joy_left_y_max, joy_left_y_scaled_min, joy_left_y_scaled_max);
}

void ClassicController::joy_right_set_min_max(unsigned char x_min, unsigned char x_max, unsigned char y_min, unsigned char y_max)
{
    joy_right_x_min = x_min;
    joy_right_x_max = x_max;
    joy_right_y_min = y_min;
    joy_right_y_max = y_max;
}
void ClassicController::joy_right_set_scaled_min_max(unsigned char x_scaled_min, unsigned char x_scaled_max, unsigned char y_scaled_min, unsigned char y_scaled_max)
{
    joy_right_x_scaled_min = x_scaled_min;
    joy_right_x_scaled_max = x_scaled_max;
    joy_right_y_scaled_min = y_scaled_min;
    joy_right_y_scaled_max = y_scaled_max;
}
unsigned char ClassicController::joy_right_x_scaled()
{
    return map(constrain(joy_right_x(), joy_right_x_min, joy_right_x_max), joy_right_x_min, joy_right_x_max, joy_right_x_scaled_min, joy_right_x_scaled_max);
}
unsigned char ClassicController::joy_right_y_scaled()
{
    return map(constrain(joy_right_y(), joy_right_y_min, joy_right_y_max), joy_right_y_min, joy_right_y_max, joy_right_y_scaled_min, joy_right_y_scaled_max);
}

