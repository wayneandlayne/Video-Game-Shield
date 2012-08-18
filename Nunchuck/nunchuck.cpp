/*
   Nunchuck.cpp - Library for Nintendo Wii Nunchuck
   Created by Adam Wolf and Matthew Beckler (Wayne and Layne, LLC)
   Recent updates:
    August 18, 2012 - Added support for the Arduino Mega, which has a different RBx pin for the nunchuck select.
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

 */

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "nunchuck.h"

#include <i2cmaster.h>

// returns 0=success, 1=could not access device
// Provide a 0 or 1 to which_nunchuck to specify which player this is
//   0 means "Player 1", 1 means "Player 2"
unsigned char Nunchuck::begin(unsigned char which_nunchuck)
{
    // set default values
    joy_left_max = NUNCHUCK_JOY_LEFT_MAX;
    joy_right_min = NUNCHUCK_JOY_RIGHT_MIN;
    joy_down_max = NUNCHUCK_JOY_DOWN_MAX;
    joy_up_min = NUNCHUCK_JOY_UP_MIN;
    joy_center_radius = NUNCHUCK_JOY_CENTER_RADIUS;

    joy_x_min = NUNCHUCK_JOY_X_MIN;
    joy_x_max = NUNCHUCK_JOY_X_MAX;
    joy_y_min = NUNCHUCK_JOY_Y_MIN;
    joy_y_max = NUNCHUCK_JOY_Y_MAX;

    joy_x_scaled_min = NUNCHUCK_JOY_X_SCALED_MIN;
    joy_x_scaled_max = NUNCHUCK_JOY_X_SCALED_MAX;
    joy_y_scaled_min = NUNCHUCK_JOY_Y_SCALED_MIN;
    joy_y_scaled_max = NUNCHUCK_JOY_Y_SCALED_MAX;

    NUNCHUCK_SELECT_SET_OUTPUT_DIR;
    this_nunchuck = which_nunchuck;

    if (this_nunchuck == NUNCHUCK_PLAYER_2)
        NUNCHUCK_SELECT_P2;
    else
        NUNCHUCK_SELECT_P1;

    i2cmaster::i2c_init(); // this is non-blocking, it just sets up the baud rate generator and internal pullups

    if (i2cmaster::i2c_start_wait(NUNCHUCK_I2C_ADDR + I2C_WRITE, 10) == 1)
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

    i2cmaster::i2c_start_wait(NUNCHUCK_I2C_ADDR + I2C_WRITE, 10);
    i2cmaster::i2c_write(0xFB);
    i2cmaster::i2c_write(0x00);
    i2cmaster::i2c_stop();
    delay(1);

    return 0;
}

void Nunchuck::update()
{
    if (this_nunchuck == NUNCHUCK_PLAYER_2)
        NUNCHUCK_SELECT_P2;
    else
        NUNCHUCK_SELECT_P1;

    i2cmaster::i2c_start_wait(NUNCHUCK_I2C_ADDR + I2C_WRITE, 0xFFFF);
    i2cmaster::i2c_write(0x00);
    i2cmaster::i2c_stop();

    delayMicroseconds(1000); // TODO is this needed?

    i2cmaster::i2c_start_wait(NUNCHUCK_I2C_ADDR + I2C_READ, 0xFFFF);
    for (unsigned char i = 0; i < 5; i++)
    {
        nunchuck_buf[i] = i2cmaster::i2c_readAck();
    }
    nunchuck_buf[5] = i2cmaster::i2c_readNak();
    i2cmaster::i2c_stop();
}

bool Nunchuck::button_z()
{
    return !((nunchuck_buf[5] >> 0) & 1);
}

bool Nunchuck::button_c()
{
    return !((nunchuck_buf[5] >> 1) & 1);
}

unsigned char Nunchuck::joy_x()
{
    return nunchuck_buf[0]; //This leaves out 2 bits of the data.
}
unsigned char Nunchuck::joy_y()
{
    return nunchuck_buf[1]; //This leaves out 2 bits of the data.
}

unsigned char Nunchuck::acc_x()
{
    return nunchuck_buf[2]; //This leaves out 2 bits of the data.
}
unsigned char Nunchuck::acc_y()
{
    return nunchuck_buf[3]; //This leaves out 2 bits of the data.
}

unsigned char Nunchuck::acc_z()
{
    return nunchuck_buf[4]; //This leaves out 2 bits of the data.
}

// higher level functions
void Nunchuck::joy_set_threshold(unsigned char left_max, unsigned char right_min, unsigned char down_max, unsigned char up_min, unsigned char center_radius)
{
    joy_left_max = left_max;
    joy_right_min = right_min;
    joy_down_max = down_max;
    joy_up_min = up_min;
    joy_center_radius = center_radius;
}

bool Nunchuck::joy_left()
{
    return nunchuck_buf[0] <= joy_left_max;
}

bool Nunchuck::joy_right()
{
    return nunchuck_buf[0] >= joy_right_min;
}

bool Nunchuck::joy_down()
{
    return nunchuck_buf[1] <= joy_down_max;
}

bool Nunchuck::joy_up()
{
    return nunchuck_buf[1] >= joy_up_min;
}

bool Nunchuck::joy_center()
{
    return ( (nunchuck_buf[0] >= 127 - joy_center_radius) &&
             (nunchuck_buf[0] <= 127 + joy_center_radius) &&
             (nunchuck_buf[1] >= 127 - joy_center_radius) &&
             (nunchuck_buf[1] <= 127 + joy_center_radius) );
}

// scaled joystick functions
void Nunchuck::joy_set_min_max(unsigned char x_min, unsigned char x_max, unsigned char y_min, unsigned char y_max)
{
    joy_x_min = x_min;
    joy_x_max = x_max;
    joy_y_min = y_min;
    joy_y_max = y_max;
}

void Nunchuck::joy_set_scaled_min_max(unsigned char x_scaled_min, unsigned char x_scaled_max, unsigned char y_scaled_min, unsigned char y_scaled_max)
{
    joy_x_scaled_min = x_scaled_min;
    joy_x_scaled_max = x_scaled_max;
    joy_y_scaled_min = y_scaled_min;
    joy_y_scaled_max = y_scaled_max;
}

unsigned char Nunchuck::joy_x_scaled()
{
    return map(constrain(nunchuck_buf[0], joy_x_min, joy_x_max), joy_x_min, joy_x_max, joy_x_scaled_min, joy_x_scaled_max);
}

unsigned char Nunchuck::joy_y_scaled()
{
    return map(constrain(nunchuck_buf[1], joy_y_min, joy_y_max), joy_y_min, joy_y_max, joy_y_scaled_min, joy_y_scaled_max);
}

