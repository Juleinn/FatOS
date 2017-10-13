; ---------------------------------------------------------
; file printstring.asm                                    ;
; Anton Claes, 2017                                       ;
; contains routines for 16bits-based screen displaying    ;
; ---------------------------------------------------------

print_string:
  push ax
print_string_loop:
  mov ah, 0x0e          ; print char function
  mov al, [si]          ; source index should be set
  cmp al, 0             ; end of string
  je print_string_ret   ; quit routine
  int 0x10              ; print the letter
  inc si                ; go to next letter
  jmp print_string_loop ; loop
print_string_ret:
  pop ax                ; restore modified registers
  ret                   ; return from routine
