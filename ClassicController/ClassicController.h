/*
   ClassicController.h - Library for the Nintendo Wii Classic Controller
   Created by: Adam Wolf and Matthew Beckler of Wayne and Layne, LLC
   http://wayneandlayne.com/projects/video-game-shield/
   Recent updates:
    August 11, 2012 - Changed initialization bytes to support knock-off nunchucks
    January 18, 2012 - Arduino 1.0 compatability fixes

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
#ifndef CLASSICCONTROLLER_H
#define CLASSICCONTROLLER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define WII_I2C_ADDR               0xA4
#define WII_PLAYER_1               0
#define WII_PLAYER_2               1
// The VGS board has the nunchuck/cc-select signal line connected to Arduino pin D12.
// On the UNO and compatible (atmega328p et al) D12 is PB4.
// On the MEGA 2560 and compatible (atmega2560 et al) D12 is PB6.
// We use an ifdef here to make our WII_SELECT_P{1,2} macros be correct
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
    #define WII_SELECT_SET_OUTPUT_DIR  DDRB |= 0x40  // set DDRB4 bit = make it an output (used for nunchuck select)
    #define WII_SELECT_P1              PORTB &= 0xBF // set PB6 = low
    #define WII_SELECT_P2              PORTB |= 0x40 // set PB6 = high
#else
    #define WII_SELECT_SET_OUTPUT_DIR  DDRB |= 0x10  // set DDRB4 bit = make it an output (used for nunchuck select)
    #define WII_SELECT_P1              PORTB &= 0xEF // set PB4 = low
    #define WII_SELECT_P2              PORTB |= 0x10 // set PB4 = high
#endif

// These are default values:
#define CLASSICCONTROLLER_JOY_LEFT_MAX          22
#define CLASSICCONTROLLER_JOY_RIGHT_MIN         45
#define CLASSICCONTROLLER_JOY_DOWN_MAX          22
#define CLASSICCONTROLLER_JOY_UP_MIN            45
#define CLASSICCONTROLLER_JOY_CENTER_RADIUS     5

#define CLASSICCONTROLLER_JOY_X_MIN             4
#define CLASSICCONTROLLER_JOY_X_MAX             60
#define CLASSICCONTROLLER_JOY_Y_MIN             4
#define CLASSICCONTROLLER_JOY_Y_MAX             60

#define CLASSICCONTROLLER_JOY_X_SCALED_MIN      0
#define CLASSICCONTROLLER_JOY_X_SCALED_MAX      99
#define CLASSICCONTROLLER_JOY_Y_SCALED_MIN      0
#define CLASSICCONTROLLER_JOY_Y_SCALED_MAX      99


class ClassicController
{
    public:
        // Returns 0=success, 1=could not access device
        // Argument specifies which_nunchuck to initialize,
        //   pass in either WII_PLAYER_1 or WII_PLAYER_2
        unsigned char begin(unsigned char which_controller);

        // Call this function to query the Controller and update the data
        void update();

        // For debugging
        unsigned char* get_data();

        // Buttons
        bool button_a();
        bool button_b();
        bool button_y();
        bool button_x();

        // Top buttons
        bool button_zr();
        bool button_zl();
        bool button_r();
        bool button_l();

        // D-pad buttons
        bool button_up();
        bool button_left();
        bool button_down();
        bool button_right();

        // Middle buttons
        bool button_plus_start();
        bool button_home();
        bool button_minus_select();

        // Left and right joysticks
        unsigned char joy_left_x();
        unsigned char joy_left_y();
        unsigned char joy_right_x();
        unsigned char joy_right_y();

        // Left and right shoulder button pressure
        unsigned char shoulder_left();
        unsigned char shoulder_right();

        // Higher-level functions
        // 1. Up/down/left/right/center boolean joystick functions - Can set a different threshold or use the default threshold values.
        //    For joy_xxxx_center(), you can set the center_radius to give us a deadband between u/d/l/r and center.
        void joy_left_set_threshold(unsigned char left_max, unsigned char right_min, unsigned char down_max, unsigned char up_min, unsigned char center_radius);
        bool joy_left_up();
        bool joy_left_left();
        bool joy_left_down();
        bool joy_left_right();
        bool joy_left_center();

        void joy_right_set_threshold(unsigned char left_max, unsigned char right_min, unsigned char down_max, unsigned char up_min, unsigned char center_radius);
        bool joy_right_up();
        bool joy_right_left();
        bool joy_right_down();
        bool joy_right_right();
        bool joy_right_center();

        // 2. Scaled joystick functions - Can set a different min/max for x and y (like from a calibration function).
        // The scaled() functions return a value bewteen 0 and 99, guaranteed never to be outside of that range.
        // The range defaults to 0-99, but can be updated independently for x and y with the set_scaled_min_max function.
        // Note that you can have a max value smaller than the min value, and it will do the Right Thing and invert the values.
        void joy_left_set_min_max(unsigned char x_min, unsigned char x_max, unsigned char y_min, unsigned char y_max);
        void joy_left_set_scaled_min_max(unsigned char x_scaled_min, unsigned char x_scaled_max, unsigned char y_scaled_min, unsigned char y_scaled_max);
        unsigned char joy_left_x_scaled();
        unsigned char joy_left_y_scaled();

        void joy_right_set_min_max(unsigned char x_min, unsigned char x_max, unsigned char y_min, unsigned char y_max);
        void joy_right_set_scaled_min_max(unsigned char x_scaled_min, unsigned char x_scaled_max, unsigned char y_scaled_min, unsigned char y_scaled_max);
        unsigned char joy_right_x_scaled();
        unsigned char joy_right_y_scaled();

    private:
        // This stores the raw data from the controller, properly translated
        unsigned char data[6]; 

        // Stores a 0 = player 1, 1 = player 2
        // Used to auto-switch between controllers so the user doesn't have to worry about it
        unsigned char this_controller;

        // used for u/d/l/r joystick bool functions
        unsigned char joy_left_left_max;
        unsigned char joy_left_right_min;
        unsigned char joy_left_down_max;
        unsigned char joy_left_up_min;
        unsigned char joy_left_center_radius;
        // these are used for the joy_x/y_scaled() functions
        unsigned char joy_left_x_min;
        unsigned char joy_left_x_max;
        unsigned char joy_left_y_min;
        unsigned char joy_left_y_max;
        // output ranges for scaled values
        unsigned char joy_left_x_scaled_min;
        unsigned char joy_left_x_scaled_max;
        unsigned char joy_left_y_scaled_min;
        unsigned char joy_left_y_scaled_max;

        // used for u/d/l/r joystick bool functions
        unsigned char joy_right_left_max;
        unsigned char joy_right_right_min;
        unsigned char joy_right_down_max;
        unsigned char joy_right_up_min;
        unsigned char joy_right_center_radius;
        // these are used for the joy_x/y_scaled() functions
        unsigned char joy_right_x_min;
        unsigned char joy_right_x_max;
        unsigned char joy_right_y_min;
        unsigned char joy_right_y_max;
        // output ranges for scaled values
        unsigned char joy_right_x_scaled_min;
        unsigned char joy_right_x_scaled_max;
        unsigned char joy_right_y_scaled_min;
        unsigned char joy_right_y_scaled_max;
};

#endif
