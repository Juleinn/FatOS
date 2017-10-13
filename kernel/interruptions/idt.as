/* File idt.as */

.global idt_load
.extern idt_descriptor


/* Load the idt */
idt_load:
	lidt idt_descriptor
	ret

/* remaps the IRQs to the entries 32-47 of the IDT 
 * By default IRQ 0-7 are mapped 8-15 and 8-15 0x70 ... 
 * Remapping procedure : send 0x11 to both PICs (master and slave) -> tells pics how to work (cascaded) 
 * send offset : MASTER-> offset 32 = 0x20 ; slave -> offset = 40 = 0x28
 * send which IRQ to use for communication 
 * send architecture type */

.global irq_remap

irq_remap:
	push %eax
	/* step 1 : cascaded work */
	mov $0x11, %al
	out %al, $0x20 
	mov $0x11, %al
	out %al, $0xA0
	/* step 2 : offsets */
	mov $0x20, %al 	// offset 32
	out %al, $0x21	// output at PIC+1
	mov $0x28, %al 	// offset 40
	out %al, $0xA1
	/* Step 3 : connection to slave */
	mov $0x04, %al
	out %al, $0x21
	mov $0x02, %al
	out %al, $0xA1
	/* step4 : arch */
	mov $0x01, %al
	out %al, $0x21
	mov $0x01, %al
	out %al, $0xA1
	/* step 5 : enable all */
	mov $0x00, %al
	out %al, $0x21
	mov $0x00, %al
	out %al, $0xA1

	pop %eax
	ret

/* Resets the PIC controlers by sending an End Of Interrupt EOI 
 * send an EOI=0x20 to the controlers at 0x20, 0xA0 */

.global irq_reset_master
 irq_reset_master:
 	push %eax
 	mov $0x20, %al
 	out %al, $0x20 	// master
 	pop %eax
 	ret

.global irq_reset_slave
irq_reset_slave:
 	push %eax
 	mov $0x20, %al
 	out %al, $0xA0 	// master
 	pop %eax
 	ret
