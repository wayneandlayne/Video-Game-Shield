unsigned char neighbors(unsigned char x, unsigned char y)
{
  unsigned char neighbor_count;

  if (x > 0 && x < XCOLS-1 && y > 0 && y < YCOLS-1)
  {  //this is the middle of the array
    neighbor_count = is_alive(x + 1, y) +
      is_alive(x, (y + 1)) +
      is_alive((x - 1), y) +
      is_alive(x, (y - 1)) +
      is_alive((x + 1), (y + 1)) +
      is_alive((x  - 1) , (y + 1)) +
      is_alive((x  - 1), (y - 1)) +
      is_alive((x + 1), (y - 1));
  } 
  else 
  {  
    neighbor_count = is_alive((x + 1) % XCOLS, y) +
      is_alive(x, (y + 1) % YCOLS) +
      is_alive((x + XCOLS - 1) % XCOLS, y) +
      is_alive(x, (y + YCOLS - 1) % YCOLS) +
      is_alive((x + 1) % XCOLS, (y + 1) % YCOLS) +
      is_alive((x + XCOLS - 1) % XCOLS, (y + 1) % YCOLS) +
      is_alive((x + XCOLS - 1) % XCOLS, (y + YCOLS - 1) % YCOLS) +
      is_alive((x + 1) % XCOLS, (y + YCOLS - 1) % YCOLS);
  } 
  return neighbor_count;
}

void inline copy_from_screen_to_buffer()
{
  for (unsigned char x = 0; x < ARRAY_XCOLS; x++) {
    for (unsigned char y = 0; y < YCOLS; y++) {
      // Default is for cell to stay the same
      life[x][y] = display.screen[x + y*display.hres];
    }
  }
}

void inline copy_from_buffer_to_screen()
{
  for (unsigned char x = 0; x < ARRAY_XCOLS; x++) {
    for (unsigned char y = 0; y < YCOLS; y++) {
      // Default is for cell to stay the same
      display.screen[x + y*display.hres] = life[x][y];
    }
  }
}


long  update_field()
{
  long start_frame = display.frames;


  for (unsigned char x = 0; x < ARRAY_XCOLS; x++)
  {
    for (unsigned char y = 0; y < YCOLS; y++)
    {
      //default is for cell to stay the same
      life[x][y] = display.screen[x+y*display.hres];  
    }
  }

  for (unsigned char x = 0 ; x < XCOLS; x++)
  {
    update_cell(x, 0);
  }

  for (unsigned char y = 1; y < YCOLS-1; y++)
  {


    for (unsigned char i = 0; i < 8; i++)
    {
      update_cell(i,y);
    }

    for (unsigned char x = 1; x < ARRAY_XCOLS-1; x++)
    {
      char current = display.screen[x+y*display.hres];
      char prev = display.screen[(x-1)+y*display.hres];
      char next = display.screen[(x+1)+y*display.hres];

      char top_current = display.screen[x+(y-1)*display.hres];
      char top_prev = display.screen[(x-1)+(y-1)*display.hres];
      char top_next = display.screen[(x+1)+(y-1)*display.hres];

      char bottom_current = display.screen[x+(y+1)*display.hres];
      char bottom_prev = display.screen[(x-1)+(y+1)*display.hres];
      char bottom_next = display.screen[(x+1)+(y+1)*display.hres];

      if (current == 0x00 && prev == 0x00 && next == 0x00)
      {
        if (top_current == 0x00 && top_prev == 0x00 && top_next == 0x00 && bottom_current == 0x00 && bottom_prev == 0x00 && bottom_next == 0x00)
        {
        }
        else
        {
          //we used to have an optimization in here, but it's being tricky.
          life[x][y] = update_cells(prev, current, next, top_prev, top_current, top_next, bottom_prev, bottom_current, bottom_next);

        }
      }

      else
      {
        life[x][y] = update_cells(prev, current, next, top_prev, top_current, top_next, bottom_prev, bottom_current, bottom_next);
      }
    }


    for (int i = (ARRAY_XCOLS-1)*8; i < ARRAY_XCOLS*8; i++)
    {
      update_cell(i, y);
    }


  }

  for (unsigned char x = 0 ; x < XCOLS; x++)
  {
    update_cell(x, YCOLS-1);
  }

  //copy to the display buffer
  for (unsigned char x = 0; x < ARRAY_XCOLS; x++) {
    for (unsigned char y = 0; y < YCOLS; y++) {
      display.screen[x + y*display.hres] = life[x][y];
    }
  }

  //update the array for next time, so we can draw on the screen now.
  //copy_from_screen_to_buffer();

  return display.frames-start_frame;

}

unsigned char update_cells(char prev, char current, char next, char top_prev, char top_current, char top_next, char bottom_prev, char bottom_current, char bottom_next)  //HAVE TO REDO THIS TO MAKE it WORK
{
  char output;

  output = current;

  //first iteration


  //figure out bit 0 of the output unsigned char.  This is the rightmost byte.
  unsigned char count = ((top_next >> 7) & _BV(0)) +
    ((next >> 7) & _BV(0)) +
    ((bottom_next >> 7) & _BV(0)) +
    (top_current & _BV(0)) +
    (bottom_current & _BV(0)) +
    ((top_current >> 1) & _BV(0)) +
    ((bottom_current >> 1) & _BV(0)) +
    ((current >> 1) & _BV(0));

  if (count == 3 && ! (current & _BV(0))) { //is_alive
    // A new cell is born
    output |= _BV(0);
  }

  if ((count < 2 || count > 3) && (current & _BV(0))) {
    // Cell dies
    output &= ~_BV(0);
  }

  //this is a less obvious way of counting the neighbors than using the neighbors function.
  //it's much faster though.
  count -= (top_next >> 7) & _BV(0);
  count -= (bottom_next >> 7) & _BV(0);
  count -= (next >> 7) & _BV(0);
  count += (current & _BV(0));
  count -= (current >> 1) & _BV(0);
  count += (top_current >> (2)) & _BV(0);
  count += (bottom_current >> (2)) & _BV(0);
  count += (current >> (2)) & _BV(0);

  if (count == 3 && ! (current & _BV(1))) { //is_alive
    // A new cell is born
    output |= _BV(1);
  }

  if ((count < 2 || count > 3) && (current & _BV(1))) {
    // Cell dies
    output &= ~_BV(1);
  }

  for (unsigned char i = 2; i < 7; i++)
  {

    //this is a less obvious way of counting the neighbors than using the neighbors function.
    //it's much faster though.
    count -= (top_current >> (i-2)) & _BV(0);
    count -= (current >> (i-2)) & _BV(0);
    count -= (bottom_current >> (i-2)) & _BV(0);
    count += (current >> (i-1)) & _BV(0);
    count -= (current >> (i)) & _BV(0);
    count += (top_current >> (i+1)) & _BV(0);
    count += (bottom_current >> (i+1)) & _BV(0);
    count += (current >> (i+1)) & _BV(0);


    if (count == 3 && ! (current & _BV(i))) { //is_alive
      // A new cell is born
      output |= _BV(i);
    }

    if ((count < 2 || count > 3) && (current & _BV(i))) {
      // Cell dies
      output &= ~_BV(i);
    }
  }

  count -= (top_current >> (5)) & _BV(0);
  count -= (current >> (5)) & _BV(0);
  count -= (bottom_current >> (5)) & _BV(0);
  count += (current >> (6)) & _BV(0);
  count -= (current >> (7)) & _BV(0);
  count += (top_prev & _BV(0));
  count += (prev  & _BV(0));
  count += (bottom_prev  & _BV(0));

  if (count == 3 && ! (current & _BV(7))) { //is_alive
    // A new cell is born
    output |= _BV(7);
  }

  if ((count < 2 || count > 3) && (current & _BV(7))) {
    // Cell dies
    output &= ~_BV(7);
  }

  return output;
}  



unsigned char is_alive(unsigned char x, unsigned char y)
{
  char packed_cell;
  char mask;
  packed_cell = display.screen[x/8 + y*display.hres];
  if (packed_cell & _BV(7-(x%8)))
  {
    return 1;
  } 
  else
  {
    return 0;
  }
}

void set_cell(unsigned char x, unsigned char y)
{
  life[x/8][y] |= 1 << (7-(x % 8));
}

void clear_cell(unsigned char x, unsigned char y)
{
  life[x/8][y] &= ~(1 << (7-(x % 8)));
}

unsigned char update_cell(unsigned char cell_x, unsigned char cell_y)
{
  unsigned char alive;
  alive = 0;

  unsigned char count = neighbors(cell_x, cell_y);

  if (count == 3 && ! is_alive(cell_x, cell_y)) {
    // A new cell is born
    set_cell(cell_x, cell_y);
    alive = 1;
  }
  if ((count < 2 || count > 3) && is_alive(cell_x,cell_y)) {
    // Cell dies
    clear_cell(cell_x, cell_y);
    alive = -1;
  }
  return alive;
}         

void make_glider(unsigned char x, unsigned char y)
{
  set_cell(x, y);
  set_cell(x+1, y+1);
  set_cell(x-1, y+2);
  set_cell(x, y+2);
  set_cell(x+1, y+2);
}


