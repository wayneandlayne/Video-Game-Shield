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

#include <nunchuck.h>
#include <TVout.h>
#include <avr/pgmspace.h> // for PSTR()

// This function initializes nunchucks, displays your game title, and ensures
// that the proper minimum number of nunchucks have been initialized.
// Eventually, it'll have an optional menu for "1 player", "2 player", and "options".
// Eventually, it'll display your game image/logo if you want.
// Eventually, it'll let you specify a function pointer for menus and such.
// TODO - Show something more graphical for P1/P2 ready to go?
// TODO - Handle detecting a 'lost' nunchuck that gets removed after being ready.
void title_screen_init_nunchucks(TVout* TV, char* title, Nunchuck* player1, Nunchuck* player2, boolean p2_required)
{
  TV->clear_screen();
  
  TV->print(0, 0, title);
  // TODO draw an image somewhere?
  TV->draw_line(0, 15, TV->hres()-1, 15, 1);
  if (p2_required)
    TV->printPGM(0, 20, PSTR("Connect P1 & P2"));
  else
    TV->printPGM(0, 20, PSTR("Connect P1"));
  
  boolean p1_ready = false;
  boolean p2_ready = false;
  boolean ready = false;
  
  while (1)
  {
    if (ready)
    {
      player1->update();
      if (player1->button_c())
        break;
    }
    
    if (!p1_ready)
    {
      if (player1->begin(NUNCHUCK_PLAYER_1) == 0)
      {
        p1_ready = true;
        TV->printPGM(0, 30, PSTR("P1 ready"));
        if ( (p1_ready && !p2_required) || (p1_ready && p2_ready) )
        {
          ready = true;
          TV->printPGM(0, 60, PSTR("Press C to Start"));
        }
      }
      // else timeout, not connected yet
    }
    
    if (!p2_ready)
    {
      if (player2->begin(NUNCHUCK_PLAYER_2) == 0)
      {
        p2_ready = true;
        TV->printPGM(0, 40, PSTR("P2 ready"));
        if ( (p1_ready && !p2_required) || (p1_ready && p2_ready) )
        {
          ready = true;
          TV->printPGM(0, 60, PSTR("Press C to Start"));
        }
      }
      // else timeout, not connected yet
    }
      
    TV->delay_frame(10);
  }
}

// Display a question box with selectable answers. Make sure default choice is in (0, num_answers]
unsigned char question_box(TVout* TV, Nunchuck* player, char* question, char* answers[], unsigned char num_answers, unsigned char default_choice)
{
    // returns the choice, 0 through num_answers-1
    TV->clear_screen();

    TV->print(0, 0, question);
    TV->draw_line(0, 15, TV->hres() - 1, 15, 1);
    for (unsigned char i = 0; i < num_answers; i++)
        TV->print(10, 20 + i * 10, answers[i]);

    unsigned char choice = default_choice;
    //TV->draw_box(3, 10*choice+23, 4, 4, 1, 1, 0, 0); // draw selection box
    TV->draw_rect(3, 10*choice+20, 4, 4, 1, 1); // draw selection box
    while (1)
    {
        TV->delay_frame(10);

        player->update();
        if (player->joy_up())
        {
            //TV->draw_box(3, 10*choice+23, 4, 4, 0, 0, 0, 0); // remove selection box
            TV->draw_rect(3, 10*choice+20, 4, 4, 0, 0); // remove selection box
            if (choice == 0)
                choice = num_answers - 1;
            else
                choice--;
	    //TV->draw_box(3, 10*choice+23, 4, 4, 1, 1, 0, 0); // draw selection box
            TV->draw_rect(3, 10*choice+20, 4, 4, 1, 1); // draw selection box
        }
        if (player->joy_down())
        {
           // TV->draw_box(3, 10*choice+23, 4, 4, 0, 0, 0, 0); // remove selection box
            TV->draw_rect(3, 10*choice+20, 4, 4, 0, 0); // remove selection box
            choice = (choice + 1) % num_answers;
            //TV->draw_box(3, 10*choice+23, 4, 4, 1, 1, 0, 0); // draw selection box
            TV->draw_rect(3, 10*choice+20, 4, 4, 1, 1); // draw selection box
        }

        if (player->button_c() || player->button_z()) // TODO what to do here for buttons?
        {
            break;
        }
    }

    return choice;
}

// A wizard to calibrate a nunchuck, and updates the x/y min/max values for the scaling functions.
void nunchuck_calibration(TVout* TV, Nunchuck* player)
{
    TV->clear_screen();

    TV->printPGM(0, 0, PSTR("Nunchuck Calibration"));
    TV->draw_line(0, 15, TV->hres() - 1, 15, 1);
    TV->printPGM(0, 20, PSTR("Move joystick"));
    TV->printPGM(0, 30, PSTR("in all directions"));
    TV->printPGM(0, 50, PSTR("Press C when ready"));
    while (1)
    {
        player->update();
        if (player->button_c())
        {
            break;
        }
    }
    // wait for release
    while (1)
    {
        player->update();
        if (!player->button_c())
            break;
    }

    TV->clear_screen();
    TV->printPGM(0, 0, PSTR("Press C to continue"));
    TV->draw_line(0, 15, TV->hres() - 1, 15, 1);
    unsigned char vert_left = TV->vres() - 16;
    unsigned char x_min = 255;
    unsigned char x_max = 0;
    unsigned char y_min = 255;
    unsigned char y_max = 0;
    unsigned char x, y;
    char buf[4];

    TV->printPGM(vert_left, 16, PSTR("x:"));
    TV->printPGM(vert_left, 56, PSTR("y:"));

    while(1)
    {
        player->update();
        if (player->button_c())
        {
            break;
        }
        x = player->joy_x();
        y = player->joy_y();
        TV->set_pixel(map(x, 0, 255, 0, vert_left), map(y, 0, 255, vert_left, 16), 1);
        // note that the y is being inverted above

        if (x < x_min)
        {
            x_min = x;
            itoa(x_min, buf, 10);
            //TV->draw_box(vert_left, 26, TV->hres() - vert_left, 10, 0, 0, 0, 1);
            TV->draw_rect(vert_left, 26, TV->hres() - vert_left, 8, 0, 0);
            TV->print(vert_left, 26, buf);
        }
        if (x > x_max)
        {
            x_max = x;
            itoa(x_max, buf, 10);
            //TV->draw_box(vert_left, 36, TV->hres() - vert_left, 10, 0, 0, 0, 1);
            TV->draw_rect(vert_left, 36, TV->hres() - vert_left, 8, 0, 0);
            TV->print(vert_left, 36, buf);
        }
        if (y < y_min)
        {
            y_min = y;
            itoa(y_min, buf, 10);
            //TV->draw_box(vert_left, 66, TV->hres() - vert_left, 10, 0, 0, 0, 1);
            TV->draw_rect(vert_left, 66, TV->hres() - vert_left, 8, 0, 0);
            TV->print(vert_left, 66, buf);
        }
        if (y > y_max)
        {
            y_max = y;
            itoa(y_max, buf, 10);
            //TV->draw_box(vert_left, 76, TV->hres() - vert_left, 10, 0, 0, 0, 1);
            TV->draw_rect(vert_left, 76, TV->hres() - vert_left, 8, 0, 0);
            TV->print(vert_left, 76, buf);
        }
        TV->delay_frame(1); // TODO needed?
    }

    // finished calibrating, update min/max values
    player->joy_set_min_max(x_min, x_max, y_min, y_max);
}

// Asks the player to enter their three initials, storing them in buf, 
// which needs to be size 4 or more, to include the '\0' at the end.
// Allows for the entry of A-Z and 0-9.
void high_score_get_name(TVout* TV, Nunchuck* player, char* buf)
{
    TV->clear_screen();

    TV->printPGM(0, 0, PSTR("New high score!"));
    TV->draw_line(0, 15, TV->hres() - 1, 15, 1);
    TV->printPGM(0, 20, PSTR("Pick initials:"));
    TV->printPGM(0, 70, PSTR("Press C to cont."));

    unsigned char which_digit = 0;
    TV->print_char(30 + which_digit * 8, 50, 24);

    buf[0] = 'A';
    buf[1] = 'A';
    buf[2] = 'A';
    buf[3] = '\0';
    TV->print(30, 40, buf);

    while (1)
    {
        player->update();
        if (player->joy_right())
        {
            TV->print_char(30 + which_digit * 8, 50, 0);
            if (which_digit == 2) which_digit = 0;
            else which_digit++;
            TV->print_char(30 + which_digit * 8, 50, 24);
        }
        if (player->joy_left())
        {
            TV->print_char(30 + which_digit * 8, 50, 0);
            if (which_digit == 0) which_digit = 2;
            else which_digit--;
            TV->print_char(30 + which_digit * 8, 50, 24);
        }
        if (player->joy_up())
        {
            if (buf[which_digit] == 'A')  buf[which_digit] = '9';
            if (buf[which_digit] == '0')  buf[which_digit] = 'Z';
            else                          buf[which_digit]--;
            TV->print(30, 40, buf);
        }
        if (player->joy_down())
        {
            if (buf[which_digit] == 'Z')  buf[which_digit] = '0';
            if (buf[which_digit] == '9')  buf[which_digit] = 'A';
            else                          buf[which_digit]++;
            TV->print(30, 40, buf);
        }
        if (player->button_c())
        { 
            break;
        }

        TV->print(30, 40, buf);

        TV->delay_frame(10);
    }
}

// Displays the list of high scores.
// num is the number of high scores to show.
// TODO make this scroll?
void high_score_show_list(TVout* TV, Nunchuck* player, char* names[], unsigned long scores[], unsigned char num)
{
  TV->clear_screen();
  
  TV->printPGM(0, 0, PSTR("High Scores"));
  TV->draw_line(0, 15, TV->hres() - 1, 15, 1);
  char buf[10];
  for (unsigned char i = 0; i < num; i++)
  {
    TV->print_char(0, 20+10*i, '1' + i);
    TV->print(10, 20+10*i, names[i]);
    ltoa(scores[i], buf, 10);
    TV->print(TV->hres() - 8*strlen(buf) - 8, 20+10*i, buf);
  }
  TV->printPGM(0, 85, PSTR("Press C to continue"));
  
  while(1)
  {
    player->update();
    if (player->button_c())
      break;
  }
}

