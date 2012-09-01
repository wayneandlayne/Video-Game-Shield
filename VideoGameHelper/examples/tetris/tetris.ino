#include <TVout.h>
#include <fontALL.h>
#include "stc.h"
#include "platform.h"
#include "bitmaps.h"
#include "tones.h"
#include <avr/pgmspace.h>

#include <i2cmaster.h>

//#define HACKVISION  // compile this for a hackvision
//#define VGS_CC  // compile this for a video game shield with a Wii classic controller on P1
#define VGS_NUNCHUCK // compile this for a video game shield with a Wii nunchuck controller on P1

//#define DISABLE_INTRO

#ifdef HACKVISION
#define STARTMSG "Press Fire"
#include <ButtonController.h>
#endif

#ifdef VGS_CC
#include <ClassicController.h>
ClassicController cc;
#define STARTMSG "Press Start"
#endif

#ifdef VGS_NUNCHUCK
#include <nunchuck.h>
Nunchuck n;
#define STARTMSG "Press C"
#endif

//BOARD_X and NXT_X must be divisable by 8!!!!!!!!
#define BOARD_X (24)
#define BOARD_Y (0)
#define NXT_X (0)
#define NXT_Y (8)

// this must be for for the current render impimentation
#define TILE_SIZE (4)

#define W (112)
#define H (90)

TVout TV;
StcGame game;
char prev = 0;
char restarted = 0;

void intro();
void printboard();
void setCell(char x, char y,char bx, char by, char c, char f);

void setup() {
  int error;

  TV.begin(_NTSC,W,H);
  TV.select_font(font6x8);
#ifdef VGS_CC
  if (cc.begin(WII_PLAYER_1)) {
    TV.print("CC begin error");
    while(1);
  }
  //ccpoll = 0;
#elif defined(VGS_NUNCHUCK)
  if (n.begin(NUNCHUCK_PLAYER_1)) {
    TV.print("Nunchuck begin error");
    while(1);
  }
#endif
#ifndef DISABLE_INTRO
  intro();
  TV.clear_screen();
  TV.select_font(font8x8);
  TV.printPGM(28,35,PSTR("Arduino"));
  TV.printPGM(32,43,PSTR("TETRIS"));
  TV.select_font(font6x8);
  TV.delay(500);
  TV.printPGM(25,60,PSTR(STARTMSG));
  

  

#ifdef HACKVISION
  TV.printPGM(0,H-16,PSTR("To pause, press"));
  TV.printPGM(0,H-8,PSTR("< & > together"));
#endif
  int time = TV.millis() + 2000;
  //yeah yeah bad form bla bla
  while(1) {
#ifdef HACKVISION
  if (Controller.firePressed()) {
    while (Controller.firePressed());
    break;
  }
#elif defined(VGS_CC)
    cc.update();
    if (cc.button_plus_start()) {
      while (cc.button_plus_start())
        cc.update();
      break;
    }
#elif defined(VGS_NUNCHUCK)
    n.update();
    if (n.button_c()) {
         while (n.button_c())
           n.update();
         break;
    }
#endif
  }
#endif
  TV.delay(500);
  gameInit(&game);
}

void loop() {
  gameUpdate(&game);
}

void intro() {
unsigned char w,l,wb;
  int index;
  w = pgm_read_byte(TVOlogo);
  l = pgm_read_byte(TVOlogo+1);
  if (w&7)
    wb = w/8 + 1;
  else
    wb = w/8;
  index = wb*(l-1) + 2;
  for ( unsigned char i = 1; i < l; i++ ) {
    TV.bitmap((TV.hres() - w)/2,0,TVOlogo,index,w,i);
    index-= wb;
    TV.delay(50);
#ifdef HACKVISION
  if (Controller.firePressed())
    return;
#elif defined(VGS_CC)
    cc.update();
    if (cc.button_plus_start())
      return;
#elif defined(VGS_Nunchuck)
    n.update();
    if (n.button_c())
      return;
#endif
  }
  for (unsigned char i = 0; i < (TV.vres() - l)/2; i++) {
    TV.bitmap((TV.hres() - w)/2,i,TVOlogo);
    TV.delay(50);
#ifdef HACKVISION
  if (Controller.firePressed())
    return;
#elif defined(VGS_CC)
    cc.update();
    if (cc.button_plus_start())
      return;
#elif defined(VGS_NUNCHUCK)
    n.update();
    if (n.button_c())
      return;
#endif
  }
  int time = TV.millis() + 2000;
  while(time > TV.millis()) {
#ifdef HACKVISION
  if (Controller.firePressed())
    return;
#elif defined(VGS_CC)
    cc.update();
    if (cc.button_plus_start())
      return;
#elif defined(VGS_NUNCHUCK)
    n.update();
    if (n.button_c())
      return;
#endif
  }
}

//define these for the arduino platform
int platformInit(StcGame *gameInstance) {
  printboard();
  return 0;
}

void printboard() {
  TV.clear_screen();

  // label the next piece
  TV.printPGM(NXT_X,NXT_Y-8,PSTR("Next"));
  
  //print arduino tetris vertically allong the left
  TV.select_font(font8x8);
  TV.printPGM(0,NXT_Y+TETROMINO_SIZE*TILE_SIZE+8,PSTR("A\nrT\ndE\nuT\niR\nnI\noS"));
  TV.select_font(font6x8);
  TV.bitmap(W-39,H-40,TVoutMini);

  // label the Score:
  TV.printPGM(BOARD_X+BOARD_TILEMAP_WIDTH*TILE_SIZE+4,0,PSTR("lvl"));
  TV.printPGM(BOARD_X+BOARD_TILEMAP_WIDTH*TILE_SIZE+4,12,PSTR("Score"));
  TV.printPGM(BOARD_X+BOARD_TILEMAP_WIDTH*TILE_SIZE+4,28,PSTR("Lines"));

  //draw the game boarders
  TV.draw_line(BOARD_X-2,BOARD_Y, BOARD_X-2, BOARD_Y+BOARD_TILEMAP_HEIGHT*TILE_SIZE, WHITE);
  TV.draw_line(BOARD_X+BOARD_TILEMAP_WIDTH*TILE_SIZE,
               BOARD_Y,BOARD_X+BOARD_TILEMAP_WIDTH*TILE_SIZE,
               BOARD_Y+BOARD_TILEMAP_HEIGHT*TILE_SIZE,
               WHITE);
  TV.draw_line(BOARD_X-2,
               BOARD_Y+BOARD_TILEMAP_HEIGHT*TILE_SIZE+1,
               BOARD_X+BOARD_TILEMAP_WIDTH*TILE_SIZE,
               BOARD_Y+BOARD_TILEMAP_HEIGHT*TILE_SIZE+1,
               WHITE);
}
void platformEnd(StcGame *gameInstance) {
}

//modify this to read controller inputs
//there is something wrong in here but there is temp fix at the end
void platformReadInput(StcGame *gameInstance){
#ifdef HACKVISION

#define LEFTBIT 1
#define RIGHTBIT 2
#define UPBIT 4
#define DOWNBIT 8
#define FIREBIT 16
#define PAUSBIT 32
#define RESBIT 64

#define PLEFT (prev &LEFTBIT)
#define PRIGHT (prev & RIGHTBIT)
#define PDOWN (prev &DOWNBIT)
#define PUP (prev & UPBIT)
#define PFIRE (prev & FIREBIT)
#define PPAUSE (prev & PAUSBIT)
#define PRES (prev & RESBIT)
  if (Controller.leftPressed() && Controller.rightPressed()) {
    if (!PPAUSE) {
      gameOnKeyDown(&game, EVENT_PAUSE);
      prev |= PAUSBIT;
    }
  }
  else if (PAUSBIT) {
    if (!Controller.leftPressed() && !Controller.rightPressed()) {
      prev &= ~PAUSBIT;
    }
  }
  
  if (game.isOver && Controller.firePressed())
    prev |= RESBIT;
  else if (PRES) {
    if (!Controller.firePressed()) {
      gameOnKeyDown(&game, EVENT_RESTART);
      prev &= ~RESBIT;
    }
  }
  
  if (Controller.upPressed()) {
    if (!PUP) {
      gameOnKeyDown(&game, EVENT_ROTATE_CW);
      prev |= UPBIT;
    }
  }
  else if (PUP){
    prev &= ~UPBIT;
  }
 
  if (!game.isOver && Controller.firePressed()) {
    if (!PFIRE) {
      gameOnKeyDown(&game, EVENT_DROP);
      prev |= FIREBIT;
    }
  }
  else if (PFIRE) {
    prev &= ~FIREBIT;
  }
 
  if (Controller.leftPressed() && !PPAUSE) {
    if (!PLEFT) {
      gameOnKeyDown(&game, EVENT_MOVE_LEFT);
      prev |= LEFTBIT;
    }
  }
  else if (PLEFT) {
    gameOnKeyUp(&game, EVENT_MOVE_LEFT);
    prev &= ~LEFTBIT;
  }
  
  if (Controller.rightPressed() && !PPAUSE) {
    if (!PRIGHT) {
      gameOnKeyDown(&game, EVENT_MOVE_RIGHT);
      prev |= RIGHTBIT;
    }
  }
  else if (PRIGHT) {
    gameOnKeyUp(&game, EVENT_MOVE_RIGHT);
    prev &= ~RIGHTBIT;
  }
  
  if (Controller.downPressed()) {
    if (!PDOWN) {
      gameOnKeyDown(&game, EVENT_MOVE_DOWN);
      prev |= DOWNBIT;
    }
  }
  else if (PDOWN) {
    gameOnKeyUp(&game, EVENT_MOVE_DOWN);
    prev &= ~DOWNBIT;
  }

#elif defined(VGS_CC)

#define LEFTBIT 1
#define RIGHTBIT 2
#define UPBIT 4
#define DOWNBIT 8
#define ABIT 16
#define STARTBIT 32
#define SELECTBIT 64

#define PLEFT (prev &LEFTBIT)
#define PRIGHT (prev & RIGHTBIT)
#define PDOWN (prev &DOWNBIT)
#define PUP (prev & UPBIT)
#define PA (prev & ABIT)
#define PSTART (prev & STARTBIT)
#define PSELECT (prev & SELECTBIT)

  cc.update();
  
  if (cc.button_up()) {
    if (!PUP) {
      gameOnKeyDown(&game, EVENT_ROTATE_CW);
      prev |= UPBIT;
    }
  }
  else if (PUP){
    prev &= ~UPBIT;
  }
  
  if (cc.button_plus_start()) {
    if (!PSTART) {
      gameOnKeyDown(&game, EVENT_PAUSE);
      prev |= STARTBIT;
    }
  }
  else if (PSTART){
    prev &= ~STARTBIT;
  }
  
  if (cc.button_minus_select()) {
    if (!PSELECT) {
      gameOnKeyDown(&game, EVENT_RESTART);
      prev |= SELECTBIT;
    }
  }
  else if (PSELECT){
    prev &= ~SELECTBIT;
  }
  
  if (cc.button_a()) {
    if (!PA) {
      gameOnKeyDown(&game, EVENT_DROP);
      prev |= ABIT;
    }
  }
  else if (PA) {
    prev &= ~ABIT;
  }
  
  if (cc.button_left()) {
    if (!PLEFT) {
      gameOnKeyDown(&game, EVENT_MOVE_LEFT);
      prev |= LEFTBIT;
    }
  }
  else if (PLEFT) {
    gameOnKeyUp(&game, EVENT_MOVE_LEFT);
    prev &= ~LEFTBIT;
  }
  
  if (cc.button_right()) {
    if (!PRIGHT) {
      gameOnKeyDown(&game, EVENT_MOVE_RIGHT);
      prev |= RIGHTBIT;
    }
  }
  else if (PRIGHT) {
    gameOnKeyUp(&game, EVENT_MOVE_RIGHT);
    prev &= ~RIGHTBIT;
  }
  
  if (cc.button_down()) {
    if (!PDOWN) {
      gameOnKeyDown(&game, EVENT_MOVE_DOWN);
      prev |= DOWNBIT;
    }
  }
  else if (PDOWN) {
    gameOnKeyUp(&game, EVENT_MOVE_DOWN);
    prev &= ~DOWNBIT;
  }
#elif defined(VGS_NUNCHUCK)
#define LEFTBIT 1
#define RIGHTBIT 2
#define UPBIT 4
#define DOWNBIT 8
#define ABIT 16
#define STARTBIT 32
#define SELECTBIT 64

#define PLEFT (prev &LEFTBIT)
#define PRIGHT (prev & RIGHTBIT)
#define PDOWN (prev &DOWNBIT)
#define PUP (prev & UPBIT)
#define PA (prev & ABIT)
#define PSTART (prev & STARTBIT)
#define PSELECT (prev & SELECTBIT)

  n.update();
  
  if (n.joy_up()) {
    if (!PUP) {
      gameOnKeyDown(&game, EVENT_ROTATE_CW);
      prev |= UPBIT;
    }
  }
  else if (PUP){
    prev &= ~UPBIT;
  }
  
  if (n.button_c()) {
    if (!PSTART) {
      gameOnKeyDown(&game, EVENT_PAUSE);
      prev |= STARTBIT;
    }
  }
  else if (PSTART){
    prev &= ~STARTBIT;
  }

  if (n.button_c()) {
    if (!PSELECT) {
      gameOnKeyDown(&game, EVENT_RESTART);
      prev |= SELECTBIT;
    }
  }
  else if (PSELECT){
    prev &= ~SELECTBIT;
  }
  
  if (n.button_z()) {
    if (!PA) {
      gameOnKeyDown(&game, EVENT_DROP);
      prev |= ABIT;
    }
  }
  else if (PA) {
    prev &= ~ABIT;
  }
  
  if (n.joy_left()) {
    if (!PLEFT) {
      gameOnKeyDown(&game, EVENT_MOVE_LEFT);
      prev |= LEFTBIT;
    }
  }
  else if (PLEFT) {
    gameOnKeyUp(&game, EVENT_MOVE_LEFT);
    prev &= ~LEFTBIT;
  }
  
  if (n.joy_right()) {
    if (!PRIGHT) {
      gameOnKeyDown(&game, EVENT_MOVE_RIGHT);
      prev |= RIGHTBIT;
    }
  }
  else if (PRIGHT) {
    gameOnKeyUp(&game, EVENT_MOVE_RIGHT);
    prev &= ~RIGHTBIT;
  }
  
  if (n.joy_down()) {
    if (!PDOWN) {
      gameOnKeyDown(&game, EVENT_MOVE_DOWN);
      prev |= DOWNBIT;
    }
  }
  else if (PDOWN) {
    gameOnKeyUp(&game, EVENT_MOVE_DOWN);
    prev &= ~DOWNBIT;
  }
#endif

}

  

void setCell(char x, char y,char bx, char by, char c, char f) {
  int index = bx/8 + x/2 + by*W/8 + y*4*W/8;
  if (c) {
    if (x & 1) {
      TV.screen[index] &= 0b11110000;
      TV.screen[index] |= 0b00001110;
      index+=W/8;
      TV.screen[index] &= 0b11110000;
      TV.screen[index] |= 0b00001010;
      if (f) TV.screen[index] &= 0b11111011;
      index+=W/8;
      TV.screen[index] &= 0b11110000;
      TV.screen[index] |= 0b00001110;
      index+=W/8;
      TV.screen[index] &= 0b11110000;
    }
    else {
      TV.screen[index] &= 0b00001111;
      TV.screen[index] |= 0b11100000;
      index+=W/8;
      TV.screen[index] &= 0b00001111;
      TV.screen[index] |= 0b10100000;
      if (f) TV.screen[index] &= 0b10111111;
      index+=W/8;
      TV.screen[index] &= 0b00001111;
      TV.screen[index] |= 0b11100000;
      index+=W/8;
      TV.screen[index] &= 0b00001111;
    }
  }
  else {
    if (x & 1) {
      TV.screen[index] &= 0b11110000;
      index+=W/8;
      TV.screen[index] &= 0b11110000;
      if (f) TV.screen[index] |= 0b00000100;
      index+=W/8;
      TV.screen[index] &= 0b11110000;
      index+=W/8;
      TV.screen[index] &= 0b11110000;
    }
    else {
      TV.screen[index] &= 0b00001111;
      index+=W/8;
      TV.screen[index] &= 0b00001111;
      if (f) TV.screen[index] |= 0b01000000;
      index+=W/8;
      TV.screen[index] &= 0b00001111;
      index+=W/8;
      TV.screen[index] &= 0b00001111;
    }
  }
}

void update_music()
{
  static unsigned char current_note = 0;
  static long next_note_start_time = 0;
  if (TV.millis() >= next_note_start_time)
  {
    if (current_note == SONG_LENGTH)
    {
       //it's the end of the song!
       current_note = 0;    
    }
    
      int note_duration = (pgm_read_word_near(duration_t1_c1 + current_note)/16)*5;
      next_note_start_time = TV.millis()+note_duration+PAUSE_BETWEEN_NOTES;
      TV.tone(pgm_read_word_near(melody_t1_c1 + current_note), note_duration); 
    if (current_note == SONG_LENGTH-1)
    {
      next_note_start_time += PAUSE_BETWEEN_SONGS;
      next_note_start_time -= PAUSE_BETWEEN_NOTES;
    }
    current_note++;
    
  }
}


//modify this to render the game TVout redering is defined here.
void platformRenderGame(StcGame *gameInstance) {
  char i,j;
  TV.delay_frame(1);
if (!game.isPaused && !game.isOver) {
    //draw the static blocks
    for (i = 0; i < BOARD_TILEMAP_WIDTH; i++) {
      for (j = 0; j < BOARD_TILEMAP_HEIGHT; j++) {
#ifdef STC_SHOW_GHOST_PIECE
        setCell(i,j,BOARD_X,BOARD_Y,game.map[i][j],0);
#else
        setCell(i,j,BOARD_X,BOARD_Y,game.map[i][j],1);
#endif
      }
    }
    update_music();
  
#ifdef STC_SHOW_GHOST_PIECE
    for (i = 0; i < TETROMINO_SIZE; i++) {
      for (j = 0; j < TETROMINO_SIZE; j++) {
        if (game.fallingBlock.cells[i][j] != EMPTY_CELL)
          setCell(game.fallingBlock.x + i, game.fallingBlock.y + game.shadowGap + j, BOARD_X, BOARD_Y, 0, 1);
       }
     }
#endif

    // falling block
    for (i = 0; i < TETROMINO_SIZE; i++) {
      for (j = 0; j < TETROMINO_SIZE; j++) {
        if (game.fallingBlock.cells[i][j]) {
          setCell(game.fallingBlock.x + i,game.fallingBlock.y + j,BOARD_X,BOARD_Y,1,1);
        }
      }
    }

    // nextblock block
    TV.draw_rect(NXT_X,NXT_Y,TETROMINO_SIZE*TILE_SIZE,TETROMINO_SIZE*TILE_SIZE,BLACK,BLACK);
    for (i = 0; i < TETROMINO_SIZE; i++) {
      for (j = 0; j < TETROMINO_SIZE; j++) {
        setCell (game.nextBlock.x + i+1,game.nextBlock.y + j+1,NXT_X,NXT_Y,game.nextBlock.cells[i][j],0);
      }
    }
  
    //update the stats
    TV.print(BOARD_X+BOARD_TILEMAP_WIDTH*TILE_SIZE+26,0,game.stats.level);
    TV.print(BOARD_X+BOARD_TILEMAP_WIDTH*TILE_SIZE+10,20,game.stats.score);
    TV.print(BOARD_X+BOARD_TILEMAP_WIDTH*TILE_SIZE+10,36,game.stats.lines);
  }
  
  if (game.isPaused)
    TV.printPGM(BOARD_X+4,BOARD_Y+40,PSTR("Pause"));
  if (game.isOver) {
    TV.printPGM(BOARD_X+7,BOARD_Y+36,PSTR("Game"));
    TV.printPGM(BOARD_X+7,BOARD_Y+44,PSTR("Over"));
    restarted = 1;
  }
  
  if (restarted == 1 && game.isOver == 0) {
    printboard();
    restarted = 0;
  }
  
  game.stateChanged = 0;
 
}

//return millis for this since this uses TVout we need to return TVouts version of millis.
long platformGetSystemTime() {
  return TV.millis();
}

void platformSeedRandom(long seed) {
  srand(analogRead(0)*seed);
}

int platformRandom() {
  return rand();
}
