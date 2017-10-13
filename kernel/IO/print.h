/* Screen printing functions */
#ifndef PRINT_H_
#define PRINT_H_

#define MAX_COLS 80
#define MAX_ROWS 25
#define VIDEO_MEMORY 0xb8000

#include "../memory.h"

void print_char(unsigned char c, unsigned char color, int x, int y);
int get_digit_count(int x);
void print_int(int v, unsigned char color, int x, int y);
int ten_power(int x);
void clear_screen();
void set_screen(unsigned char c, unsigned char color);
void print_string(unsigned char *str, char color, int x, int y);

/* Scroll the given number of lines 
 * Freed line at end of screen will be full of ' ' */
void scroll();

/*Prints the given character at the current position. if character = \n then interpreted as new linefeed */
void printc(char c);

/* Gives a new linefeed. if at bottom, scrolls everything up */
void nwline();

/* Prints a string at the current position. */
void prints(char* s);

/* Inits the text output */
void init_text();

/* Sets current text pos */
void set_cursor_pos(unsigned short x, unsigned short y);

void set_color(unsigned char color);

/* Prints an integer in dec. format at pos */
void printi(int x);

/* Handles backspaces */
void backspace();

/* Prints an unsigned integer value in hex format /!\Buggy !!*/
void printh(unsigned int x);

#endif