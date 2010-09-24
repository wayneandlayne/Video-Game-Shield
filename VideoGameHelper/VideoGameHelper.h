/*
   Video Game Helper library - Version 1.01 - September 20, 2010
   Copyright (c) 2010 Wayne and Layne, LLC

   This library is just a collection of helpful functions and routines that
   might be useful when you're writing video games and other programs for
   the Video Game Shield.

   Written by Matthew Beckler and Adam Wolf of Wayne and Layne, LLC
   http://wayneandlayne.com/projects/video-game-shield/

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

// This function initializes nunchucks, displays your game title, and ensures
// that the proper minimum number of nunchucks have been initialized.
// Eventually, it'll have an optional menu for "1 player", "2 player", and "options".
// Eventually, it'll display your game image/logo if you want.
// Eventually, it'll let you specify a function pointer for menus and such.
// TODO - Show something more graphical for P1/P2 ready to go?
// TODO - Handle detecting a 'lost' nunchuck that gets removed after being ready.
void title_screen_init_nunchucks(TVout* TV, char* title, Nunchuck* player1, Nunchuck* player2, boolean p2_required);

// Display a question box with selectable answers. Make sure default choice is in (0, num_answers]
unsigned char question_box(TVout* TV, Nunchuck* player, char *question, char* answers[], unsigned char num_answers, unsigned char default_choice);

// A wizard to calibrate a nunchuck, and updates the x/y min/max values for the scaling functions.
void nunchuck_calibration(TVout* TV, Nunchuck* player);

// Asks the player to enter their three initials, storing them in buf, 
// which needs to be size 4 or more, to include the '\0' at the end.
// Allows for the entry of A-Z and 0-9.
void high_score_get_name(TVout* TV, Nunchuck* player, char* buf);

// Displays the list of high scores.
// num is the number of high scores to show.
// TODO make this scale up for more scores, might have to scroll?
void high_score_show_list(TVout* TV, Nunchuck* player, char* names[], unsigned long scores[], unsigned char num);

