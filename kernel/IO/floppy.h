/* file floppy.h */

#ifndef FLOPPY_H_
#define FLOPPY_H_

#include "print.h"
#include "io.h"
#include "timer.h"
#include "keyboard.h"

void floppy_read();

void floppy_irq_handler();

void print_floppy_drives();

void floppy_send_byte(unsigned char byte);

char floppy_read_byte();

void floppy_init();

void floppy_reset();

void floppy_waitinterrupt();

void floppy_configure(unsigned char impseek_EN, unsigned char fifoDIS, unsigned char pollingDIS, unsigned char threshold);

void floppy_specify();

void floppy_motoron();

void floppy_recalibrate();

void floppy_seek(unsigned char drive, unsigned char head, unsigned char cylinder);

/* Reads a sector at a given position specified as linear 
 * returns a pointer to the read sector */
unsigned char* floppy_readsector(unsigned int lba, unsigned char drive_id);

/* Writes the given array to the specified sector (linear). Performs a copy because 
 * DMA cannot acces memory beyond 16k. Input array size must be inferior to sector size */
void floppy_writesector(unsigned char* data, unsigned int lba, unsigned char drive_id);

void lba_to_chs(unsigned int lba, unsigned short* cylinder, unsigned short* head, unsigned short* sector);

// DMA acces functions 
/* DMA ports - CODE COPY - */
#define DMA_ADDRL       0x004	/* port for low 16 bits of DMA address */
#define DMA_ADDRH       0x081	/* port for top 4 bits of 20-bit DMA addr */
#define DMA_COUNT      	0x005	/* port for DMA count (count =  bytes - 1) */
#define DMA_FLIPFLOP   	0x00C	/* DMA byte pointer flip-flop */
#define DMA_MODE       	0x00B	/* DMA mode port */
#define DMA_INIT       	0x00A	/* DMA init port */
#define DMA_RESET_VAL   0x06

#define DMA_MODE_READ 	0x46
#define DMA_MODE_WRITE 	0x4A

#define SECTOR_SIZE 512

/* opcode = read/write */
void floppy_DMAsetup(unsigned char mode, unsigned int address, unsigned int count);

#define STATUS_REGISTER_A                0x3F0 // read-only
#define STATUS_REGISTER_B                0x3F1 // read-only
#define DIGITAL_OUTPUT_REGISTER          0x3F2
#define TAPE_DRIVE_REGISTER              0x3F3
#define MAIN_STATUS_REGISTER             0x3F4 // read-only
#define DATARATE_SELECT_REGISTER         0x3F4 // write-only
#define DATA_FIFO                        0x3F5
#define DIGITAL_INPUT_REGISTER           0x3F7 // read-only
#define CONFIGURATION_CONTROL_REGISTER   0x3F7  // write-only

#endif