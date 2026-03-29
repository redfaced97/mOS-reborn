bits 32

section .text
    align 4
    dd 0x1BADB002             ; Multiboot magic
    dd 0x03                   ; flags: ALIGN (1) + MEMINFO (2) = 3
    dd -(0x1BADB002 + 0x03)   ; checksum

global _start
extern kinit

_start:
    cli
    mov esp, stack_space
    push ebx          ; addr (multiboot_info)
    push eax          ; magic
    call kinit
    hlt

section .bss
resb 32768
stack_space: