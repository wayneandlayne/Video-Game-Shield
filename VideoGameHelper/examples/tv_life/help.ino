#include <avr/pgmspace.h>
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

char cells_per_side = 3;
char border_size=2;
char top_right_x = 70;
char top_right_y = 6;
char cell_size=8;
  
void draw_big_square_grid()
{
  char total_size = (cells_per_side)*cell_size+(cells_per_side+1)*border_size-1;
  
  for (int i=0; i <= cells_per_side ; i++)
  {
    //TV.draw_box(top_right_x+cell_size*i+i*(border_size), top_right_y, border_size-1, total_size, 1, 1, 0, 0);
    TV.draw_rect(top_right_x+cell_size*i+i*(border_size), top_right_y, border_size-1, total_size, 1, 1);
    
    //TV.draw_box(top_right_x, top_right_y+cell_size*i+i*(border_size), total_size, border_size, 1, 1, 0, 0);
    TV.draw_rect(top_right_x, top_right_y+cell_size*i+i*(border_size), total_size, border_size, 1, 1);
  }
}

void clear_big_square_grid()
{
  char total_size = (cells_per_side)*cell_size+(cells_per_side+1)*border_size-1;
  //TV.draw_box(top_right_x, top_right_y, total_size, total_size, 0, 0, 0, 0);
  TV.draw_rect(top_right_x, top_right_y, total_size, total_size, 0, 0);
}


void set_big_square_alive(char x, char y)
{
  //TV.draw_box(top_right_x+cell_size*x+(x+1)*(border_size), top_right_y+cell_size*y+(y+1)*(border_size), cell_size-1, cell_size, 1, 1, 0, 0);
  TV.draw_rect(top_right_x+cell_size*x+(x+1)*(border_size), top_right_y+cell_size*y+(y+1)*(border_size), cell_size-1, cell_size, 1, 1);

}

void set_big_square_dead(char x, char y)
{
  //TV.draw_box(top_right_x+cell_size*x+(x+1)*(border_size), top_right_y+cell_size*y+(y+1)*(border_size), cell_size-1, cell_size, 0, 0, 0, 0);
  TV.draw_rect(top_right_x+cell_size*x+(x+1)*(border_size), top_right_y+cell_size*y+(y+1)*(border_size), cell_size-1, cell_size, 0, 0);

}
  

void explanation_screen()
{
  
  long base_delay = 60*6;
  cells_per_side = 3;
  border_size=2;
  top_right_x = 70;
  top_right_y = 6;
  cell_size=8;

  
  TV.select_font(font6x8);
  TV.clear_screen();
  TV.printPGM(0, 0, PSTR("    Conway's Game of"));
  TV.printPGM(0, 9*1, PSTR("Life is a \"cellular") );
  TV.printPGM(0, 9*2, PSTR("automaton\" developed"));
  TV.printPGM(0, 9*3, PSTR("by the British"));
  TV.printPGM(0, 9*4, PSTR("mathematician John"));
  TV.printPGM(0, 9*5, PSTR("Conway in 1970."));
  delay_frame_or_break_for_z(&nunchuck, base_delay);
  TV.clear_screen();

  
  TV.printPGM(0, 0, PSTR("The rules are simple"));
  TV.printPGM(0, 9*1, PSTR("but the results are"));
  TV.printPGM(0, 9*2, PSTR("often surprising."));
  delay_frame_or_break_for_z(&nunchuck, base_delay);
  
  TV.clear_screen();
  TV.printPGM(0, 7*0, PSTR("The world consists of"));
  TV.printPGM(0, 7*1, PSTR("a grid, with each"));
  TV.printPGM(0, 7*2, PSTR("cell being either"));
  TV.printPGM(0, 7*3, PSTR("alive or dead."));
  
  TV.printPGM(0, 7*5, PSTR("Live cells are white,"));
  TV.printPGM(0, 7*6, PSTR("while dead cells are"));
  TV.printPGM(0, 7*7, PSTR("white."));

  delay_frame_or_break_for_z(&nunchuck, base_delay);  
  
  TV.clear_screen();
  TV.printPGM(0, 7*1, PSTR("Every cell"));
  TV.printPGM(0, 7*2, PSTR("has eight"));
  TV.printPGM(0, 7*3, PSTR("neighbors."));
  
  draw_big_square_grid();
  delay_frame_or_break_for_z(&nunchuck, base_delay/2);
  set_big_square_alive(0,0);
  delay_frame_or_break_for_z(&nunchuck, base_delay/18);
  set_big_square_dead(0,0);
  set_big_square_alive(1,0);
  delay_frame_or_break_for_z(&nunchuck, base_delay/18);
  set_big_square_dead(1,0);
  set_big_square_alive(2,0);
  delay_frame_or_break_for_z(&nunchuck, base_delay/18);
  set_big_square_dead(2,0);
  set_big_square_alive(2,1);
  delay_frame_or_break_for_z(&nunchuck, base_delay/18);
  set_big_square_dead(2,1);
  set_big_square_alive(2,2);
  delay_frame_or_break_for_z(&nunchuck, base_delay/18);
  set_big_square_dead(2,2);
  set_big_square_alive(1,2);
  delay_frame_or_break_for_z(&nunchuck, base_delay/18);
  set_big_square_dead(1,2);
  set_big_square_alive(0,2);
  delay_frame_or_break_for_z(&nunchuck, base_delay/18);
  set_big_square_dead(0,2);
  set_big_square_alive(0,1);
  delay_frame_or_break_for_z(&nunchuck, base_delay/18);
  set_big_square_dead(0,1);
  delay_frame_or_break_for_z(&nunchuck, base_delay/9);

  TV.clear_screen();
  TV.printPGM(0, 9*0, PSTR("Every generation,"));
  TV.printPGM(0, 9*1, PSTR("each cell either "));
  TV.printPGM(0, 9*2, PSTR("dies, lives or is"));
  TV.printPGM(0, 9*3, PSTR("born based upon "));
  TV.printPGM(0, 9*4, PSTR("its neighbors."));
  delay_frame_or_break_for_z(&nunchuck, base_delay);

  TV.clear_screen();
  delay_frame_or_break_for_z(&nunchuck, base_delay/10);
  
  top_right_x=6;
  top_right_y=6;
  draw_big_square_grid();
  set_big_square_alive(1,1);
  set_big_square_alive(2,2);
  
  
  TV.printPGM(44, 9*0, PSTR("Any live"));
  TV.printPGM(44, 9*1, PSTR("cell with"));
  TV.printPGM(44, 9*2, PSTR("one or"));
  TV.printPGM(44, 9*3, PSTR("fewer live"));
  TV.printPGM(44, 9*4, PSTR("neighbors"));  
  TV.printPGM(44, 9*5, PSTR("dies."));
  delay_frame_or_break_for_z(&nunchuck, 3*base_delay/4);
  set_big_square_dead(1,1);
  delay_frame_or_break_for_z(&nunchuck, base_delay);
  
  TV.clear_screen();
  delay_frame_or_break_for_z(&nunchuck, base_delay/10);
  draw_big_square_grid();
  set_big_square_alive(0,1);
  set_big_square_alive(1,1);
  set_big_square_alive(2,2);
  set_big_square_alive(2,1);
  set_big_square_alive(1,2);
  TV.printPGM(44, 9*1, PSTR("Any live"));
  TV.printPGM(44, 9*2, PSTR("cell with"));
  TV.printPGM(44, 9*3, PSTR("more than"));
  TV.printPGM(44, 9*4, PSTR("three live"));
  TV.printPGM(44, 9*5, PSTR("neighbors"));  
  TV.printPGM(44, 9*6, PSTR("dies."));
  delay_frame_or_break_for_z(&nunchuck, 3*base_delay/4);
  set_big_square_dead(1,1);
  delay_frame_or_break_for_z(&nunchuck, base_delay);

  TV.clear_screen();
  delay_frame_or_break_for_z(&nunchuck, base_delay/10);
  draw_big_square_grid();
  set_big_square_alive(0,1);
  set_big_square_alive(1,1);
  set_big_square_alive(2,1);
  TV.printPGM(44, 9*0, PSTR("Any live"));
  TV.printPGM(44, 9*1, PSTR("cell with"));
  TV.printPGM(44, 9*2, PSTR("two or "));
  TV.printPGM(44, 9*3, PSTR("three live"));
  TV.printPGM(44, 9*4, PSTR("neighbors"));  
  TV.printPGM(44, 9*5, PSTR("lives on."));
  delay_frame_or_break_for_z(&nunchuck, base_delay);
  
  
  TV.clear_screen();
  delay_frame_or_break_for_z(&nunchuck,base_delay/10);
  draw_big_square_grid();
  set_big_square_alive(0,1);
  set_big_square_alive(2,1);
  set_big_square_alive(1,2);
  TV.printPGM(44, 9*0, PSTR("Any dead"));
  TV.printPGM(44, 9*1, PSTR("cell with"));
  TV.printPGM(44, 9*2, PSTR("exactly "));
  TV.printPGM(44, 9*3, PSTR("three live"));
  TV.printPGM(44, 9*4, PSTR("neighbors"));  
  TV.printPGM(44, 9*5, PSTR("is born."));
  delay_frame_or_break_for_z(&nunchuck, 3*base_delay/4);
  set_big_square_alive(1,1);
  delay_frame_or_break_for_z(&nunchuck, base_delay);
 
  
  TV.clear_screen();
  delay_frame_or_break_for_z(&nunchuck, base_delay/10);
  TV.printPGM(0, 9*0, PSTR("All these changes"));
  TV.printPGM(0, 9*1, PSTR("occur at the same"));
  TV.printPGM(0, 9*2, PSTR("time, and the new"));
  TV.printPGM(0, 9*3, PSTR("field is redrawn."));
  delay_frame_or_break_for_z(&nunchuck, base_delay);
}
