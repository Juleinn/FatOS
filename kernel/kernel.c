/* file kernel.c 
 * Anton Claes, 2017
 * This is the first file of my kernel */

// #include "IO/print.h"

/* Kernel's entry point */

#include "IO/print.h"
#include "memory.h"
#include "interruption.h"
#include "memorymanager.h"

int main(){
	clear_screen();	
	init_text();

	// set up clock speed
	timer_init();
	idt_setup();	

	clear_screen();

	/* test mem manager */
	memorymanager_init();
	memorymanager_print();
	char * somedata = (char*) sys_alloc(128);
	char * somedata2 = (char*) sys_alloc(128);
	char * somedata3 = (char*) sys_alloc(128);
	sys_free(somedata3);
	sys_alloc(120);
	memorymanager_print();

	console();

	return 0;
}