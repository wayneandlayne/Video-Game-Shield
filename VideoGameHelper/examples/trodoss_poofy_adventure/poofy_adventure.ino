/*  ________                    ______
   /    __  \  ______  ______  /   __/---//--/\
   |   /__/  |/ __   \/ __   \/   __/|  /  /  /
  /     ____   /__/    /__/      /\ \|   /  /
 /_____/\   |_______/\______/\__/   |__/  /
 \     \    \       \ \     \ \  \  \  \ / 
   An Adventure Game for the Arduino/Video Game Shield 

   2012 - trodoss
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
 */
 
#include <stdlib.h> 
#include <i2cmaster.h>
#include <nunchuck.h>
#include <TVout.h>
#include <font4x6.h>

#define SCREENWIDTH     128
#define SCREENHEIGHT    96

#define PAUSE_BETWEEN_ACTIONS 50

#define GAME_TITLE   0
#define GAME_PLAYING 1
#define GAME_PAUSED  2
#define GAME_WON     3
#define GAME_OVER    4

#include "sound.h"
#include "logo.h"
#include "level.h"
#include "poofy.h"
#include "display.h"

TVout TV;
Nunchuck player1;
char game_state;

void setup()
{
  TV.begin(_NTSC, SCREENWIDTH, SCREENHEIGHT);
  TV.select_font(font4x6);
  
  randomSeed(analogRead(0));
  
  player1.begin(0);
  start_title();
}

void loop()
{  
  static long next_action = 0;
  switch (game_state)
  {
    case GAME_TITLE:
    case GAME_OVER:
    case GAME_WON:
       if (TV.millis() >= next_action)
       {
         player1.update();
		 //wait for a button to be pressed to continue
         if ((player1.button_c()) || (player1.button_z()))
         {	   
		   game_state = GAME_PLAYING;
		   start_game();
         }
		         
         next_action = TV.millis() + PAUSE_BETWEEN_ACTIONS; 
       }	
	   sound_update();
	   break;
	   
	case GAME_PLAYING:
       if (TV.millis() >= next_action)
       {      
         player1.update();
       
         if (player1.joy_right()) poofy_move(FACING_RIGHT);
         if (player1.joy_left()) poofy_move(FACING_LEFT);
         if (player1.button_z()) poofy_jump();
         if (player1.button_c()) game_state = GAME_PAUSED;
    
         next_action = TV.millis() + PAUSE_BETWEEN_ACTIONS; 
	
         level_update();
		 
		 //check the poofy's state
		 if (poofy_getState() > POOFY_FALLING)
		 {
		   if (poofy_getState() == POOFY_DEAD)
		   {
		     start_game_over();
		   } else {
		     start_game_won();
		   }
		 }
       }
       sound_update();
       break;
       
   case GAME_PAUSED:
       if (TV.millis() >= next_action)
       {
         player1.update();
		 
         if (player1.button_c()) game_state = GAME_PLAYING;
         
         next_action = TV.millis() + PAUSE_BETWEEN_ACTIONS; 
       }
       break;
  }
}

void start_game()
{
  game_state = GAME_PLAYING;
  poofy_init();
  level_draw();
  sound_play_song(1);
}

void start_title()
{
  TV.clear_screen();
  game_state = GAME_TITLE;
  logo_draw();
  sound_play_song(0);
}

void start_game_over()
{
  TV.clear_screen();
  TV.printPGM(48, 40, PSTR("Game Over"));
  game_state = GAME_OVER;
  display_update(0);
  sound_play_song_once(2);
}  

void start_game_won()
{
  TV.clear_screen();
  TV.printPGM(48, 40, PSTR("Victory!!"));
  game_state = GAME_WON;
  display_update(0);
  sound_play_song_once(3);
}
