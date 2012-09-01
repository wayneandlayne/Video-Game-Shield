
/*
  Two Player Pong
  Written by Matthew Beckler and Adam Wolf for Wayne and Layne, LLC
  http://www.wayneandlayne.com/projects/video-game-shield/
  v1.2, 2011-05-02

  Demonstrates using both nunchucks with the Video Game Shield,
  in a fun and simple pong-like game. We also have a version that
  uses potentiometers as the controllers, if you don't have any
  Wii nunchucks to use.

  Includes new pause functionality added by forum member Pascal König.

  Copyright (c) 2011, Wayne and Layne, LLC
  
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

#define PADDLE_MOVEMENT 3
#define PADDLE_HEIGHT 10
#define PADDLE_OFFSET 2
byte hres, vres;

byte ball_x, ball_y;
char ball_dx = 1;
char ball_dy = 1;

byte score[] = {0, 0};

byte pause = 0; // 0 = not paused, 1 = player 1 paused, 2 = player 2 paused
byte pause_timer = 0; // used to debounce switching between pause and un-pause

byte leftpaddle_y, rightpaddle_y;

#define BEEP TV.tone(2000, 30)

#define STATE_TEST    0
#define STATE_START   1
#define STATE_PLAY    2
#define STATE_MISS    3
#define STATE_PAUSE   4

byte state = STATE_TEST;
byte missed = 0; // who missed?

void setup()
{
  TV.begin(_NTSC);
  TV.select_font(font4x6);
  TV.delay_frame(60);
  state = STATE_START;
}

void draw_scores()
{
  TV.print_char((hres / 4) - 4, 0, '0' + score[0]);
  TV.print_char((hres / 4) + (hres / 2) - 4, 0, '0' + score[1]);
}

void draw_paddles()
{
  // clear old paddles
  TV.draw_rect(0, 0, 1, vres, 0, 0);
  TV.draw_rect(hres-1, 0, 1, vres, 0, 0);
  
  // draw new paddles
  TV.draw_rect(0, leftpaddle_y, 1, PADDLE_HEIGHT, 1, 1);
  TV.draw_rect(hres-1, rightpaddle_y, 1, PADDLE_HEIGHT, 1, 1);
}

void init_display()
{
  // draw dotted vertical middle line
  for (byte y = 0; y < vres; y += 6)
    TV.draw_line(hres / 2, y, hres / 2, y + 2, 1);
  
  draw_scores();
  draw_paddles();
}

void draw_ball()
{
  //TV.draw_rect(ball_x, ball_y, 1, 2, 2);
  TV.set_pixel(ball_x, ball_y, 2);
}

void reset_ball_and_paddles()
{
  byte noise = analogRead(0);
  
  ball_x = (noise & 0x04) ? ((noise & 0x08) ? hres/4 : (hres/4 + hres/2)) : hres / 2;
  ball_y = (noise & 0x10) ? ((noise & 0x20) ? vres/4 : (vres/4 + vres/2)) : vres / 2;
  ball_dx = (noise & 0x01) ?  1 : -1;
  ball_dy = (noise & 0x02) ? -1 :  1;
  
  leftpaddle_y = vres / 2;
  rightpaddle_y = vres / 2;
}

void loop()
{
  switch (state)
  {
    case STATE_TEST:
      for (byte t = 0; t < 10; t++)
      {
        score[0] = 9 - t;
        score[1] = t;
        leftpaddle_y = t << 3;
        rightpaddle_y = t << 2;
        
        draw_scores();
        draw_paddles();
        
        TV.delay_frame(20);
      }
      break;
      
    case STATE_START:
      title_screen_init_nunchucks(&TV, "Pong", &player1, &player2, true);
      
      hres = TV.hres() - 6; // this is based on what's visible on my tv
      vres = TV.vres();

      TV.clear_screen();
      score[0] = 0;
      score[1] = 0;
      reset_ball_and_paddles();
      init_display();
      draw_ball(); // pre-draw the ball so we can erase it
      
      state = STATE_PLAY;
      break;
      
    case STATE_PLAY:
      pause_timer++;

      // top and bottom walls
      if (ball_y == vres || ball_y == 0)
      {
        BEEP;
        ball_dy *= -1; // TODO fancier angles?
      }
      
      // left and right walls/paddles
      if (ball_x >= hres - 2)
      { // right side
        if (ball_y > rightpaddle_y - PADDLE_OFFSET && ball_y < (rightpaddle_y + PADDLE_HEIGHT + PADDLE_OFFSET) && ball_dx > 0 )
        {
          BEEP;
          ball_dx = -1; // TODO fancier angles for bounce?
        }
      }
      if (ball_x == hres)
      {
        missed = 1;
        state = STATE_MISS;
        break;
      }
      
      if (ball_x <= 2)
      { // left side
        if (ball_y > leftpaddle_y - PADDLE_OFFSET && ball_y < (leftpaddle_y + PADDLE_HEIGHT + PADDLE_OFFSET) && ball_dx < 0 )
        {
          BEEP;
          ball_dx = 1; // TODO fancier angles for bounce?
        }
      }
      if (ball_x == 0)
      { // left side missed
        missed = 0;
        state = STATE_MISS;
        break;
      }
      
      // update paddle positions
      player1.update();
      if (player1.button_z() && pause_timer > 20)
      {
        pause_timer = 0;
        state = STATE_PAUSE;
        pause = 1;
      }
      if (player1.joy_up())
      {
        leftpaddle_y -= PADDLE_MOVEMENT;
        if (leftpaddle_y > 250) leftpaddle_y = 0;
      }
      if (player1.joy_down())
      {
        leftpaddle_y += PADDLE_MOVEMENT;
        if (leftpaddle_y > (vres - PADDLE_HEIGHT - 1)) leftpaddle_y = vres - PADDLE_HEIGHT;
      }
      player2.update();
      if (player2.button_z() && pause_timer > 20)
      {
        pause_timer = 0;
        state = STATE_PAUSE;
        pause = 2;
      }
      if (player2.joy_up())
      {
        rightpaddle_y -= PADDLE_MOVEMENT;
        if (rightpaddle_y>= 250) rightpaddle_y = 0;
      }
      if (player2.joy_down())
      {
        rightpaddle_y += PADDLE_MOVEMENT;
        if (rightpaddle_y > (vres - PADDLE_HEIGHT - 1)) rightpaddle_y = vres - PADDLE_HEIGHT;
      }
      
      // update ball position
      draw_ball();
      ball_x += ball_dx;
      ball_y += ball_dy;
      draw_paddles();
      draw_ball();
      
      TV.delay_frame(1);
      break;
      
    case STATE_MISS:
      // someone missed
      score[(missed + 1) & 0x01]++;
      if (score[(missed + 1) & 0x01] == 10)
      {
        // winner winner, chicken dinner
        
        // TODO make this look nicer
        TV.printPGM((missed) ? (8) : (hres / 2 + 8), 15, PSTR("Winner!"));
        TV.delay_frame(120);
        state = STATE_START;
      }
      else
      {
        TV.tone(500, 30);
        TV.printPGM((missed) ? (hres / 2 + 8) : (8), ball_y - 4, PSTR("Missed!"));
        TV.delay_frame(40);
        reset_ball_and_paddles();
        TV.clear_screen();
        init_display();
        draw_ball(); // pre-draw the ball so we can erase it
        state = STATE_PLAY;
      }
      break;

    case STATE_PAUSE:
      pause_timer++;
      TV.printPGM(hres/2 - 10, vres/2, PSTR("Pause!"));
      player1.update();
      player2.update();
      if ( (pause_timer > 20) &&
           ( (pause == 1 && player1.button_z()) ||
             (pause == 2 && player2.button_z()) ) )
      {
        state = STATE_PLAY;
        pause = 0;
        pause_timer = 0;
        TV.clear_screen();
        init_display();
        draw_ball(); // pre-draw the ball so we can erase it
      }
      break;
      
  } // end of main FSM
}

