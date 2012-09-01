/*
   ==Data Collection Example sketch==
   Demonstrates collecting data with the Video Game Shield,
   tracking and recording for long-term capture.

   Written by Matthew Beckler and Adam Wolf for Wayne and Layne, LLC
   http://www.wayneandlayne.com/projects/video-game-shield/
   Last updated: 19/12/2010

   Copyright (c) 2010, Wayne and Layne, LLC

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
#include <fontALL.h>

TVout TV;

#define NUM_SAMPLES             50
#define FRAMES_BETWEEN_SAMPLE   5 // = time_in_sec*60 (60 frames per second)
#define INITIAL_VALUE           0

unsigned long next_sample_time;
unsigned char samples[NUM_SAMPLES];
unsigned char current_sample_ix = NUM_SAMPLES;
unsigned char data_mean, data_min, data_max;

#define STATE_TEST    0
#define STATE_START   1
#define STATE_RUN     2
byte state = STATE_TEST;

void setup()
{
  TV.begin(_NTSC);
  TV.select_font(font6x8);
  TV.delay_frame(120);
  state = STATE_START;

  memset(samples, INITIAL_VALUE, 50);
}

void draw_plot()
{
    char buf[4];

    TV.clear_screen();

    TV.print(0, 0, "Cur:");
    itoa(samples[current_sample_ix], buf, 10);
    TV.print(30, 0, buf);

    TV.print(60, 0, "Avg:");
    itoa(data_mean, buf, 10);
    TV.print(90, 0, buf);

    TV.print(0, 8, "Min:");
    itoa(data_min, buf, 10);
    TV.print(30, 8, buf);

    TV.print(60, 8, "Max:");
    itoa(data_max, buf, 10);
    TV.print(90, 8, buf);

    TV.draw_line(0, 20, TV.hres(), 20, 1);

    // plot area is from (5, 25) to (TV.hres()-5, TV.vres()-5)
    byte edge_left = 5;
    byte edge_right = TV.hres() - 5;
    byte edge_top = 25;
    byte edge_bottom = TV.vres() - 5;
    
    byte oldx = edge_left;
    byte oldy = map(samples[current_sample_ix + 1], 0, 255, edge_bottom, edge_top); // TODO make y axis have auto scaling or pre-set scaling?
    for (byte ix = 1; ix < NUM_SAMPLES; ix++)
    {
        byte x = map(ix, 0, NUM_SAMPLES, edge_left, edge_right);
        byte y = map(samples[(ix + current_sample_ix + 1) % NUM_SAMPLES], 0, 255, edge_bottom, edge_top); // TODO make y axis have auto scaling or pre-set scaling?
        TV.draw_line(oldx, oldy, x, y, 1);
        oldx = x;
        oldy = y;
    }
}

unsigned char sampling_function()
{
    // This function actually does the sampling, and returns an unsigned char
    // For now, it just returns a constant 0x55, but it can be updated to
    //   read an analog value, query an I2C thermal sensor, etc
    return (display.frames>>1) & 0xFF;
}

void take_sample()
{
    //TV.tone(2000, 50); // beep when taking each sample

    // be sure to update current_sample_ix, data_mean, data_min, data_max!
    current_sample_ix++;
    if (current_sample_ix >= NUM_SAMPLES)
        current_sample_ix = 0;
    samples[current_sample_ix] = sampling_function();

    // recompute min, max, mean
    data_min = 255;
    data_max = 0;
    unsigned int data_sum = 0;
    for (byte ix = 0; ix < NUM_SAMPLES; ix++)
    {
        data_sum += samples[ix];
        data_min = min(data_min, samples[ix]);
        data_max = max(data_max, samples[ix]);
    }
    data_mean = data_sum / NUM_SAMPLES;
}

void loop()
{
    // TODO add nice title screen here
    TV.clear_screen();
    TV.print(0, 0, "TV Data Collection");
    TV.print(0, 10, "Wayne and Layne");
    TV.delay_frame(120);
    
    TV.clear_screen();
    draw_plot();

    next_sample_time = display.frames; // sample right away

    while (1)
    {
        // wait for the next sample time
        while (display.frames < next_sample_time);

        take_sample();

        draw_plot();

        next_sample_time = display.frames + FRAMES_BETWEEN_SAMPLE;
    }
}

