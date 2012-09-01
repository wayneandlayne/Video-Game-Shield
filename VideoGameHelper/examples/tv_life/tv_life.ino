/* 
   
   John Conway's Game of Life
   Written by Adam Wolf and Matthew Beckler for Wayne and Layne, LLC
   http://www.wayneandlayne.com/projects/video-game-shield/
    v1.1, 19/12/2010

This is intended for the Video Game Shield, and uses the TVOut library and the software I2C Nunchuck library.

  *Stores fields in program memory
  *Around 3 FPS at resolution 104x64
  *A nunchuck can be used to pause or access a menu with settings.
  *Help screen explaining what Conway's Game of Life is
  
Features:
  *Works with 1 or 0 nunchucks
  *Outputs to a TV over an RCA connection
  *Audio notifier for each update

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

#include <TVout.h>
#include <video_gen.h>
#include <nunchuck.h>
#include <VideoGameHelper.h>
#include "Patterns.h"
#include <fontALL.h>
#include <i2cmaster.h>

//The resolution of the life playing field is XCOLS by YCOLS.
#define XCOLS 104
#define YCOLS 64
#define ARRAY_XCOLS XCOLS/8

TVout TV;
char life[ARRAY_XCOLS][YCOLS];
Nunchuck nunchuck;
char nunchuck_plugged_in = 0;

char mute = 0; //Set to 1 to start the game muted

void setup()
{
  TV.begin(_NTSC,104,64);
  TV.clear_screen();
  TV.select_font(font6x8);
  TV.printPGM(0,  0, PSTR("Conway's")); //PSTR is a macro that puts a string in PROGMEM.  It's a really useful shortcut if you're only going to use that string once in a sketch.
  TV.printPGM(0, 10, PSTR("Game of Life")); 
  TV.printPGM(0, 32, PSTR("by"));
  TV.printPGM(0, 41, PSTR("Wayne and Layne"));
  int random_seed = analogRead(0);
  randomSeed(random_seed);
  setup_nunchuck(&nunchuck);
  delay_frame_or_break_for_z(&nunchuck, 60*15);
  TV.clear_screen();
}


void loop()
{
  //randomize_field();           //uncomment this, and comment the other two lines here to get the field to begin randomly
  load_pattern_into_buffer(patterns[1]);   //uncomment this, and comment the other two lines here to start with a pattern
  //make_glider(32,32);          //uncomment this, and comment the other two lines here to start with a simple glider
  
  copy_from_buffer_to_screen();
  
  while (1)
  {
    update_field();
    if (!mute)
    {
      TV.tone(500, 20);
    }
    
    //react to nunchuck
    if (nunchuck_plugged_in)
    {
      nunchuck.update();
      if (nunchuck.button_z()) //they pushed down pause
      {
        if (!mute)
        {
          pause_noise();
        }
        //wait for them to release the pause button
        while (nunchuck.button_z())
        {
          TV.delay_frame(5);
          nunchuck.update();
        }


        //they've released the pause button
        //wait for them to press it again or do a single frame
        while (!nunchuck.button_z())
        {
          TV.delay_frame(5);
          nunchuck.update();
        }

        //they pressed it again
        //wait for them to release it for good!
        while (nunchuck.button_z())
        {
          TV.delay_frame(5);
          nunchuck.update();
        }
      }

      if (nunchuck.button_c())
      {
        TV.select_font(font4x6);
        //they pushed down menu
        char return_to_life = 0;
        while (!return_to_life)
        {
          char* options[] = { "Back", "Load Saved Pattern", "Randomize Field", "Toggle Audio", "Help"};
          unsigned char answer = question_box(&TV, &nunchuck, "Menu", options, 5, 0);
          switch (answer)
          {
            case 0:
              return_to_life = 1;
              break; //do nothing, get outta here
            case 1:
              if (loadPatternsMenu() != 0) //they didn't choose back
                return_to_life = 1;
              break;
            case 2:
              randomize_buffer();
              return_to_life = 1;
              break;
            case 3:
              mute = !mute;
              break;
            case 4:
              helpMenu();
              break;
          }
        }
        //exit menu
        TV.clear_screen();
        copy_from_buffer_to_screen(); 
      }
    }
  }
}

void setup_nunchuck(Nunchuck* nunchuck)
{
  if (nunchuck->begin(NUNCHUCK_PLAYER_1) == 0)
  {
    //nunchuck detected
    nunchuck_plugged_in = 1;
  } 
  else
  {
    nunchuck_plugged_in = 0;
  }
}

void pause_noise() 
{
  #define NOTE_C7  2093
  #define NOTE_E7  2637
  
  
  int melody[] = {NOTE_E7, NOTE_C7, NOTE_E7, NOTE_C7};
//The rest of the notes can be found in pitches.h on the Arduino website
  byte noteDurations[] = {12, 12, 12, 3};
  for (byte thisNote = 0; thisNote < 4; thisNote++) 
  {
    int noteDuration = 800/noteDurations[thisNote];
    TV.tone(melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    TV.delay_frame((pauseBetweenNotes*6)/100);
  }
}

void helpMenu()
{
  explanation_screen();
}

char loadPatternsMenu()
{
  // returns the choice, 0 through num_answers-1
  TV.clear_screen();

  //TV.select_font(_3x5);
  TV.select_font(font4x6);

  TV.printPGM(0, 0, PSTR("Load Patterns"));
  TV.draw_line(0, 7, TV.hres() - 1, 7, 1);
  TV.printPGM(8, 9, PSTR("Back"));
  for (byte i = 0; i < NUM_OF_PATTERNS; i++)
    TV.printPGM(8, 16 + i * 7, patterns[i]->name);

  byte choice = 0;
  //TV.draw_box(1, 7*choice+9, 5, 5, 1, 1, 0, 0); // draw selection box
  TV.draw_rect(1, 7*choice+9, 5, 5, 1, 1); // draw selection box
  
  while (1)
  {
    TV.delay_frame(10);

    nunchuck.update();
    if (nunchuck.joy_up())
    {
      //TV.draw_box(1, 7*choice+9, 5, 5, 0, 0, 0, 0); // remove selection box
      TV.draw_rect(1, 7*choice+9, 5, 5, 0, 0); // remove selection box
      
      if (choice == 0)
        choice = NUM_OF_PATTERNS ; //not num of patterns -1 cuz we added BACK
      else
        choice--;
      //TV.draw_box(1, 7*choice+9, 5, 5, 1, 1, 0, 0); // draw selection box
        TV.draw_rect(1, 7*choice+9, 5, 5, 1, 1); // draw selection box
    }
    if (nunchuck.joy_down())
    {
      //TV.draw_box(1, 7*choice+9, 5, 5, 0, 0, 0, 0); // remove selection box
      TV.draw_rect(1, 7*choice+9, 5, 5, 0, 0); // remove selection box

      choice = (choice + 1) % (NUM_OF_PATTERNS+1);
      //TV.draw_box(1, 7*choice+9, 5, 5, 1, 1, 0, 0); // draw selection box
      TV.draw_rect(1, 7*choice+9, 5, 5, 1, 1); // draw selection box

    }
    if (nunchuck.button_c() || nunchuck.button_z()) // TODO what to do here for buttons
    {
      break;
    }
  }
  
  //TV.select_font(_5x7);
  TV.select_font(font6x8);
  if (choice > 0) //i.e. not back
  {
    load_pattern_into_buffer(patterns[choice-1]);
  }  
  return choice;
}

void delay_frame_or_break_for_z(Nunchuck* player, unsigned int frames)
{
  long end_frame = display.frames + frames;

  while ( display.frames < end_frame )
  {
    player->update();
    if (player->button_z())
    {
      while (player->button_z()) //basically wait for the unpress
      {
        TV.delay_frame(5);
        player->update();
      }
      //they pressed z, and now they stopped
      TV.delay_frame(5);
      return;
    }
  }
}

void load_pattern(const Pattern* pattern)
{
  for (byte x = 0; x < ARRAY_XCOLS; x++)
  {
    for (byte y = 0; y < YCOLS; y++)
    {
      display.screen[x+y*display.hres] = pgm_read_byte(&(pattern->field[y][x]));
    }
  }
}

void load_pattern_into_buffer(const Pattern* pattern)
{
  for (byte x = 0; x < ARRAY_XCOLS; x++)
  {
    for (byte y = 0; y < YCOLS; y++)
    {
      life[x][y] = pgm_read_byte(&(pattern->field[y][x]));
    }
  }
}

void initialize_field()
{
  for (byte x = 0; x < ARRAY_XCOLS; x++)
  {
    for (byte y = 0; y < YCOLS; y++)
    {
      life[x][y] = 0x00;
    }
  }
}


void randomize_field()
{
  for (byte x = 0; x < ARRAY_XCOLS; x++)
  {
    for (byte y = 0; y < YCOLS; y++)
    {
      display.screen[x+y*display.hres] = random(0, 256);
    }
  }
}

void randomize_buffer()
{
  for (byte x = 0; x < ARRAY_XCOLS; x++)
  {
    for (byte y = 0; y < YCOLS; y++)
    {
      life[x][y] = random(0, 256);
    }
  }
}

