#include "memorymap.h"

void print_memorymap(char color)
{
	unsigned char* memorymap_entrycount = (unsigned char*) MEMORYMAP_ENTRYCOUNT;
	Memorymap_entry* mentry = (Memorymap_entry*) MEMORYMAP_BASEADDRESS;

	int i;
	print_string("Memory mapping. Entries :", color, 0, 0);
	print_int((int) *memorymap_entrycount, color, 26, 0);

	for(i=0; i<*memorymap_entrycount; i++)
	{
		print_int(i, color, 0, i+1);
		
		print_string(" | base : ", color, 2, i+1);
		print_int((int) mentry->base_address, color,  12, i+1);

		print_string(" | length : ", color, 24, i+1);
		print_int((int) mentry->length, color,  36, i+1);

		print_string(" | end : ", color, 46, i+1);
		print_int((int) (mentry->length + mentry->base_address - 1), color,  53, i+1);

		print_string(" | type : ", color, 68, i+1);
		print_int((int) mentry->type, color,  79, i+1);	

		mentry++;	
	}
	print_string("Total RAM : ", 0x0F, 0, 8);
	print_int(memorymap_get_RAM_size(), 0x0F, 20, 8);

	set_cursor_pos(0, *memorymap_entrycount + 4); 
	return;
}

int memorymap_get_entrycount()
{
	unsigned char* memorymap_entrycount = (unsigned char*) MEMORYMAP_ENTRYCOUNT;
	return (int) *memorymap_entrycount;
}

Memorymap_entry memorymap_get_entry(int i)
{
	/* Make a copy of it*/
	Memorymap_entry result;
	Memorymap_entry *source = (Memorymap_entry*) MEMORYMAP_BASEADDRESS;
	source += i;
	result.base_address = source->base_address;
	result.length = source->length;
	result.type = source->type;
	return result;
}

int memorymap_get_RAM_size()
{
	int sum = 0;
	int count = memorymap_get_entrycount();
	Memorymap_entry *source = (Memorymap_entry*) MEMORYMAP_BASEADDRESS;
	int i;
	for(i=0; i<count; i++)
	{
		sum += source->length;
		source++;
	}		
	return sum;
}
