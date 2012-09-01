/*
     ,---.   ,--.  ,--.   ,---.   ,--. ,--. ,------.  ,---.  
    '   .-'  |  ,'.|  |  /  O  \  |  .'   / |  .---' '   .-' 
    `.  `-.  |  |' '  | |  .-.  | |  .   '  |  `--,  `.  `-. 
    .-'    | |  | `   | |  | |  | |  |\   \ |  `---. .-'    |
    `-----'  `--'  `--' `--' `--' `--' '--' `------' `-----' 

    A two-player game for the Wayne and Layne Video Game Shield.

Written by Matthew Beckler and Adam Wolf for Wayne and Layne, LLC
http://www.wayneandlayne.com/projects/video-game-shield/

Last updated: September 1, 2012

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
#include <video_gen.h>
#include <VideoGameHelper.h>
#include "snake.h"
#include <fontALL.h>
#include <avr/pgmspace.h>
#include <i2cmaster.h>

TVout TV;
Nunchuck player1;
Nunchuck player2;

Snake snakes[2]; // struct defined in external header file

long round_num = 0;

#define EDGE 3
#define BAR_HEIGHT 8
#define CHAR_BAR_HEIGHT 6

#define SNAKES_ON_A_PLANE 0
#define SNAKES_ON_A_SPHERE 1
#define SNAKES_ON_A_TORUS 2

unsigned char game_mode = SNAKES_ON_A_SPHERE;
int scores[2] = {0, 0};

void setup()
{
  TV.begin(_NTSC, 128, 96);
  TV.select_font(font4x6);
 
  title_screen_init_nunchucks(&TV, "Snakes on surfaces", &player1, &player2, true);

  rules_screen();
}

void rules_screen()
{
  TV.clear_screen();
  
  TV.printPGM(0, 0, PSTR("Available Maps:"));
  TV.draw_line(0, 10, TV.hres()-1, 10, 1);
  
  TV.draw_line(0, 15, 16, 15, 1); // top
  TV.draw_line(0, 25, 16, 25, 1); // bottom
  TV.draw_line(0, 15, 0, 25, 1); // left
  TV.draw_line(16, 15, 16, 25, 1); // right
  TV.printPGM(26, 18, PSTR("Plane"));
  
  TV.draw_line(0, 35, 16, 35, 1); // top
  TV.draw_line(0, 45, 16, 45, 1); // bottom
  TV.draw_line(0, 40, 5, 40, 1); // left arrow mid
  TV.draw_line(0, 40, 2, 38, 1); // left arrow upper diag
  TV.draw_line(0, 40, 2, 42, 1); // left arrow lower diag
  TV.draw_line(16, 40, 11, 40, 1); // right arrow mid
  TV.draw_line(16, 40, 14, 38, 1); // right arrow upper diag
  TV.draw_line(16, 40, 14, 42, 1); // right arrow lower diag
  TV.printPGM(26, 38, PSTR("Sphere"));
  
  TV.draw_line(8, 54, 8, 58, 1); // up arrow mid
  TV.draw_line(8, 54, 10, 56, 1); // up arrow right diag
  TV.draw_line(8, 54, 6, 56, 1); // up arrow left diag
  TV.draw_line(8, 66, 8, 62, 1); // down arrow mid
  TV.draw_line(8, 66, 10, 64, 1); // down arrow right diag
  TV.draw_line(8, 66, 6, 64, 1); //down arrow left diag
  TV.draw_line(0, 60, 5, 60, 1); // left arrow mid
  TV.draw_line(0, 60, 2, 58, 1); // left arrow upper diag
  TV.draw_line(0, 60, 2, 62, 1); // left arrow lower diag
  TV.draw_line(16, 60, 11, 60, 1); // right arrow mid
  TV.draw_line(16, 60, 14, 58, 1); // right arrow upper diag
  TV.draw_line(16, 60, 14, 62, 1); // right arrow lower diag
  TV.printPGM(26, 58, PSTR("Torus"));
  
  TV.draw_line(0, 75, TV.hres()-1, 75, 1);
  TV.printPGM(0, 80, PSTR("P1 press C"));
  
  TV.delay_frame(30);
  while(1)
  {
    player1.update();
    if (player1.button_c())
      break;
  }
  TV.delay_frame(30);
}

void ready_screen()
{
  draw_score_bar();
 
  unsigned char p1x, p2x, p1y, p2y;
  p1x = TV.hres()/3;
  p1y = TV.vres()/2+5;
  
  p2x = (TV.hres()/3)*2;
  p2y = TV.vres()/2+5;
  
  TV.printPGM(0, 0, PSTR("Press C when ready"));
  TV.printPGM(24, 32, PSTR("P1 ready?"));
  TV.printPGM(72, 32, PSTR("P2 ready?"));
  boolean p1_ready = false;
  boolean p2_ready = false;
  while (! (p1_ready && p2_ready) )
  {
    delay(10);
    player1.update();
    if (player1.button_c())
    {
      p1_ready = true;
      TV.print(40, 40  , "ok");
    }
    player2.update();
    if (player2.button_c())
    {
      p2_ready = true;
      //TV.print(80, 40, "ok");
      TV.print(80, 40, "ok");
    }
  }
  
  TV.print(TV.hres()/2-8, 50, "3...");
  TV.delay_frame(60);
  TV.print(TV.hres()/2-8, 60, "2...");
  TV.delay_frame(60);
  TV.print(TV.hres()/2-8, 70, "1...");
  TV.delay_frame(30);
}



char move_snakes()
{
  for (int i = 0; i < 2; i++)
  {
    snakes[i].x = (snakes[i].x + snakes[i].dir_x);// % (TV.hres()-2*EDGE);
    snakes[i].y = (snakes[i].y + snakes[i].dir_y);// % (TV.vres()-2*EDGE);

    if (snakes[i].x < EDGE)
    {
      snakes[i].x = TV.hres()-2*EDGE;
    } 
    else if (snakes[i].x > TV.hres()-EDGE)
    {
      snakes[i].x = EDGE;
    }

    if (snakes[i].y < EDGE)
    {
      snakes[i].y = TV.vres()-EDGE-BAR_HEIGHT-5;
    } 
    else if (snakes[i].y > TV.vres()-5-EDGE-BAR_HEIGHT)
    {
      snakes[i].y = EDGE;
    }

    if (TV.get_pixel(snakes[i].x, snakes[i].y) == 1)
    {
      return i+1; //it returns back 1 or 2, which indicates who lost.
    } 
    else
    {
      TV.set_pixel(snakes[i].x, snakes[i].y, 1);
    }
  }
  
  return 0;
  //TODO: handle simultaneous losing 
}


void loop()
{
  TV.clear_screen();
  scores[0] = 0;
  scores[1] = 0;
  char* options[] = {"Plane", "Sphere", "Torus"};
  game_mode = question_box(&TV, &player1, "Which surface?", options, 3, 0);
  TV.delay_frame(30);
  //char* speed_options[] = {"Fastest", "Fast", "Medium", "Slow", "Slower", "Snail's pace"};
  //int throttle = question_box(&TV, &player1, "Which speed?", speed_options, 6, 2);
  game_mode = 0;
  int throttle = 2;

  while (scores[0] < 10 && scores[1] < 10)  
  {
    TV.clear_screen();
    ready_screen();
    initialize_game();
    byte game_over = 0;
    long next_frame;

    while (!game_over) 
    {
      next_frame = display.frames+throttle; 
      round_num++;

      player1.update();
      player2.update();

      // these functions update snake.dir_x and snake.dir_y:
      handle_snake(&snakes[0]);
      handle_snake(&snakes[1]);

      game_over = move_snakes();
      if (game_over)
      {
        TV.tone(1000,50);
        scores[game_over-1] += 1;
      }
      
      while (display.frames < next_frame) {};
    }
  }
  
  // winner winner chicken dinner
  TV.clear_screen();
  if (scores[0] == 10)
    TV.printPGM(0, 10, PSTR("Player 1 wins!"));
  else
    TV.printPGM(0, 10, PSTR("Player 2 wins!"));
  TV.delay_frame(120);
}

void handle_snake(Snake* snake)
{
  Nunchuck* n = snake->nunchuck;

  if (n->joy_right() && ( snake->dir_x != -1 && snake->dir_y != 0))
  {
    snake->dir_x = 1;
    snake->dir_y = 0;
  } 
  else if (n->joy_left() && ( snake->dir_x != 1 && snake->dir_y != 0))
  {
    snake->dir_x = -1;
    snake->dir_y = 0;
  } 
  else if (n->joy_up() && ( snake->dir_x != 0 && snake->dir_y != 1)) 
  {
    snake->dir_x = 0;
    snake->dir_y = -1;
  } 
  else if (n->joy_down() && ( snake->dir_x != 0 && snake->dir_y != -1))
  {
    snake->dir_x = 0;
    snake->dir_y = 1;
  }
}

void print_whole_number(byte x, byte y, int number)
{
  char output[10];
  itoa(number, output, 10);
  TV.print(x, y, output);
}

void initialize_game()
{
  TV.clear_screen();
  draw_field();
  draw_score_bar();

  snakes[0].x = TV.hres()/3;
  snakes[0].y = TV.vres()/2+5;
  snakes[0].dir_x = 1;
  snakes[0].dir_y = 0;
  snakes[0].nunchuck=&player1;

  snakes[1].x = (TV.hres()/3)*2;
  snakes[1].y = TV.vres()/2-5;
  snakes[1].dir_x = -1;
  snakes[1].dir_y = 0;
  snakes[1].nunchuck=&player2;
  round_num = 0;
}

void draw_score_bar()
{
  TV.draw_line(3, TV.vres()-EDGE-1-BAR_HEIGHT, TV.hres()-EDGE-1, TV.vres()-EDGE-1-BAR_HEIGHT, 1); //separator
  
  TV.print_char(8, TV.vres()-1-EDGE-CHAR_BAR_HEIGHT, '0' + scores[0]);//TODO: fix x value
  TV.print_char(TV.hres()-8, TV.vres()-1-EDGE-CHAR_BAR_HEIGHT, '0' + scores[1]);//TODO: fix x value
  
  if (game_mode == SNAKES_ON_A_PLANE)
  {
    TV.printPGM(32, TV.vres()-1-EDGE-CHAR_BAR_HEIGHT, PSTR("Snakes on a Plane")); //TODO: make this actually centered
  } 
  else if (game_mode == SNAKES_ON_A_SPHERE)
  {
    TV.print(32, TV.vres()-1-EDGE-CHAR_BAR_HEIGHT, "Snakes on a Sphere"); //TODO: make this actually centered
  }  
  else if (game_mode == SNAKES_ON_A_TORUS)
  {
    TV.print(32, TV.vres()-1-EDGE-CHAR_BAR_HEIGHT, "Snakes on a Torus"); //TODO: make this actually centered    
  }
}

void draw_field()
{
  switch (game_mode)
  {
    case SNAKES_ON_A_PLANE:
      TV.draw_line(TV.hres()-EDGE-1,EDGE,TV.hres()-EDGE-1,TV.vres()-EDGE-1,1); //right
      TV.draw_line(EDGE,TV.vres()-EDGE-1, EDGE,EDGE,1); //left
      // yes, we want to fall-through here
    case SNAKES_ON_A_SPHERE:
      TV.draw_line(EDGE,EDGE,TV.hres()-EDGE-1,EDGE,1); //top
      TV.draw_line(EDGE, TV.vres()-EDGE-1,TV.hres()-EDGE-1,TV.vres()-EDGE-1,1); //bottom
      break;
  }
}

