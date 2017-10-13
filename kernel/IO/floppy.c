#include "floppy.h"

static char floppy_interrupt = 0;

static unsigned char floppy_tmpdata[SECTOR_SIZE];

void floppy_read()
{
	floppy_init();

	floppy_readsector(1, 0);

	prints("Sector 1\n");
	int i;
	for(i=0; i<100; i++)
		printc(floppy_tmpdata[i]);
	

	char testdata[50] = "cessoinssontsanssucces";
	memcpy(testdata, floppy_tmpdata, 20);

	floppy_writesector(floppy_tmpdata, 1, 0);

}

/* Reads a sector at a given position specified as linear 
 * returns a pointer to the read sector */
unsigned char* floppy_readsector(unsigned int lba, unsigned char drive_id)
{
	// get the chs position of the sector on disk
	unsigned short cyl, head, sector;
	lba_to_chs(lba, &cyl, &head, &sector);

	// seek the appropriate position
	floppy_seek(drive_id, head, cyl);

	// set up the DMA buffer for the transfer to the static buffer.
	// set only transfer of 1 buffer 
	floppy_DMAsetup(DMA_MODE_READ, floppy_tmpdata, SECTOR_SIZE);

	// issue read
	floppy_interrupt = 0;
	// lets read head 0, cyl 0 sec 1
	floppy_send_byte(0xc6);					/* read command, MultiTrack set, MFM set */
	floppy_send_byte((((unsigned char) head) << 2) | drive_id); // head << 2 | drive
	floppy_send_byte(cyl);					// cyl
	floppy_send_byte(head); 				// head again
	floppy_send_byte(sector);				// sector
	floppy_send_byte(0x02);					// 2
	floppy_send_byte(0x01);					// sector count. Always 1 here, limited by buffer size
	floppy_send_byte(0x1b);					// gap size
	floppy_send_byte(0xff);					// 0xff

	floppy_waitinterrupt();					// wait here for end of reading

	#ifdef DEBUG
		prints("Read command completed\n");
	#endif

	// seven result bytes 
	char st0 = floppy_read_byte();
	char st1 = floppy_read_byte();
	char st2 = floppy_read_byte();
	cyl = (unsigned short) floppy_read_byte();
	head = (unsigned short) floppy_read_byte();
	sector = (unsigned short) floppy_read_byte();
	char byte2 = floppy_read_byte();

	#ifdef DEBUG
		prints("st0="); printh(st0);
		prints("\nst1="); printh(st1);
		prints("\nst2="); printh(st2);
		prints("\ncylinder="); printh(cyl);
		prints("\nend head="); printh(head);
		prints("\nend sector="); printh(sector);
		prints("\nbyte2="); printh(byte2);
		prints("\nRead bytes = ");
	#endif

	return floppy_tmpdata;
}

void floppy_writesector(unsigned char* data, unsigned int lba, unsigned char drive_id)
{
	// copy data to a low part of memory
	memcpy(data, floppy_tmpdata, SECTOR_SIZE);

	// get the chs address
	unsigned short cyl, head, sector;
	lba_to_chs(lba, &cyl, &head, &sector);

	// seek the apropriate position for the writing
	floppy_seek(drive_id, head, cyl);

	// setup DMA
	floppy_DMAsetup(DMA_MODE_WRITE, floppy_tmpdata, SECTOR_SIZE);

	// initiate writing to disk
	// issue read
	floppy_interrupt = 0;
	// lets read head 0, cyl 0 sec 1
	floppy_send_byte(0xc5);					/* read command, MultiTrack set, MFM set */
	floppy_send_byte((((unsigned char) head) << 2) | drive_id); // head << 2 | drive
	floppy_send_byte(cyl);					// cyl
	floppy_send_byte(head); 				// head again
	floppy_send_byte(sector);				// sector
	floppy_send_byte(0x02);					// 2
	floppy_send_byte(0x01);					// sector count. Always 1 here, limited by buffer size
	floppy_send_byte(0x1b);					// gap size
	floppy_send_byte(0xff);					// 0xff

	floppy_waitinterrupt();					// wait here for end of reading

	#ifdef DEBUG
		prints("Read command completed\n");
	#endif

	// seven result bytes 
	char st0 = floppy_read_byte();
	char st1 = floppy_read_byte();
	char st2 = floppy_read_byte();
	cyl = (unsigned short) floppy_read_byte();
	head = (unsigned short) floppy_read_byte();
	sector = (unsigned short) floppy_read_byte();
	char byte2 = floppy_read_byte();

	#ifdef DEBUG
		prints("st0="); printh(st0);
		prints("\nst1="); printh(st1);
		prints("\nst2="); printh(st2);
		prints("\ncylinder="); printh(cyl);
		prints("\nend head="); printh(head);
		prints("\nend sector="); printh(sector);
		prints("\nbyte2="); printh(byte2);
		prints("\nRead bytes = ");
	#endif

}

void floppy_irq_handler()
{
	#ifdef DEBUG
		prints("Floppy interrupt occured (IRQ6)\n");
	#endif
	floppy_interrupt = 1;
}

#define SEND_BYTE_TRIES 2000

void floppy_send_byte(unsigned char byte)
{
	int i;
	for(i=0; i<SEND_BYTE_TRIES; i++)
	{
		// read main status reg
		unsigned char msr = inb(MAIN_STATUS_REGISTER);
		if((msr & 0xc0) == 0x80)	// drive ready
		{
			outb(byte, DATA_FIFO);
			return;
		}
	}
	/* Print failure message */
	prints("Unable to send byte to floppy controler : drive not ready\n");
	return;
}

char floppy_read_byte()
{
	int i;
	for(i=0; i<SEND_BYTE_TRIES; i++)
	{
		// read main status reg
		unsigned char msr = inb(MAIN_STATUS_REGISTER);
		if((msr & 0xd0) == 0xd0)	// drive ready
		{
			return inb(DATA_FIFO);
		}
	}
	/* Print failure message */
	prints("Unable to read byte from floppy controler : drive not ready\n");
	return 0x00;
}

void floppy_init()
{
	// init sequence : reset - prog. datarate - wait interrupt - send 4 sense interrupts. - configure - specify - ready
	floppy_interrupt = 0;
	floppy_reset();	

	// ack with an interrupt 
	floppy_waitinterrupt();

	#ifdef DEBUG
		prints("Sending sense interrupts\n");
	#endif

	// send 4 sense interrupts and read result ()
	// send sense interrupts only if polling is on, which is default after reset
	int i;
	for(i=0; i<4; i++)
	{
		floppy_send_byte(0x08);
		char st0 = floppy_read_byte();
		char pcn = floppy_read_byte();
	}


	#ifdef DEBUG
		prints("programming datarate\n");
	#endif
	// program datarate : CCR to 0
	outb(0x00, CONFIGURATION_CONTROL_REGISTER);

	#ifdef DEBUG
		prints("Configuring floppy drive\n");
	#endif
	// configure 
	floppy_configure(1, 1, 1, 8);

	#ifdef DEBUG
		prints("specify\n");
	#endif
	// specify 
	floppy_specify();

	#ifdef DEBUG
		prints("Motor on\n");
	#endif
	// turn motor on for recalibrate 
	floppy_motoron();

	#ifdef DEBUG
		prints("Recal\n");
	#endif
	// recalibrate 
	floppy_recalibrate();

	#ifdef DEBUG
		prints("Floppy controler reset\n");
	#endif
}

void floppy_reset()
{
	floppy_interrupt = 0;
	// set DOR to 0x00 (enters reset state )
	outb(0x00, DIGITAL_OUTPUT_REGISTER);
	// wait some time for it to reset (not really needed )
	timer_wait(500);
	// put it back in its normal state with IRQ and DMA active
	outb(0x0C, DIGITAL_OUTPUT_REGISTER);
}

void floppy_configure(unsigned char impseek_EN, unsigned char fifoDIS, unsigned char pollingDIS, unsigned char threshold)
{
	floppy_send_byte(0x13);		// configure 
	floppy_send_byte(0x00);
	floppy_send_byte((impseek_EN << 6) | (fifoDIS << 5) | (pollingDIS << 4) | (threshold - 1));
	floppy_send_byte(0x00);
}

void floppy_specify()
{
	floppy_send_byte(0x03);
	floppy_send_byte(0xdf);	
	floppy_send_byte(0x02);
}

void floppy_motoron()
{
	// turn motor 0 on
	outb(0x1c, DIGITAL_OUTPUT_REGISTER);

	// wait some time for it to spin up
	timer_wait(500); // 1s is too much, may be changed later
}

void floppy_recalibrate()
{
	floppy_interrupt = 0;
	floppy_send_byte(0x07);
	floppy_send_byte(0x00);

	floppy_waitinterrupt();

	// check the recal status with a sense interrupt 
	floppy_send_byte(0x08);

	char st0 = floppy_read_byte();
	char cyl = floppy_read_byte();
	#ifdef DEBUG
		if(st0 & 0x20)
			prints("Recalibrate successfull\n");
	#endif
}

void floppy_seek(unsigned char drive, unsigned char head, unsigned char cylinder)
{
	floppy_interrupt = 0;
	floppy_send_byte(0x0F);// seek
	floppy_send_byte((head << 2) | (drive));
	floppy_send_byte(cylinder);

	floppy_waitinterrupt();

	floppy_send_byte(0x08);

	char st0 = floppy_read_byte();
	char cyl = floppy_read_byte();

	#ifdef DEBUG
		if(cyl == cylinder)
			prints("Seek successfull\n");
	#endif
}

void floppy_DMAsetup(unsigned char mode, unsigned int address, unsigned int count)
{
	// reset dma controler
	outb(DMA_RESET_VAL, DMA_INIT);
	// reset flipflop 
	outb(0x00, DMA_FLIPFLOP);
	// set mode 
	outb(mode, DMA_MODE);
	// set low 16  bytes of address
	outb(address & 0xFF, DMA_ADDRL);
	outb((address >> 8 ) & 0xFF, DMA_ADDRL);
	// set high 4 bytes of address
	outb((address >> 16) & 0xFF, DMA_ADDRH);
	// set size to read
	outb(count & 0xFF, DMA_COUNT);
	outb((count >> 8) & 0xFF, DMA_COUNT);
	// do reset
	outb(2, DMA_INIT);// "magic" value 
}

#define SECTORS_PER_TRACK 18

void lba_to_chs(unsigned int lba, unsigned short* cylinder, unsigned short* head, unsigned short* sector)
{
	*cylinder = (unsigned short) (lba) / (2 * SECTORS_PER_TRACK);
	*head = (unsigned short) ((lba % (2 * SECTORS_PER_TRACK)) / SECTORS_PER_TRACK);
	*sector = (unsigned short) (((lba % (2 * SECTORS_PER_TRACK)) % SECTORS_PER_TRACK) + 1);	
}

void floppy_waitinterrupt()
{
	while(!floppy_interrupt);
	return;
}

static const char *floppy_drive_types[] = 
{
	"no drive", "360KB 5.25\"", "1.2MB 5.25\"", "720KB 3.5\"", "1.44MB 3.5\"", "2.88MB 3.5\"" 
};

void print_floppy_drives()
{
	char flp0 = read_floppies();
	char flp1 = flp0 & 0x0F;
	flp0 = (flp0 & 0xF0) >> 4;
	prints("floppy 0 : ");
	prints(floppy_drive_types[flp0]);
	prints("\nfloppy 1 : ");
	prints(floppy_drive_types[flp1]);
	printc('\n');

	// read MAIN_STATUS_REGISTER
	char msr = inb(MAIN_STATUS_REGISTER);
	if(msr & 0x80)
		prints("Ok for exchange on FIFO port\n");
	else
		prints("Not OK for exchange on FIFO port\n");

	if(msr & 0x40)
		prints("Floppy expects IN FIFO opcode\n");
	else
		prints("Floppy doesn't expects IN FIFO opcode\n");

	if(msr & 0x10)
		prints("Drive busy\n");
	else
		prints("Drive idle\n");

	if(msr & 0x08)
		prints("Drive 3 seeking\n");
	if(msr & 0x04)
		prints("Drive 2 seeking\n");
	if(msr & 0x02)
		prints("Drive 1 seeking\n");
	if(msr & 0x01)
		prints("Drive 0 seeking\n");
}