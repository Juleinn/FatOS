/* file interruption.h 
   Anton CLAES
   Contains interruption handling functions & structures 
*/

#ifndef INTERRUPTION_H_
#define INTERRUPTION_H_

/* Interruption descritor table entry */
struct IDT_Entry {
	unsigned short base_lower;			/* Lowest bytes of the offset to the interruption routine */
	unsigned short segment_selector;	/* Segment selector of the segment where the interruption routine is */
	unsigned char always_0;				/* fiels always set to 0 */
	unsigned char flags;				/* 01110 - 2 bits DPL = protection level = who can raise the interruption, 
											0 highest - p = segment present*/
	unsigned short base_higher;			/* Highest bytes of the offset of the interruption routine */
}__attribute__((packed));

/* Interruption descriptor table descriptor 
 tells cpu where the IDT_Entries are */
struct IDT_Descriptor{
	unsigned short limit;				/* Size of the IDT */
	unsigned int base;					/* Address of the IDT */
	
}__attribute__((packed));

/* Links the newly created IDT */
void idt_setup();

/* Adds an entry to the IDT at given pos*/
void idt_set_entry(int p, unsigned long subroutine, char flags);

void default_it_hdlr();

#define CODE_SEGMENT 0x7dfb
#define DATA_SEGMENT 0x7dfc

void dfault_exc_handler();
void bkp_exc_handler();

#endif