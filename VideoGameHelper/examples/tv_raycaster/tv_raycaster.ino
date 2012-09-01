/*
 _____        __     _______           _____ _______ ______ _____  
|  __ \     /\\ \   / / ____|   /\    / ____|__   __|  ____|  __ \ 
| |__) |   /  \\ \_/ / |       /  \  | (___    | |  | |__  | |__) |
|  _  /   / /\ \\   /| |      / /\ \  \___ \   | |  |  __| |  _  / 
| | \ \  / ____ \| | | |____ / ____ \ ____) |  | |  | |____| | \ \ 
|_|  \_\/_/    \_\_|  \_____/_/    \_\_____/   |_|  |______|_|  \_\

Copyright (c) 2010 Wayne and Layne, LLC
Based heavily on code by Lode Vandevenne, and modified to run on arduino with the TVout library.
Code taken from the most excellent guide to raycasting technology, located on the web at:
http://lodev.org/cgtutor/raycasting.html

Copyright (c) 2004-2007, Lode Vandevenne
 
All rights reserved.
 
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
*/

#include <TVout.h>
#include <font4x6.h>
#include <video_gen.h>
#include <i2cmaster.h>
#include <nunchuck.h>

#define SCREENWIDTH     128
#define SCREENHEIGHT    96

#define RENDERWIDTH     96
#define RENDERHEIGHT    96

#define MAPWIDTH        24
#define MAPHEIGHT       24

unsigned char render[SCREENWIDTH];

TVout TV;
Nunchuck nunchuck_strafe;
Nunchuck nunchuck_turn;

byte worldMap[MAPHEIGHT][MAPWIDTH >> 3] =
{
  {0b11111111, 0b11111111, 0b11111111},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b01010101},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b01000101},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b01010101},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b10000000, 0b00000000, 0b00000001},
  {0b11111111, 0b11111111, 0b11111111}
};

// just an accessor for the new compressed world map structure
byte checkWorldMap(byte mapX, byte mapY)
{
  if ( (worldMap[mapX][mapY >> 3]) & (1 << (7 - (mapY & 0x07))) )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup()
{
  TV.begin(_NTSC, SCREENWIDTH, SCREENHEIGHT);
  TV.select_font(font4x6);
  TV.delay_frame(60);
  TV.clear_screen();
  TV.printPGM(0, 0, PSTR("tv raycaster"));
  TV.delay_frame(60);
  nunchuck_strafe.begin(WII_PLAYER_1);
  nunchuck_turn.begin(WII_PLAYER_2);
}

void loop()
{
  double posX = 10, posY = 10;        // x and y start position
  double dirX = -1, dirY = 0;         // initial direction vector
  double planeX = 0, planeY = 0.66;   // the 2d raycaster version of camera plane
  
  // used in the drawing phase
  byte midpoint = RENDERHEIGHT/2; 

  while(1)
  {
    unsigned long start = display.frames;

    for (byte x = 0; x < RENDERWIDTH; x++)
    {
      // -------------------- Render the next frame -------------------------------
      
      // calculate ray position and direction 
      double cameraX = 2 * x / (double) RENDERWIDTH - 1; // x-coordinate in camera space
      double rayPosX = posX;
      double rayPosY = posY;
      double rayDirX = dirX + planeX * cameraX;
      double rayDirY = dirY + planeY * cameraX;
      // which box of the map we're in  
      int mapX = (int) rayPosX;
      int mapY = (int) rayPosY;

      // length of ray from current position to next x or y-side
      double sideDistX;
      double sideDistY;

      // length of ray from one x or y-side to next x or y-side
      double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
      double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
      double perpWallDist;

      // what direction to step in x or y-direction (either +1 or -1)
      int stepX;
      int stepY;

      int side; // was a NS or a EW wall hit?
      // calculate step and initial sideDist
      if (rayDirX < 0)
      {
        stepX = -1;
        sideDistX = (rayPosX - mapX) * deltaDistX;
      }
      else
      {
        stepX = 1;
        sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
      }
      if (rayDirY < 0)
      {
        stepY = -1;
        sideDistY = (rayPosY - mapY) * deltaDistY;
      }
      else
      {
        stepY = 1;
        sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
      }
      
      // perform DDA
      while (1)
      {
        // jump to next map square, OR in x-direction, OR in y-direction
        if (sideDistX < sideDistY)
        {
          sideDistX += deltaDistX;
          mapX += stepX;
          side = 0;
        }
        else
        {
          sideDistY += deltaDistY;
          mapY += stepY;
          side = 1;
        }
        
        // Check if ray has hit a wall
        if (checkWorldMap(mapX, mapY) > 0)
          break;
      }
      // Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
      if (side == 0)
      {
        perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
      }
      else
      {
        perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);
      }

      // Calculate height of line to draw on screen
      byte lineHeight = abs((char)(RENDERHEIGHT / perpWallDist));
      if (lineHeight > RENDERHEIGHT)
      {
        lineHeight = RENDERHEIGHT;
      }
      
      double wallX; // where exactly the wall was hit
      if (side == 1)
      {
        wallX = rayPosX + ((mapY - rayPosY + (1 - stepY) / 2) / rayDirY) * rayDirX;
      }
      else
      {
        wallX = rayPosY + ((mapX - rayPosX + (1 - stepX) / 2) / rayDirX) * rayDirY;
      }
      wallX -= floor(wallX);
      float fudge = 0.1;

      render[x] = lineHeight/2;
      if (wallX <= fudge || wallX >= (1-fudge))
      {
        render[x] += RENDERHEIGHT;
      }
      
    } // end of main render loop
    
    char render_time_str[10];
    itoa((1000 * (display.frames - start)) / 60, render_time_str, 10);
    
    // -------------------- Drawing to the screen -------------------------------

    start = display.frames;

    TV.clear_screen();
    
    for (byte col = 0; col < RENDERWIDTH; col++)
    {
      if (render[col] > RENDERHEIGHT)
      {
        TV.draw_line(col, midpoint + (render[col]-RENDERHEIGHT), col, midpoint - (render[col]-RENDERHEIGHT), 1);
      }
      else
      {
        TV.set_pixel(col, midpoint + render[col], 1);
        TV.set_pixel(col, midpoint - render[col], 1);
      }
    }
    
    char draw_time_str[10];
    itoa((1000 * (display.frames - start)) / 60, draw_time_str, 10);
    
    // -------------------- Dealing with user input -------------------------------
    
    start = display.frames;
    nunchuck_strafe.update();
    nunchuck_turn.update();
    
    double frameTime = .1; //TODO: actually measure frame length!

    //speed modifiers
    double moveSpeed; // = frameTime * 5.0; //the constant value is in squares/second
    double rotSpeed; // = frameTime * 1.0; //the constant value is in radians/second

    unsigned char temp;
    
    // move forward if no wall in front of you
    temp = nunchuck_strafe.joy_y_scaled();
    if (temp >= 60)
    {
      moveSpeed = frameTime * mapDouble(temp, 60, 99, 0.0, 10.0);
      if (!checkWorldMap(int(posX + dirX * moveSpeed), int(posY))) posX += dirX * moveSpeed;
      if (!checkWorldMap(int(posX), int(posY + dirY * moveSpeed))) posY += dirY * moveSpeed;
    }

    // move backwards if no wall behind you
    if (temp <= 40)
    {
      moveSpeed = frameTime * mapDouble(temp, 40, 0, 0.0, 10.0);
      if (!checkWorldMap(int(posX - dirX * moveSpeed), int(posY))) posX -= dirX * moveSpeed;
      if (!checkWorldMap(int(posX), int(posY - dirY * moveSpeed))) posY -= dirY * moveSpeed;
    }
    
    // When rotating a vector to the right by 90 degrees:
    //    x' = y
    //    y' = -x
    // When rotating a vector to the left by 90 degrees:
    //    x' = -y
    //    y' = x
    // In the two strafe functions below, we've switched dirX and dirY according to the above rules:
    
    temp = nunchuck_strafe.joy_x_scaled();
    // move right if no wall in the way
    if (temp >= 60)
    {
      moveSpeed = frameTime * mapDouble(temp, 60, 99, 0.0, 5.0);
      if (!checkWorldMap(int(posX + dirY * moveSpeed), int(posY))) posX += dirY * moveSpeed;
      if (!checkWorldMap(int(posX), int(posY + -dirX * moveSpeed))) posY += -dirX * moveSpeed;
    }
    
    // move left if no wall in the way
    if (temp <= 40)
    {
      moveSpeed = frameTime * mapDouble(temp, 40, 0, 0.0, 5.0);
      if (!checkWorldMap(int(posX + -dirY * moveSpeed), int(posY))) posX += -dirY * moveSpeed;
      if (!checkWorldMap(int(posX), int(posY + dirX * moveSpeed))) posY += dirX * moveSpeed;
    }
    
    // rotate to the right
    temp = nunchuck_turn.joy_x_scaled();
    if (temp >= 60)
    {
      rotSpeed = frameTime * mapDouble(temp, 60, 99, 0.0, 1.0);
      //both camera direction and camera plane must be rotated
      double oldDirX = dirX;
      dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
      dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
      double oldPlaneX = planeX;
      planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
      planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    }

    // rotate to the left
    if (temp <= 40)
    {
      rotSpeed = frameTime * mapDouble(temp, 40, 0, 0.0, 1.0);
      //both camera direction and camera plane must be rotated
      double oldDirX = dirX;
      dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
      dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
      double oldPlaneX = planeX;
      planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
      planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
    }
    

    char other_time_str[10];
    itoa((1000 * (display.frames - start)) / 60, other_time_str, 10);
    
    TV.print(96, 0, render_time_str);
    TV.print(96, 8, draw_time_str);
    TV.print(96, 16, other_time_str);
  }
}




