/*                                   __    
                                   /___/
                                   / /
   /-------------/              __/_/__
     |  |-------/       //===//__<*>_ //===//
     |  .----/                 / / /
     |  .---/                 / / /
     |  |------/             / / /
   /----------/ L + V + E + N + T + U + R + E *=-
   An Adventure Game for the Arduino/Video Game Shield 

   2011 - trodoss
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   bitmap_funcs.cpp used from "Parachute" sketch by pocketscience.
   For a full copy of that sketch, see:
   http://nootropicdesign.com/hackvision/downloads/games/Parachute.zip
   
 */
 
#include <stdlib.h> 
#include <i2cmaster.h>
#include <nunchuck.h>
#include <TVout.h>
#include <font4x6.h>

#include "map.h"
#include "room.h"
#include "elf.h"
#include "sound.h"
#include "display.h"
#include "logo.h"

#define SCREENWIDTH     128
#define SCREENHEIGHT    96

#define PAUSE_BETWEEN_ACTIONS 100

#define GAME_TITLE   0
#define GAME_PLAYING 1
#define GAME_PAUSED  2
#define GAME_WON     3
#define GAME_OVER    4
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
		   
		   //check to see what we are doing when we start the game
		   if (game_state > GAME_TITLE)
		   {
		     if (game_state == GAME_WON)
			 {
			   //if the game has been won, then reset the inventory of the elf
			   resetElf(true);
			 } else {
			   //if the game was just over (died) then do not reset the inventory
		       resetElf(false);
			 }
		   }
		   
		   game_state = GAME_PLAYING;
		   start_game();
         }
		         
         next_action = TV.millis() + PAUSE_BETWEEN_ACTIONS; 
       }	
	   update_sound();
	   break;
	   
	case GAME_PLAYING:
       if (TV.millis() >= next_action)
       {      
         player1.update();
       
         if (player1.joy_up()) moveElf(FACING_UP);
         if (player1.joy_down()) moveElf(FACING_DOWN);
         if (player1.joy_right()) moveElf(FACING_RIGHT);
         if (player1.joy_left()) moveElf(FACING_LEFT);
         if (player1.button_z()) throwSword();
         if (player1.button_c()) game_state = GAME_PAUSED;
    
         next_action = TV.millis() + PAUSE_BETWEEN_ACTIONS; 
	
         handleRoomElements();
		 
		 //check the elf's state
		 if (getElfState() != ELFSTATE_PLAYING)
		 {
		   if (getElfState() == ELFSTATE_DEAD)
		   {
		     start_game_over();
		   } else {
		     start_game_won();
		   }
		 }
       }
       update_sound();
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
  //clear the contents of the screen
  TV.clear_screen();
  
  //start the game
  setMapRoom(0);
  showElf();
  updateDisplay(getElf());
  play_song(0);
  game_state = GAME_PLAYING;  
}

void start_title()
{
  //clear the contents of the screen
  TV.clear_screen();
  
  game_state = GAME_TITLE;
  drawLogo(0);
  play_song(4);
}

void start_game_over()
{
  //clear the contents of the screen
  TV.clear_screen();
  
  game_state = GAME_OVER;
  drawLogo(4);
  play_song_once(2);
}

void start_game_won()
{
  //clear the contents of the screen
  TV.clear_screen();
  
  game_state = GAME_WON;
  drawLogo(8);
  play_song(3);
}
