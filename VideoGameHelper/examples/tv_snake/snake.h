//This file is a struct to hold the Snake information.  In Arduino, (at one point at least), you had to put struct declarations used in function calls in a separate file.

struct Snake {
  unsigned char x;
  unsigned char y;
  signed char dir_x;
  signed char dir_y;
  Nunchuck* nunchuck;
};

void handle_nunchuck(byte* data, Snake* snake);
