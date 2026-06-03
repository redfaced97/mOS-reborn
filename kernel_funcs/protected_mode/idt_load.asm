global load_idt

section .text

load_idt:
    mov eax, [esp + 4]  ; Забираем (uint32_t)&idtp из Си кода
    lidt [eax]          ; Загружаем таблицу в процессор
    ret
