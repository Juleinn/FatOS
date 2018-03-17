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
#include "sysfile.h"

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

	sysfile_init();

	int file = sys_fopen("0/FAT.bin", SYSFD_MODE_READ);
	sysfd_printfdlist();

	char buffer[512] = "0/";
	
	sys_fread(file, buffer, 0, 512);
	prints(buffer);

	console();

	return 0;
}
