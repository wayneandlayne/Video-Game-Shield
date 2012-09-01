/*
   ==Two player nunchuck TV out demo sketch==
 Demonstrates using both nunchucks with the Video Game Shield,
 by reading and displaying the controllers' values to the TV.
 
 Copyright (c) 2010 Wayne and Layne, LLC - Last updated 2010/12/21
 
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
#include <fontALL.h>
#include <VideoGameHelper.h>
#include <pollserial.h>
pollserial pserial;

#define SCREENWIDTH     128
#define SCREENHEIGHT    96

TVout TV;
Nunchuck player1;
Nunchuck player2;

void setup()
{
  pserial.begin(57600);
  pserial.println("Video Game Shield Test");  
  TV.begin(_NTSC, SCREENWIDTH, SCREENHEIGHT);
  TV.select_font(font6x8);
  pserial.println("TV began");
  TV.delay_frame(60);

  title_screen_init_nunchucks(&TV, "TV two player test", &player1, &player2, true);
  pserial.println("Nunchucks initialized and providing data");
  TV.clear_screen();
  nunchuck_calibration(&TV, &player1);
  nunchuck_calibration(&TV, &player2);
  TV.clear_screen();
}

void process_nunchuck(Nunchuck* nunchuck, byte* data)
{
  nunchuck->update();
  data[0] = nunchuck->joy_x();
  data[1] = nunchuck->joy_y();
  data[2] = nunchuck->acc_x();
  data[3] = nunchuck->acc_y();
  data[4] = nunchuck->acc_z();
  data[5] = 0;
  if (nunchuck->button_c())
    data[5] |= 0x01;
  if (nunchuck->button_z())
    data[5] |= 0x02;
}

void disp_nunchuck(byte id, byte* data)
{
  byte offset = (id == 1) ? 1 : 60;
  char buf[4];

  TV.draw_rect(0 + offset, 0, 20, SCREENHEIGHT, 0, 0);

  itoa(data[0], buf, 10);
  TV.print(offset, 0, buf);
  pserial.print(buf);
  pserial.print(" ");
  itoa(data[1], buf, 10);
  TV.print(offset, 10, buf);
  pserial.print(buf);
  pserial.print(" ");
  itoa(data[2], buf, 10);
  TV.print(offset, 20, buf);
  pserial.print(buf);
  pserial.print(" ");
  itoa(data[3], buf, 10);
  TV.print(offset, 30, buf);
  pserial.print(buf);
  pserial.print(" ");
  itoa(data[4], buf, 10);
  TV.print(offset, 40, buf);
  pserial.print(buf);
  pserial.print(" ");
  itoa(data[5], buf, 10);
  TV.print(offset, 50, buf);
  pserial.println(buf);

  // draw a box and plot a point for the joystick to move around
  //TV.draw_box(20 + offset, 0, 30, 30, 1, 0, 0, 1);
  TV.draw_rect(20 + offset, 0, 30, 30, 1);
  TV.set_pixel(map(data[0], 20, 240, 0, 30) + 20 + offset, map(data[1], 20, 230, 30, 0), 1); // note that the y is being inverted
}

void loop()
{
  byte buf1[6];
  byte buf2[6];

  process_nunchuck(&player1, buf1);
  process_nunchuck(&player2, buf2);

  disp_nunchuck(1, buf1);
  disp_nunchuck(2, buf2);
}


