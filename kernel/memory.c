#include "memory.h"

void memcpy(char* source, char* dest, int count)
{
	int i;
	for(i=0; i<count; i++)
		dest[i] = source[i];
	
}

void memset(char value, char* dest, int count)
{
	int i;
	for(i=0; i<count; i++)
		dest[i] = value;
}