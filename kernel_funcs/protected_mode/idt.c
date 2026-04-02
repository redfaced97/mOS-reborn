
#include <idt.h>

struct idt_entry idt[256];
struct idt_ptr idtp;

void set_idt_gate(int n, uint32_t handler) {
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = 0x08;
    idt[n].zero        = 0;
    idt[n].type_attr   = 0x8E;
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_init(void) {
    memset(idt, 0, sizeof(idt));

    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;

    load_idt();

    printk("IDT initialized!\n");
}