/*
  Two Player TicTacToe
  by Pascal König
  v1.0, 24/04/2011 - Original
  v1.1, 27/04/2011 - Touched-up and translated into English

  Copyright (c) 2011 Pascal König and Wayne and Layne, LLC

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
#include <i2cmaster.h>
#include <nunchuck.h>
#include <TVout.h>
#include <font4x6.h>
#include <VideoGameHelper.h>
#include <avr/pgmspace.h>

TVout TV;
Nunchuck player1;
Nunchuck player2;

byte hres, vres;

byte cursor_x, cursor_y;

byte field[9] = {0,0,0,0,0,0,0,0,0}; //Fields: 0 = free, 1 = Player 1, 2 = Player 2 ----- left to right, top to bottom

byte player = 0; // 1 = Player 1, 2 = Player 2 - will be randomly decided at the beginning

#define STATE_TEST    0
#define STATE_START   1
#define STATE_PLAY    2
byte state = STATE_TEST;

void setup()
{
  TV.begin(_NTSC);
  TV.select_font(font4x6);
  TV.delay_frame(60);
  state = STATE_START;
 
  randomSeed(analogRead(0));
  player = random(1, 3);
  
  title_screen_init_nunchucks(&TV, "TicTacToe", &player1, &player2, true);
  // If we set the thresholds to different values, we can activate diagonal movement of the cursor:
  player1.joy_set_threshold(80, 170, 80, 170, 30);
  player2.joy_set_threshold(80, 170, 80, 170, 30);
  
  hres = TV.hres();
  vres = TV.vres();

}

void init_display()
{
  // draw dashed lines for the game board
  for (byte y = 0; y < vres; y += 6)
  {
    TV.draw_line(hres / 3, y, hres / 3, y + 2, 1);
    TV.draw_line((hres / 3) + (hres / 3), y, (hres / 3) + (hres / 3), y + 2, 1);
  }
  for (byte x = 0; x < hres; x += 6)
  {
    TV.draw_line(x, vres / 3, x + 2, vres / 3, 1);
    TV.draw_line(x, (vres / 3) + (vres / 3), x + 2, (vres / 3) + (vres / 3), 1);
  }
}

void draw_cursor()
{
  TV.set_pixel(cursor_x, cursor_y, 2);
}

void draw_x(byte index)
{
  byte x_inset = hres / 10;
  byte y_inset = vres / 10;
  byte x_left =   (((index % 3) * hres) / 3) + x_inset;
  byte x_right =  ((((index % 3) + 1) * hres) / 3) - x_inset;
  byte y_top =    (((index / 3) * vres) / 3) + y_inset;
  byte y_bottom = ((((index / 3) + 1) * vres) / 3) - y_inset;
    
  TV.draw_line(x_left, y_top,    x_right, y_bottom, 1);
  TV.draw_line(x_left, y_bottom, x_right, y_top,    1);
}
void draw_o(byte index)
{
  byte x_center = ((((index % 3) * 2) + 1) * hres) / 6;
  byte y_center = ((((index / 3) * 2) + 1) * vres) / 6;
  byte radius = (min(hres, vres) / 5) - 10;
  TV.draw_circle(x_center, y_center, radius, 1, -1);
}

// returns whether it is set or not (1 = valid move, player just played, or 0 = space already occupied)
byte set_field(byte player)
{
    byte index;

    if(cursor_x > 0 && cursor_x < hres / 3 && cursor_y > 0 && cursor_y < vres / 3) // top-left
        index = 0;
    if(cursor_x > hres / 3 && cursor_x < (hres / 3) + (hres / 3) && cursor_y > 0 && cursor_y < vres / 3) // top-middle
        index = 1;
    if(cursor_x > (hres / 3) + (hres / 3) && cursor_x < hres && cursor_y > 0 && cursor_y < vres / 3) // top-right
        index = 2;

    if(cursor_x > 0 && cursor_x < hres / 3 && cursor_y > vres / 3 && cursor_y < (vres / 3) + (vres / 3)) // middle-left
        index = 3;
    if(cursor_x > hres / 3 && cursor_x < (hres / 3) + (hres / 3) && cursor_y > vres / 3 && cursor_y < (vres / 3) + (vres / 3)) // middle-middle
        index = 4;
    if(cursor_x > (hres / 3) + (hres / 3) && cursor_x < hres && cursor_y > vres / 3 && cursor_y < (vres / 3) + (vres / 3)) // middle-right
        index = 5;

    if(cursor_x > 0 && cursor_x < hres / 3 && cursor_y > (vres / 3) + (vres / 3) && cursor_y < vres) // bottom-left
        index = 6;
    if(cursor_x > hres / 3 && cursor_x < (hres / 3) + (hres / 3) && cursor_y > (vres / 3) + (vres / 3) && cursor_y < vres) // bottom-middle
        index = 7;
    if(cursor_x > (hres / 3) + (hres / 3) && cursor_x < hres && cursor_y > (vres / 3) + (vres / 3) && cursor_y < vres) // bottom-right
        index = 8;

    // TODO check for index not set?
    if (field[index] == 0) // empty space, we can play here
    {
        if (player == 1)
            draw_x(index);
        else
            draw_o(index);
        field[index] = player;
        return 1;
    }
    // else, space already taken, return 0
    return 0;
}

// Reads from the provided Nunchuck and updates the cursor variables
void update_cursor(Nunchuck* player)
{
    player->update();
    
    if (player->joy_up())
    {
        cursor_y--;
        if (cursor_y == 255) // if we decrement a byte variable from 0, it rolls-under to be 255
            cursor_y = 0;
    }

    if (player->joy_down())
    {
        cursor_y++;
        if (cursor_y == vres)
            cursor_y = vres - 1;
    }

    if (player->joy_left())
    {
        cursor_x--;
        if(cursor_x == 255) // if we decrement a byte variable from 0, it rolls-under to be 255
            cursor_x = 0;
    }

    if (player->joy_right())
    {
        cursor_x++;
        if(cursor_x == hres)
            cursor_x = hres - 1;
    }
}

// check if the specified player has won (1) or not (0).
byte check_for_winner(byte player)
{
    if ( (field[0] == player && field[1] == player && field[2] == player) ||    // row 0
         (field[3] == player && field[4] == player && field[5] == player) ||    // row 1
         (field[6] == player && field[7] == player && field[8] == player) ||    // row 2
         (field[0] == player && field[3] == player && field[6] == player) ||    // col 0
         (field[1] == player && field[4] == player && field[7] == player) ||    // col 1
         (field[2] == player && field[5] == player && field[8] == player) ||    // col 2
         (field[0] == player && field[4] == player && field[8] == player) ||    // diag upper-left to lower-right
         (field[2] == player && field[4] == player && field[6] == player) )     // diag upper-right to lower-left
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

byte determine_victory() // returns the winner: 1 = player 1, 2 = player 2, 3 = draw, 0 = no winner yet
{
    if (check_for_winner(1))
        return 1;
    else if (check_for_winner(2))
        return 2;
    else if (field[0] != 0 && field[1] != 0 && field[2] != 0 && field[3] != 0 && field[4] != 0 && field[5] != 0 && field[6] != 0 && field[7] != 0 && field[8] != 0)
        return 3;
    else
        return 0;
}

void reset_game_board()
{
    for (byte i = 0; i < 10; i++)
        field[i] = 0;
}

void loop()
{
    switch (state)
    {
        case STATE_TEST:
            for (byte t = 0; t < 10; t++)
            {       
                TV.delay_frame(20);
            }
            break;
            
        case STATE_START:
            TV.clear_screen();
            init_display();
            draw_cursor(); // pre-draw the cursor so we can erase it

            if (player == 1)
                TV.printPGM((hres / 2) - 16, 0, PSTR("Player 1"));
            else
                TV.printPGM((hres / 2) - 16, 0, PSTR("Player 2"));

            state = STATE_PLAY;
            break;
            
        case STATE_PLAY:
            draw_cursor();
            if (player == 1)
            {
                update_cursor(&player1);
                if (player1.button_z())
                {
                    if (set_field(1) == 1)       
                    {
                        player = 2;
                        TV.printPGM((hres / 2) - 16, 0, PSTR("Player 2"));
                    }
                }
            }
            else // if (player == 2)
            {
                update_cursor(&player2);
                if (player2.button_z())
                {
                    if (set_field(2) == 1)
                    {
                        player = 1;
                        TV.printPGM((hres / 2) - 16, 0, PSTR("Player 1"));
                    }
                }
            }

            byte victory_value = determine_victory();
            switch (victory_value)
            {
                case 1:
                    TV.printPGM((hres / 2) - 16, vres / 2, PSTR("Player 1 wins!"));
                    break;
                case 2:
                    TV.printPGM((hres / 2) - 16, vres / 2, PSTR("Player 2 wins!"));
                    break;
                case 3:
                    TV.printPGM((hres / 2) - 16, vres / 2, PSTR("Nobody wins!"));
                    break;
            }
            if (victory_value != 0)
            {
                reset_game_board();
                TV.delay_frame(240);
                state = STATE_START;
                break;
            }

            draw_cursor();

            TV.delay_frame(1);
            break;
    }
}
