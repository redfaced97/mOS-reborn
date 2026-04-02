global load_idt
extern idt
extern idtp

section .data
idt_ptr:
    dw 256*8-1
    dd idt

section .text

load_idt:
    lidt [idtp]
    ret