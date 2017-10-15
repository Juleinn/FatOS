#include "print.h"

void print_char(unsigned char c, unsigned char color, int x, int y)
{
	char *  vram = (char*) VIDEO_MEMORY;
	int index = ((y * MAX_COLS) + x) * 2;	// two bytes per displayer char
	vram[index] = (unsigned char) c;
	vram[index + 1] = color;
}

void print_int(int v, unsigned char color, int x, int y)
{
	if(v == 0)
	{
		print_char('0', color, x, y);
		return;
	}

	// print in reverse
	int digitPos = x + get_digit_count(v)-1;
	while(v > 0)
	{
		print_char((char) (v % 10) + '0', color, digitPos, y);
		digitPos--;
		v /= 10;
	}
}

int ten_power(int x)
{
	int i, result = 1;
	for(i=0;i<x;i++)
	{
		result *= 10;
	}
	return result;
}

int get_digit_count(int x)
{
	int digitCount = 0;
	while(x > 0)
	{
		digitCount++;
		x /= 10;
	}
	return digitCount;
}

void clear_screen()
{
	// just clear out the characters, leave color
	unsigned char* vram = (char*) VIDEO_MEMORY;
	#define MAX_VRAM VIDEO_MEMORY + 4000
	while(vram < (unsigned char*) MAX_VRAM)
	{
		*vram = ' ';
		vram+=2;
	}
}

void print_string(unsigned char *str, char color, int x, int y)
{
	while(*str)
	{
		print_char(*str, color, x, y);


		str++;
		x++;
	}
}

void set_screen(unsigned char c, unsigned char color)
{
	// just clear out the characters, leave color
	unsigned char* vram = (char*) VIDEO_MEMORY;
	#define MAX_VRAM VIDEO_MEMORY + 4000
	while(vram < (unsigned char*) MAX_VRAM)
	{
		*vram = c;
		*(vram+1) = color;
		vram+=2;
	}
}

/* ************ Scrollable text output ************ */

/* Current cursor position. incremented everytime a character is printed */
unsigned int text_x = 0, text_y = 0;

/* Current text color. Default = white over black */
unsigned char text_color = 0x0F;

void scroll()
{
	/* copy all data one line up. one line = MAX_COLS*2 bytes, and we want to copy MAX_COLS * (MAX_ROWS-1) * 2 bytes */
	unsigned char* source = (unsigned char*) (VIDEO_MEMORY + (MAX_COLS*2));
	unsigned char* dest = (unsigned char*) VIDEO_MEMORY;
	memcpy(source, dest, MAX_COLS * (MAX_ROWS - 1) * 2);

	/* Then wipe the last line's characters with ' '. Do not wipe color data */
	source = (unsigned char*) (VIDEO_MEMORY + (MAX_COLS * (MAX_ROWS-1) * 2));
	int i;
	for(i=0;i<MAX_COLS*2; i++)
	{
		*source = ' ';
		source += 2;
	}
}

void printc(char c)
{
	if(c == 0)
		return;

	if(c == '\n')
	{
		nwline();
		return;
	}

	print_char(c, text_color, text_x, text_y);
	text_x++;
	if(text_x >= MAX_COLS)
		nwline();
}

void nwline()
{
	if(text_y >= MAX_ROWS - 1)	// scroll everything
	{
		scroll();
	}
	else
	{
		text_y++;
	}
	text_x = 0;
}

void prints(char* s)
{
	while(*s)
	{
		printc(*s);
		s++;
	}
}

void init_text()
{
	clear_screen();
	text_x = 0;
	text_y = 0;
	text_color = 0x0F;
}

void set_cursor_pos(unsigned short x, unsigned short y)
{
	// clamp values
	x = x < 0 ? 0 : (x >= MAX_COLS ? MAX_COLS-1 : x);
	y = y < 0 ? 0 : (y >= MAX_ROWS ? MAX_ROWS-1 : y);

	text_x = x;
	text_y = y;
}

void set_color(unsigned char color)
{
	text_color = color;
}

unsigned char get_color()
{
	return text_color;
}

void printi(int x)
{
	if(x<0)
	{
		printc('-');
		x *= -1;
	}

	if(x == 0)
	{
		printc('0');
		return;
	}

	int divider = ten_power(get_digit_count(x) - 1);
	while(divider > 0)
	{
		printc((x / divider) + '0');
		x = x % divider;
		divider /= 10;
	}
}

void printh(int x)
{
	int i;
	prints("0x");
	for(i=0; i<8; i++)
	{
		int displayvalue = (((x << 4*i) & 0xF0000000) >> 28);
		if(displayvalue < 10)
			printc((char) (displayvalue + '0'));
		else
			printc((char) (displayvalue - 10 + (int)'a'));
	}
}

void printic(int x, unsigned char color)
{
	unsigned char saved_color = text_color;
	set_color(color);
	printi(x);
	set_color(saved_color);
}

void backspace()
{
	if(text_x == 0 && text_y != 0)
	{
		text_y--;
		text_x = MAX_COLS-1;
	}
	else
	{
		text_x--;
	}
	print_char(' ', text_color, text_x, text_y);
}
