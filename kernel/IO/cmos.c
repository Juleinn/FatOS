#include "cmos.h"

char read_floppies()
{
	outb(0x10, 0x70);
	return inb(0x71);
}

unsigned char get_boot_drive_id()
{
	unsigned char * boot_drive_id = (unsigned char*) BOOT_DRIVE_ID;
	return *boot_drive_id;
}