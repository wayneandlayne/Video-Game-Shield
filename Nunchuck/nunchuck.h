/*
   Nunchuck.h - Library for the Nintendo Wii Nunchuck
   Created by: Adam Wolf and Matthew Beckler, Wayne and Layne, LLC
   Last updated: August 16, 2010

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
#ifndef Nunchuck_h
#define Nunchuck_h

#include "WProgram.h" // TODO is this needed?

#define NUNCHUCK_I2C_ADDR               0xA4

// these are default values:
#define NUNCHUCK_JOY_LEFT_MAX           50
#define NUNCHUCK_JOY_RIGHT_MIN          200
#define NUNCHUCK_JOY_DOWN_MAX           50
#define NUNCHUCK_JOY_UP_MIN             200
#define NUNCHUCK_JOY_CENTER_RADIUS      30

#define NUNCHUCK_JOY_X_MIN              25
#define NUNCHUCK_JOY_X_MAX              230
#define NUNCHUCK_JOY_Y_MIN              28
#define NUNCHUCK_JOY_Y_MAX              226

#define NUNCHUCK_JOY_X_SCALED_MIN      0
#define NUNCHUCK_JOY_X_SCALED_MAX      100
#define NUNCHUCK_JOY_Y_SCALED_MIN      0
#define NUNCHUCK_JOY_Y_SCALED_MAX      100

#define NUNCHUCK_SELECT_P1 PORTB &= 0xEF // set PB4 = low
#define NUNCHUCK_SELECT_P2 PORTB |= 0x10 // set PB4 = high
#define NUNCHUCK_PLAYER_1 0
#define NUNCHUCK_PLAYER_2 1

class Nunchuck
{
    public:
        // returns 0=success, 1=could not access device
        // Provide a 0 or 1 to which_nunchuck to specify which player this is
        //   0 means "Player 1", 1 means "Player 2"
        unsigned char begin(unsigned char which_nunchuck);

        // Call this function to query the Nunchuck and update the data
        void update();

        // low level functions
        unsigned char joy_x();
        unsigned char joy_y();
        unsigned char acc_x();
        unsigned char acc_y();
        unsigned char acc_z();
        bool button_c();
        bool button_z();

        // Higher level functions
        // 1. Up/Down/Left/Right/Center joystick functions - Can set a different threshold, or use the default one.
        //    For joy_center(), you can set the center_radius to give us a deadband between u/d/l/r and center.
        void joy_set_threshold(unsigned char left_max, unsigned char right_min, unsigned char down_max, unsigned char up_min, unsigned char center_radius);
        bool joy_up();
        bool joy_down();
        bool joy_left();
        bool joy_right();
        bool joy_center();
        // 2. Scaled joystick functions - Can set a different min/max for x/y (like from a calibration function).
        // The scaled() functions return a value between 0 and 99, guaranteed never to be outside of that range.
        // It defaults to 0-100, but can be updated in both x and y directions with joy_set_scaled_min_max.
        // Note that you can have a max value smaller than the min value for the scaled min/max function,
        // and that it'll do the Right Thing and invert your values.
        void joy_set_min_max(unsigned char x_min, unsigned char x_max, unsigned char y_min, unsigned char y_max);
        void joy_set_scaled_min_max(unsigned char x_scaled_min, unsigned char x_scaled_max, unsigned char y_scaled_min, unsigned char y_scaled_max);
        unsigned char joy_x_scaled();
        unsigned char joy_y_scaled();

    private:
        // This stores the raw data from the nunchuck, properly translated
        unsigned char nunchuck_buf[6]; 

        // Stores a 0 = player 1, 1 = player 2
        // Used to auto-switch between nunchucks so the user doesn't have to worry about it
        unsigned char this_nunchuck;

        // used for u/d/l/r joystick bool functions
        unsigned char joy_left_max;
        unsigned char joy_right_min;
        unsigned char joy_down_max;
        unsigned char joy_up_min;
        unsigned char joy_center_radius;
        // these are used for the joy_x/y_scaled() functions
        unsigned char joy_x_min;
        unsigned char joy_x_max;
        unsigned char joy_y_min;
        unsigned char joy_y_max;
        // output ranges for scaled values
        unsigned char joy_x_scaled_min;
        unsigned char joy_x_scaled_max;
        unsigned char joy_y_scaled_min;
        unsigned char joy_y_scaled_max;
};

unsigned char decode_byte(unsigned char);

#endif
