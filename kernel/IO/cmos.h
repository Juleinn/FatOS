/* File cmos.h 
 * Anton CLAES
 * Provides functions for CMOS acces 
 */

#ifndef CMOS_H_
#define CMOS_H_

#include "print.h"

/* Floppy drive detection */
char read_floppies();

#define BOOT_DRIVE_ID 0x7dfd

/* Returs the boot drive id */
unsigned char get_boot_drive_id();

#endif