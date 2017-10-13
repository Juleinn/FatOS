; file memorymap.asm
; gets the memory map in real mode 

; memorymap will be created 1 sector before the bootloader @(7c00 - 512) bytes

; address of the number of entries in the memory map
MEMORYMAP_ENTRYCOUNT EQU 0x7c00

; define some handy constants
MEMORYMAP_START EQU 0x7c01				; overwrite MBR

; processes the memorymap detection
memorymap_map:

; --- init ----
push di

; init entry count
mov ax, MEMORYMAP_ENTRYCOUNT
mov di, ax
mov [di], byte 0x00

; es:di = address of buffer
mov ax, 0x0000
mov es, ax
mov ax, MEMORYMAP_START
mov di, ax

mov ebx, 0x0000



memorymap_readentry:
; set params

mov eax, 0x0000e820		; bios memorymap detection
mov edx, 0x534D4150		; code for 'SMAP'
mov ecx, 20				; size of the buffer entry, always 20 bytes 

int 0x15
; entry read 

; if CF set or EBX = 0 => end of memory mapping
jc memorymap_end
cmp ebx, 0
je memorymap_end

; otherwise add 20 to si and jump to the next entry reading
add di, 20
;increase memorymap count without modifying ax nor di
push di
push ax
mov ax, MEMORYMAP_ENTRYCOUNT
mov di, ax
inc byte [di]
pop ax
pop di

jmp memorymap_readentry

memorymap_end:
pop di
ret

; number of entries in the memorymap
memorymap_entries_count:
	db 0x00