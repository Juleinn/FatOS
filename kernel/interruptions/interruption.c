#include "interruption.h"

/* the actuel interrupt descriptor table. 256 interrupts, 32 for system */
struct IDT_Entry idt_entries[256];
struct IDT_Descriptor idt_descriptor;

extern void idt_load();
extern void isr000();
extern void isr001();
extern void isr002();
extern void isr003();
extern void isr004();
extern void isr005();
extern void isr006();
extern void isr007();
extern void isr008();
extern void isr009();
extern void isr010();
extern void isr011();
extern void isr012();
extern void isr013();
extern void isr014();
extern void isr015();
extern void isr016();
extern void isr017();
extern void isr018();
extern void isr019();
extern void isr020();
extern void isr021();
extern void isr022();
extern void isr023();
extern void isr024();
extern void isr025();
extern void isr026();
extern void isr027();
extern void isr028();
extern void isr029();
extern void isr030();
extern void isr031();

extern void isr_dfexception();

extern void isr_bkpexception();

extern void irq_remap();

extern void isr_irqs_low();
extern void isr_irqs_high();

extern void irq_kbd();

extern void irq_clock0();

extern void irq_reset_master();
extern void irq_reset_slave();

extern void irq_floppy();


void idt_setup()
{
	/* Set up size of IDT*/
	idt_descriptor.limit = (sizeof(struct IDT_Entry) * 256) - 1;
	idt_descriptor.base = (unsigned int) idt_entries;
	print_string("base", 0x0f, 0, 10);
	print_int(idt_descriptor.base, 0x0f, 5, 10);

	print_string("isr000", 0x0f, 0, 11);
	print_int((unsigned)isr000, 0x0f, 5, 11);

	/* clear the hole IDT */
	memset(0, idt_entries, (sizeof(struct IDT_Entry) * 256));

	/* insertion of the gates */
	idt_set_entry(0, (unsigned) isr000, 0x8e);
	idt_set_entry(1, (unsigned) isr001, 0x8e);
	idt_set_entry(2, (unsigned) isr002, 0x8e);
	idt_set_entry(3, (unsigned) isr_bkpexception, 0x8e);
	idt_set_entry(4, (unsigned) isr004, 0x8e);
	idt_set_entry(5, (unsigned) isr005, 0x8e);
	idt_set_entry(6, (unsigned) isr006, 0x8e);
	idt_set_entry(7, (unsigned) isr007, 0x8e);
	idt_set_entry(8, (unsigned) isr_dfexception, 0x8e);
	idt_set_entry(9, (unsigned) isr009, 0x8e);
	idt_set_entry(10, (unsigned) isr010, 0x8e);
	idt_set_entry(11, (unsigned) isr011, 0x8e);
	idt_set_entry(12, (unsigned) isr012, 0x8e);
	idt_set_entry(13, (unsigned) isr013, 0x8e);
	idt_set_entry(14, (unsigned) isr014, 0x8e);
	idt_set_entry(15, (unsigned) isr015, 0x8e);
	idt_set_entry(16, (unsigned) isr016, 0x8e);
	idt_set_entry(17, (unsigned) isr017, 0x8e);
	idt_set_entry(18, (unsigned) isr018, 0x8e);
	idt_set_entry(19, (unsigned) isr019, 0x8e);
	idt_set_entry(20, (unsigned) isr020, 0x8e);
	idt_set_entry(21, (unsigned) isr021, 0x8e);
	idt_set_entry(22, (unsigned) isr022, 0x8e);
	idt_set_entry(23, (unsigned) isr023, 0x8e);
	idt_set_entry(24, (unsigned) isr024, 0x8e);
	idt_set_entry(25, (unsigned) isr025, 0x8e);
	idt_set_entry(26, (unsigned) isr026, 0x8e);
	idt_set_entry(27, (unsigned) isr027, 0x8e);
	idt_set_entry(28, (unsigned) isr028, 0x8e);
	idt_set_entry(29, (unsigned) isr029, 0x8e);
	idt_set_entry(30, (unsigned) isr030, 0x8e);	
	idt_set_entry(31, (unsigned) isr031, 0x8e);

	irq_remap();

	int i;
	for(i=32;i<40;i++)
	{
		idt_set_entry(i, (unsigned)isr_irqs_low, 0x8e);
		idt_set_entry(i+8, (unsigned)isr_irqs_high, 0x8e);
	}
	// timeout fired IRQ0
	idt_set_entry(32, (unsigned)irq_clock0, 0x8e);

	// keyboard irq1 is fired = 33
	idt_set_entry(33, (unsigned) irq_kbd, 0x8e);

	// floppy controler interruption
	idt_set_entry(38, (unsigned) irq_floppy, 0x8e);

	idt_load();



	asm("sti");
	
	irq_reset_slave();
	irq_reset_master();
	
}

void idt_set_entry(int p, unsigned long subroutine, char flags)
{
	idt_entries[p].base_lower = (short) ((int) subroutine & 0x0000ffff); 	/* lower part of address of subroutine */
	idt_entries[p].base_higher = (short) (((int) subroutine >> 16) & 0xffff );
	idt_entries[p].always_0 = 0;
	idt_entries[p].flags = flags;
	// get back the segment selector 
	unsigned char* seg_selector = (unsigned char*) CODE_SEGMENT;
	idt_entries[p].segment_selector = (unsigned short) 8;
}

void default_it_hdlr()
{
	print_string("Warning : Unhandled interruption. Locking", 0x0F, 0, 0);
}

void dfault_exc_handler()
{
	print_string("Double fault exception occured", 0x0F, 0, 0);
}

void bkp_exc_handler()
{
	print_string("Breakpoint fault exception occured", 0x0F, 0, 0);
}