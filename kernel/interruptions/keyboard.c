#include "keyboard.h"

/* Some handy keycode constants */
#define CAPSLOCK 58
#define LSHIFT 42

#define BACKSPACE 14

#define NWLINE 28

/* Stores the current keystates of the keys. 1=pressed 0=up*/
static char capsLock = 0;
static char shifted = 0;

static const char table_lowercase[200] = "zz1234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'\\\0\0zxcvbnm,./\0*\0 ";
static const char table_printable[200] = "nnyyyyyyyyyyyynnyyyyyyyyyyyynnyyyyyyyyyyyynnyyyyyyyyyynyny";
static const char table_uppercase[200] = "zz!@#$ ^&*()_+\b\tQWERTYUIOP{}\n\0ASDFGHJKL:\"|\0\0ZXCVBNM<>?\0*\0 ";

char keyboard_data_available = 0;
char keyboard_data = 0;

void kbd_irq_handler()
{
	char keycode = inb(0x60);
  unsigned char id = keycode & 0x7F;

  if(keycode & 0x80)
  {
      /* Keyup */
    shifted = id == LSHIFT ? 0 : shifted;

      /* Toogle capslock */
    if(id == CAPSLOCK)
      capsLock = !capsLock;
  }
  else
  {
    shifted = id == LSHIFT ? 1 : shifted;

    /* Set data available upon keydown */
    keyboard_data_available = 1;
    keyboard_data = id;
  } 
}

char get_ascii(char keycode)
{
  if(shifted || capsLock)
  {
    return table_uppercase[keycode];
  }
  else
  {
    return table_lowercase[keycode];
  }
}

void set_leds()
{
  do{
    irq_reset_master();
    outb(0xed, 0x60);
  }while(inb(0x64) != 0xFA);
  do{
    irq_reset_master();
    outb(0x03 , 0x60);
  }while(inb(0x64) != 0xFA);
}

void cut_leds()
{
  do{
    outb(0xed, 064);
  }while(inb(0x64) != 0xFA);
  outb(0x00, 0x60);
}

unsigned char is_printable(char keycode)
{
  return table_printable[keycode] == 'y' ? 1 : 0;
}

char wait_char()
{
  /* Blocking function allowing user to wait for user input char. Blocking. */
  
  /* Blocks until data available */
  while(!keyboard_data_available);

  /* set data consumed */
  keyboard_data_available = 0;

  return keyboard_data;  
}

int getline(char* buffer, int count)
{
  /* add any printable ascii char to the buffer and return on enter */
  char entry=0;
  int pos = 0;
  do{
    entry = wait_char();
    if(is_printable(entry) && pos < count-1)
    {
      buffer[pos] = get_ascii(entry);
      printc(buffer[pos]);
      pos++;
    }
    else if(entry == BACKSPACE && pos > 0)
    {
      buffer[pos] = '\0';
      pos--;
      backspace();
    }
  }while(entry != NWLINE);
  buffer[pos] = '\0';
  return pos;
}