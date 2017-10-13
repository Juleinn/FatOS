#include "io.h"

void outb(unsigned char b, short p)
{
	asm("mov %0, %%dx\n\t"
		"mov %1, %%al\n\t"
			"out %%al, %%dx"
			:: "rm" (p) , "rm" (b));
}

unsigned char inb(short port)
{
	unsigned char res=0;
	asm("mov %1, %%dx\n\t"
			"in %%dx, %%al\n\t"
			"mov %%al, %0"
			: "=rm" (res) : "rm" (port));
	return res;
}