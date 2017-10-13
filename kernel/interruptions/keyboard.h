/* file keyboard.h 
 * Anton CLAES
 * Contains keyboard handling functions */

/* Called when a keyboard interrupt occurs */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "io.h"

void kbd_irq_handler();

/*Gets ascii code of given key */
char get_ascii(char keycode);

/* Inits ascii translation */
void keyboard_init();

/* led control */
void set_leds();

void cut_leds();

/* gets if the char is printable  */
unsigned char is_printable(char keycode);

/* Function that allows user to wait for a character entry */
char wait_char();

/* Allows to fill a text buffer with a user line-entry
 Returns the number of chars actually read */
int getline(char* buffer, int count);

#endif