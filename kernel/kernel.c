/** @file kernel.c
 *  @author Anton Claes
 	@date 2017
 *  @brief This contains the kernel's entry point*/

// #include "IO/print.h"

/* Kernel's entry point */

#include "IO/print.h"
#include "memory.h"
#include "interruption.h"
#include "memorymanager.h"
#include "fs.h"

/**
@brief OS kernel entry point
@return 0 after console exited
**/
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


	filesystem_appendBytes(fs0, fs0->root,
    "/subdir/subfile1.bin", 100);

	console();

	return 0;
}
