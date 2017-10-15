/* file kernel.c
 * Anton Claes, 2017
 * This is the first file of my kernel */

// #include "IO/print.h"

/* Kernel's entry point */

#include "IO/print.h"
#include "memory.h"
#include "interruption.h"
#include "memorymanager.h"
#include "fs.h"

int main(){
	clear_screen();
	init_text();

	// set up clock speed
	timer_init();
	idt_setup();

	clear_screen();

	/* test mem manager */
	memorymanager_init();
	filesystems_init();
	FileSystem * fs0 = getFirstFileSystem();
	// list root entry of fs0
	// filesystem_list(fs0->fileList, fs0->fileListSize);
	console();

	return 0;
}
