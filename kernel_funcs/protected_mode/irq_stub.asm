global irq0_stub
global irq1_stub
global irq2_stub
global irq3_stub
global irq4_stub
global irq5_stub
global irq6_stub
global irq7_stub
global irq8_stub
global irq9_stub
global irq10_stub
global irq11_stub
global irq12_stub
global irq13_stub
global irq14_stub
global irq15_stub

extern irq_handler_c

; =========================
; IRQ macro
; =========================
%macro IRQ 1
irq%1_stub:
    push dword %1
    jmp irq_common
%endmacro

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

; =========================
; Common handler
; =========================
irq_common:
    pusha

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10        ; kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Стек сейчас выглядит так:
    ; [esp]     -> gs
    ; ...
    ; [esp+16]  -> ds
    ; [esp+20]  -> pusha (edi...)
    ; ...
    ; [esp+...]
    ; [esp + 52] -> irq number (из push dword %1)

    mov eax, [esp + 52]

    push esp            ; esp (context pointer)
    push eax            ; irq number

    call irq_handler_c

    add esp, 8

    pop gs
    pop fs
    pop es
    pop ds

    popa
    add esp, 4          ; убрать irq номер из стека
    iret