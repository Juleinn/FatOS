#include "memorymanager.h"
#include "memorymap.h"

/* First item in the memory table */
MemoryHeader * first_memory_header;
/* Free memory base address (leave margin for bootstraped OS) */
char * freememory_base;

void memorymanager_init()
{
	/* retrieve biggest usable memory from computed memory map */
	unsigned char* memorymap_entrycount = (unsigned char*) MEMORYMAP_ENTRYCOUNT;
	Memorymap_entry* mentry = (Memorymap_entry*) MEMORYMAP_BASEADDRESS;	// current entry 
	int i;
	int maxSize = 0, maxSizeId = 0;
	for(i=0; i<*memorymap_entrycount; i++)
	{
		if(mentry[i].length > maxSize && mentry[i].type == 1) // biggest usable entry so far
		{
			maxSize = mentry[i].	length;
			maxSizeId = i;
		}
	}
	prints("Biggest memory entry : ");
	printi(maxSizeId);
	prints("\n");

	/* Leave 128 * 512 (128 sectors) free after start of free memory for the bootstraped kernel 
	and align as a multiple of 4 (should alread be the case but make sure */
	freememory_base = (char*) mentry[maxSizeId].base_address + KERNEL_SPACE;
	freememory_base += mentry[maxSizeId].base_address % 4 != 0 ? 
							4 - (mentry[maxSizeId].base_address % 4) : 0;

	/* Initialize on purpose the first header to be an empty used data never to be modified */
	first_memory_header = (MemoryHeader*) freememory_base;
	// whole left memory size - header size
	first_memory_header->length = 0;
	// flags = 0x01 (used memory : flag used = true)
	first_memory_header->flags = 0x00000001;
	first_memory_header->next = first_memory_header + 1; // set it to the directly next header
	first_memory_header->previous = (void*) 0;

	/* Initialize first true  chunk */ 
	MemoryHeader * firstTrueHeader = ((MemoryHeader*) freememory_base) + 1;
	// whole left memory size - header size
	firstTrueHeader->length = mentry[maxSizeId].length 
					- ((int) freememory_base - (int) mentry[maxSizeId].base_address)
					- (int) 2 * sizeof(MemoryHeader);
	// flags = 0x00 (free memory : flag used = false;)
	firstTrueHeader->flags = 0x00000000;
	firstTrueHeader->next = (void*) 0;
	firstTrueHeader->previous = (void*) first_memory_header;
}

void * sys_alloc(int length)
{
	/* Allocate a block of memory of the reqested size using best fit algorithm 
	best fit = tiniest chunk of memory which size > requested size 
	need to add length of header in required space. Also align length on 4*/
	length += length % 4 != 0 ? 4-(length % 4) : 0;
	MemoryHeader * bestFit = find_bestfit(length);
	prints("Best fit : "); printi((int)bestFit);prints("\n");
	/* If space left for another fit later, let's do it */
	int leftSize = bestFit->length - length;
	if(leftSize> sizeof(MemoryHeader))
	{
		bestFit->length = length;
		bestFit->flags |= 0x01;


		// create a new header right after
		MemoryHeader *nextHeader = (MemoryHeader*) ((int)((char*)bestFit) + 
			(bestFit->length) + sizeof(MemoryHeader));

		nextHeader->previous = bestFit;
		nextHeader->next = bestFit->next;
		bestFit->next = nextHeader;
		nextHeader->length = leftSize - sizeof(MemoryHeader);
		if(nextHeader->next != (void*)0)
			nextHeader->next->previous = nextHeader;

	}
	else	// no space left
	{
		// just update length
		bestFit->length = length;
	}
	return (void*)((int)bestFit + sizeof(MemoryHeader));
}

MemoryHeader * find_bestfit(int length)
{
	/* Finds the best fit for the given length */
	MemoryHeader * currentHeader = first_memory_header;
	MemoryHeader * bestFit = (void*)0;

	while(currentHeader != (void*)0 ){
		if(!currentHeader->flags & 0x01 == 1 && 
			currentHeader->length >= length && 
			// no best fit or better fit
			(bestFit == (void*)0 || currentHeader->length < bestFit->length))
			bestFit = currentHeader;

		currentHeader = currentHeader->next;
	}
	return bestFit;
}

void sys_free(void* addr)
{
	// free memory : set the flag to 0 then reduce with neighbors if free too
	MemoryHeader * headerToFree = (MemoryHeader*) ((int) addr - sizeof(MemoryHeader));
	headerToFree->flags &= 0xFFFFFFFE;
	// reduce recursively on neighbors here
}

void memorymanager_print()
{
	prints("\nMemory chuncks : ");
	MemoryHeader * currentHeader = first_memory_header;
	while(currentHeader != (void*)0 ){
		prints("\nbse(hder):"); printi((int)currentHeader);
		prints(",lgth(B)=");	printi(currentHeader->length);
		prints(",usd = "); printi(currentHeader->flags & 0x01);
		prints(",nxt=");printi((int)currentHeader->next);
		prints(",prev=");printi((int)currentHeader->previous);
		currentHeader = currentHeader->next;

	}
	prints("\n");
}