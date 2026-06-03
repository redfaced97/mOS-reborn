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

irq_common:
    pusha           ; Сохраняем EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10        ; Загружаем сегмент данных ядра
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp            ; Передаем ОДИН указатель на структуру всех сохраненных регистров!
    call irq_handler_c
    add esp, 4          ; Очищаем стек от указателя

    pop gs
    pop fs
    pop es
    pop ds

    popa
    add esp, 4          ; Удаляем номер IRQ, запущенный макросом
    iret