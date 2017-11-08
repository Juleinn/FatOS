; Stage 2 bootloader, performs gdt loading and 32 bits
; switching aswell as interrupt setup
[org 0x7e00]
[bits 16]

stage2:

	call memorymap_map

	jmp enter_protected

%include "gdt.asm"			; global descriptor table
%include "a20.asm"			; memoryline enabling
%include "memorymap.asm"	; memory detection

enter_protected:

	; switch to 32bits protected mode
	cli 	; clear interrupts until IVT is set ( or random results )
	; set up gdt
	lgdt [gdt_descriptor]

	; set the cr0 bit -> switching
	mov eax, cr0
	or eax, 0x01	; set the 1 bit
	mov cr0, eax

	; Important : clear the pipeline with a far jump
	jmp CODE_SEG:protected_mode_start

	[bits 32]
	protected_mode_start:

	; We are now in protected mode
	; need to set registers
	; all data registers (ds, ss, es, fs, gs) set to DATA_SEG
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; set the stack at top of free space ( grwing downwards )
	mov ebp, 0x7a00
	mov esp, ebp

	; we have kernel loaded at 0x7e00 but we want it loaded in high memory
	; move it by copying 512*64 bytes (=64 sectors) to high mem
	; use rep :
	; cx = count = 512*64 = 32768 bytes
	; esi = source = 0x7e00
	; edi = dest = 0x100000
	mov eax, 0x8000
	mov esi, eax
	mov eax, 0x100000
	mov edi, eax

	mov ecx, 32768
	rep movsb

	; set up stack into higher memory 
	; instead of where the kernel was loaded
	mov ebp, 0x9000
	mov esp, ebp

	; now kernel is loaded
	; call the main function
	call 0x100000

	jmp $

stage2_hello:
	db "Hello from stage 2", 0