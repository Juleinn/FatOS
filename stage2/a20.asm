; file a20.asm
; Anton CLAES
; Contains routines for BIOS-handled A20 line enabling
; who the hell is responsible for this kind designs

; extended bios instruction set
; int 0x15 : 
; ax = 0x2401 -> enable a20 gate
; ax = 0x2402 -> get a20 gate status 
; ax = 0x2403 -> get a20 bios enabling support

a20_enable:

a20_support:
	mov ax, 0x2403	; query support
	int 0x15

	jc a20_failure	; error querying gate support
	; bx = gate support : bit 0 = supported on keyboard controler, bit 1 = supported with port 92
	; ah contains gate support status. ah = 0x01 if kbd controler in secure mode, 0x86 if not supported
	cmp ah, 0x00
	jne a20_failure

; checks if a20 has not already been activated by something else
a20_enabled:
	mov ax, 0x2402
	int 0x15

	; carry = failure getting the state
	jc a20_failure
	cmp al, 0x00	; al=0 means disabled, enable it
	je a20_doenable
	;otherwise, already enabled
	jmp a20_succes


a20_doenable:
	mov ax, 0x2401
	int 0x15
	jc a20_failure	; carry flag on error
	jmp a20_succes


a20_failure:
	; prompt a20 and loop
	mov ah, 0x0e
	mov al, 'A'
	int 0x10
	mov al, '2'
	int 0x10
	mov al, '0'
	int 0x10
	jmp $

a20_succes:
	ret