[org 0x7c00]
[bits 16]

main:
jmp start
; ---------------------------------------------------------------------------------
; some additionnal files will be placed here providing some basic I/O functions   ;
; ---------------------------------------------------------------------------------

%include "printstring.asm" ; string displaying on screen
%include "readdrive.asm"  ; read floppy sectors into memory

KERNEL_SECTORS EQU 60       ; 10 sectors holding the kernel

start:

;set up stack
mov bp, 0x7bfc          ; set it up growing downwards just before the mbr load point (0x7c00)
mov sp, bp              ; empty stack

mov ax, 0x00            ; general registers all to 0
mov ds, ax
mov ss, ax
mov es, ax

; save drive id
mov [drive_id], dl

mov ax, helloworld      ; load string address into si
mov si, ax
call print_string       ; print string on screen

; load kernel at 0x7e00
call readdrive_loadkernel

jump_stage2:
	jmp stage2

jmp $                   ; endless loop here

; some data bits
helloworld:
  db "Hello world", 0

drive_id:
  db 0

; pad with zeros until magic number
times 510 - ($ - $$) db 0

magic_number:
  dw 0xaa55

 stage2:
