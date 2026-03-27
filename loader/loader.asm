

bits 32  ; 32 битная директирова для компилятора

section .text
        align 4
	dd 0x1BADB002             ; MultiBoot заголовок
	dd 0x00                   ; Флаги
	dd - (0x1BADB002 + 0x00)  ; Контрольная сумма

global init
extern kinit

init:
  cli
  mov esp, stack_space
  call kinit
  hlt

section .bss

resb 8192
stack_space: