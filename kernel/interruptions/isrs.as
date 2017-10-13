/* file isrs.as 
   Contains interruptions service routines */

/* The 32 first interruption service routines */
.global isr000		// no err.code 
.global isr001		// no err. code 
.global isr002		// no err. code 
.global isr003		// no err. code 
.global isr004		// no err. code 
.global isr005		// no err. code 
.global isr006		// no err. code 
.global isr007		// no err. code 
.global isr008		// err. code 
.global isr009		// no err. code 
.global isr010		// err. code 
.global isr011		// err. code 
.global isr012		// err. code 
.global isr013		// err. code 
.global isr014		// err. code 
.global isr015		// no err. code 
.global isr016		// no err. code 
.global isr017		// no err. code 
.global isr018		// no err. code 
.global isr019		// no err. code 
.global isr020		// no err. code 
.global isr021		// no err. code 
.global isr022		// no err. code 
.global isr023		// no err. code 
.global isr024		// no err. code 
.global isr025		// no err. code 
.global isr026		// no err. code 
.global isr027		// no err. code 
.global isr028		// no err. code 
.global isr029		// no err. code 
.global isr030		// no err. code 
.global isr031		// no err. code 
.global isr032		// no err. code 
.global isrNULL		// the do-nothing interrupt handler
.global isr_handler


.global isr_dfexception

.extern dfault_exc_handler 

/* Handles double fault exceptions 
 * Pushes error code on stack -> ignore it for now */
isr_dfexception:
	cli
	/* Store all reg values on stack */
	/* general purpose regs */
	pusha
	push %ds
	push %es
	push %fs
	push %gs
	/* Beginning or safe zone*/
	call dfault_exc_handler
	/* End of safe zone */
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
	add $4, %eax	/* Delete error code */
	iret

/* Handles a breakpoint exception 
 * Does not push an error code on stack */
.global isr_bkpexception
.extern bkp_exc_handler
isr_bkpexception:
	cli
	/* Store all reg values on stack */
	/* general purpose regs */
	pusha
	push %ds
	push %es
	push %fs
	push %gs
	/* Beginning or safe zone*/
	call bkp_exc_handler
	/* End of safe zone */
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
	iret



/* Default IRQs handlers : do nothing */
.global isr_irqs_low
.global isr_irqs_high
/* Handles all irqs */
isr_irqs_low:
	cli
	push %eax
	// mov $'l', %al
	// mov %al, 0xb8000
	call irq_reset_master
	pop %eax
	iret

isr_irqs_high:
	cli
	push %eax
	// mov $'h', %al
	// mov %al, 0xb8000
	call irq_reset_slave
	call irq_reset_master
	pop %eax
	iret

/* Handles timer IRQ*/
.global irq_clock0
.extern clock0_irq_handler
irq_clock0:
	cli
	/* Store all reg values on stack */
	/* general purpose regs */
	pusha
	push %ds
	push %es
	push %fs
	push %gs
	/* Beginning or safe zone*/
	call clock0_irq_handler
	call irq_reset_master
	/* End of safe zone */
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
	iret


/* Handles keyboard IRQ */
.global irq_kbd
.extern kbd_irq_handler
irq_kbd:
	cli
	/* Store all reg values on stack */
	/* general purpose regs */
	pusha
	push %ds
	push %es
	push %fs
	push %gs
	/* Beginning or safe zone*/
	call kbd_irq_handler
	call irq_reset_master
	/* End of safe zone */
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
	iret

/* Handles floppy drive read return interruption */
.global irq_floppy
.extern floppy_irq_handler
irq_floppy:
	cli
	/* Store all reg values on stack */
	/* general purpose regs */
	pusha
	push %ds
	push %es
	push %fs
	push %gs
	/* Beginning or safe zone*/
	call floppy_irq_handler
	call irq_reset_master
	/* End of safe zone */
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
	iret
	iret



isr000:
	push %eax
	mov $'A', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr001:
	push %eax
	mov $'B', %al
	mov %al, 0xb8000
	pop %eax
	add $4, %esp
	iret


isr002:
	push %eax
	mov $'C', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr003:
	push %eax
	mov $'C', %al
	mov %al, 0xb8000
	pop %eax
	iret


isr004:
	push %eax
	mov $'D', %al
	mov %al, 0xb8000
	pop %eax
	iret


isr005:
	push %eax
	mov $'E', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr006:
	push %eax
	mov $'F', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr007:
	push %eax
	mov $'G', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr008:
	push %eax
	mov $'H', %al
	mov %al, 0xb8000
	pop %eax
	add $4, %ebp
	iret

isr009:
	push %eax
	mov $'I', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr010:
	push %eax
	mov $'J', %al
	mov %al, 0xb8000
	pop %eax
	add $4, %ebp
	iret

isr011:
	push %eax
	mov $'K', %al
	mov %al, 0xb8000
	pop %eax
	add $4, %ebp
	iret

isr012:
	push %eax
	mov $'L', %al
	mov %al, 0xb8000
	pop %eax
	add $4, %ebp
	iret

isr013:
	push %eax
	mov $'M', %al
	mov %al, 0xb8000
	pop %eax
	add $4, %ebp
	iret

isr014:
	push %eax
	mov $'N', %al
	mov %al, 0xb8000
	pop %eax
	add $4, %ebp
	iret

isr015:
	push %eax
	mov $'O', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr016:
	push %eax
	mov $'P', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr017:
	push %eax
	mov $'Q', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr018:
	push %eax
	mov $'R', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr019:
	push %eax
	mov $'S', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr020:
	push %eax
	mov $'T', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr021:
	push %eax
	mov $'U', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr022:
	push %eax
	mov $'V', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr023:
	push %eax
	mov $'W', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr024:
	push %eax
	mov $'X', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr025:
	push %eax
	mov $'Y', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr026:
	push %eax
	mov $'Z', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr027:
	push %eax
	mov $'0', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr028:
	push %eax
	mov $'1', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr029:
	push %eax
	mov $'2', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr030:
	push %eax
	mov $'3', %al
	mov %al, 0xb8000
	pop %eax
	iret

isr031:
	push %eax
	mov $'4', %al
	mov %al, 0xb8000
	pop %eax
	iret
