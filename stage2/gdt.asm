; ----------------------------------------------------------------- ;
; file 		gdt.asm													;
; author 	Anton CLAES												;
; date 		22/01/2017												;
; global descritptor table for switching to 32 bits protected mode 	;
; ----------------------------------------------------------------- ;

; ***************************** ;
;   global descriptor table     ;
;   gdt type : flat				;
; 	2 segs of 4gb : code and 	;
;	data						;
; 	-> no protection			;
;   -> no paging				;
; ******************************;
gdt_start:		; start of gdt

gdt_null:		; null descriptor
	dd 0x0 		; all flags and fields to 0
	dd 0x0

gdt_code:		
; code descriptor
; base 0x00
; limit 0xffffff ( max ) 
; flags part 1 : present = 1  - privilege = 00 - descriptor type = 1 - code = 1 - conforming = 0 - readable = 1 - accessed = 0
; => 10011010
; flags part 2 : granularity = 1 - 32 bits default = 1 - 64bits seg = 0 - AVL = 0 + limit part 2
dw 0xffff 	; limit bits 0 - 15
dw 0x00 	; base bits 0-15
db 0x00 	; base bits 16 - 23
db 10011010b; flags part 1
db 11001111b; flags part 2 + limit bits 16-19
db 0x00 	; base bits 24-31

gdt_data:
; data descriptor
; base 0x00
; limit 0xffffff ( max ) 
; flags part 1 : present = 1  - privilege = 00 - descriptor type = 1 - code = 0 - conforming = 0 - readable = 1 - accessed = 0
; => 10011010
; flags part 2 : granularity = 1 - 32 bits default = 1 - 64bits seg = 0 - AVL = 0 + limit part 2
dw 0xffff 	; limit bits 0 - 15
dw 0x00 	; base bits 0-15
db 0x00 	; base bits 16 - 23
db 10010010b; flags part 1
db 11001111b; flags part 2 + limit bits 16-19
db 0x00 	; base bits 24-31

gdt_end:
; end of gdt
; allows nasm to compute size

; ********************** ;
; gdt descriptor		 ;
; ********************** ;
gdt_descriptor:
; size of the gdt
; adress of the gdt
dw gdt_end - gdt_start - 1 	; size of the gdt
dd gdt_start				; address of the gdt

; offsets of segments in the GDT
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start