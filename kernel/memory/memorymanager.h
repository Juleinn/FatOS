/*
 * file memorymanager.h
 * contains memory managment functions & structures
 */

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

/*
Memory managment works as a chained list of memory chuncks having a header :

+--------------------+------------------------+
|		header		 |		data			  |
+--------------------+------------------------+

with the following header :

128			96		64				32			0
+-----------+-------+---------------+-----------+
|	length	| flags	| previousAddr	| nextAddr	|
+-----------+-------+---------------+-----------+

flags is 4 bytes (to keep 4 bytes aligned)
only flag used so far = bit0 = used. 0 = free, 1 = used

flags :
32	...	16	...	1 0
				u
				s
				e
				d

*/

struct MemoryHeader
{
	int length;						// length in bytes
	unsigned int flags;				// memory space flags
	struct MemoryHeader* previous;	// address of beginning of previous header
	struct MemoryHeader* next;		// address of beginning of next heade
}__attribute__((packed));
typedef struct MemoryHeader MemoryHeader;

/* Initialize memory manager */
void memorymanager_init();

/* Allocate some memory space of required size in bytes.
Returns a pointer to the base address of the data part of the memory (not the header) */
void * sys_alloc(int length);

/* Inverse of the above : free memory which data (not header) starts at given address */
void sys_free(void* addr);

/* Concats several contiguous free chuncks of memory into a single one, given a base one. */
void memorymanager_concat(MemoryHeader* base);

/* Finds the next non empty Memoryheader */
MemoryHeader* memorymanager_findNextNonEmpty(MemoryHeader* base);

/* Finds the next non empty Memoryheader */
MemoryHeader* memorymanager_findNonEmptyBefore(MemoryHeader* base);

/* Finds the best fit for the given size (returns pointer to the corresponding header */
MemoryHeader * find_bestfit(int length);

/* Gets the free length above the given header */
int memorymanager_getFreeLengthAbove(MemoryHeader * base);

/* ---  Debugging functions : --- */
/* Prints all current memory chuncks */
void memorymanager_print();

#define KERNEL_SPACE 128*512

#endif
