/* file memorymap.h 
   Anton CLAES
   Contains structures and functions for memorymapping aquirement 

   memory mapping is at 0x7a00 ( se memorymap.asm ) 
   first byte = memory mapping entries count 
   then all entries, 20 bytes long
   byte 0-7 : base address
   byte 8-15 : length of area 
   byte 16-20 : type : 0x01 = available, >1 = unavailable ( see table for details )

   */

#ifndef MEMORYMAP_H_
#define MEMORYMAP_H_

#include "print.h"

struct Memorymap_entry{
	unsigned long long base_address;	/* Base address of the described area */
	unsigned long long length;		/* length in byte of the area */
	unsigned int type;			/* Type; 1 = free */
}__attribute__((packed));

typedef struct Memorymap_entry Memorymap_entry;

#define MEMORYMAP_ENTRYCOUNT 0x7c00  /* Adress of the memorymap entrycount in memory */
#define MEMORYMAP_BASEADDRESS 0x7c01 /* Address of the first memorymap entry */

/* Displays an overview of the memorymap on the screen at pos 0,0 */
void print_memorymap(char color);

/* Returns the number of entries in the memory map */
int memorymap_get_entrycount();

/* Returns the memory buffer structure at index i in the memory map 
! Copy ! (readonly)  */
Memorymap_entry memorymap_get_entry(int i);

/* Returns the total amount of ARM given by bios */
int memorymap_get_RAM_size();

#endif