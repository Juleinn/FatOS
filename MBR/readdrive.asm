; ---------------------------------------------------------
; file readdrive.asm                                      ;
; Anton Claes, 2017                                       ;
; contains routines to perform reads from floppy          ;
; ---------------------------------------------------------

; read drive parameters
; requires drive id in dl
readdrive_geometry:
  mov ah, 0x08
  int 0x13
  mov [max_cylinder_number], ch
  mov [max_sector_number], cl
  mov [max_head_number], dh
  ret

; function that performs the read of given sectors into memory
; and retries while unsuccessfull
; expects what int0x13 ah=0x02 expects as parameters
readdrive_loadkernel:
  pusha
  call readdrive_geometry

  mov ax, 0x00
  mov es, ax
  mov bx, 0x7e00      ; kernel will be loaded at 0x7e00
  ; read sectors one by one
  ; inefficient but nevermind for now
  mov ah, 0x02
  mov al, 0x01        ; read 1 sector
  mov ch, 0x00        ; cylinder
  mov cl, 0x0D        ; read from the 13th sector
  mov dh, 0x00        ; head 0
  mov dl, [drive_id]  ; the boot drive

  mov byte [left_to_read], KERNEL_SECTORS

readdrive_loadkernel_loop:
  call readdrive_read
readdrive_readed:
  sub byte [left_to_read], 1
  cmp byte [left_to_read], 0
  je readdrive_loadkernel_complete
  ; perform read of next sector
  add bx, 0x200                 ; next destination
  add cl, 1                     ; next sector
  cmp cl, [max_sector_number]   
  jle readdrive_loadkernel_loop 
  mov cl, 1                     ; if last sector passed, switch to next head
  add dh, 1 
  cmp dh, [max_head_number]     ; if last head passed, switch to next cylinder
  jng readdrive_loadkernel_loop
  mov dh, 0                     ; reset to head 0
  add ch, 1                     ; assume we cannot get passed last sector
  jmp readdrive_loadkernel_loop

readdrive_loadkernel_complete:
  popa
  ret


readdrive_read:
  pusha
  ; first try
  call readdrive_read_sectors ; attempt reading the sectors
sectors_attempted:
  jnc readdrive_read_success  ; successfull
  call readdrive_reset        ; reset
  ; second try
  call readdrive_read_sectors ; try again with reset drive
  jnc readdrive_read_success  ; successfull
  call readdrive_reset        ; ...
  ; 3rd try
  call readdrive_read_sectors
  jnc readdrive_read_success
  ; if here : failed after 3 times
  call readdrive_printerror
readdrive_read_success:
  popa
  ret

; routine that reads sectors into the appropriate memory location
; expects number of sectors to read in al
; expects cylinder number in ch
; expects sector number in cl
; dh = head number
; dl = drive_number
; es:bs = data buffer
readdrive_read_sectors:
  pusha
  mov ah, 0x02              ; read function
  int 0x13
  popa
  ret

; resets the drive in dl
readdrive_reset:
  pusha
  mov al, 0x00
  int 0x13
  popa

readdrive_printerror:
  mov ax, readdrive_error      ; load string address into si
  mov si, ax
  call print_string

  jmp $
  ret

max_sector_number:
  db 0
max_cylinder_number:
  db 0
max_head_number: 
  db 0
left_to_read:
  db 0

readdrive_error:
  db "Fatal error reading from drive", 0
