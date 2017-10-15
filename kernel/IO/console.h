/* file console.h
 * Anton CLAES
 * Handles a basic prompt for system output and basic data input
 * Launches a set of basic programs such as memorymaping, ... */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "string.h"
#include "keyboard.h"
#include "print.h"
#include "memorymap.h"
#include "cmos.h"
#include "floppy.h"
#include "fs.h"
#include "memorymanager.h"

/* Hard coded basic commands */
static const char *command_list[] = {"exit", "memmap", "cls", "help", "console", "floppyinfo", "flpread", "fs", "meminfo"};
#define CONSOLE_COMMAND_COUNT 9

/* Main console function. Exits on "exit" user entry */
void console();

/* returns the command id corresponding to the text (uses command_list)
 * returns -1 upon failure */
int console_command_id(char* cmd);

/* Launches command with specified ID */
void console_launch(int id);

/* lists the available commands */
void help();

#endif
