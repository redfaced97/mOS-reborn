#include <gdt.h>

struct gdt_entry gdt[3];
struct gdt_ptr gp;


static void gdt_set_gate(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[i].base_low    = base & 0xFFFF;
    gdt[i].base_middle = (base >> 16) & 0xFF;
    gdt[i].base_high   = (base >> 24) & 0xFF;

    gdt[i].limit_low   = limit & 0xFFFF;
    gdt[i].granularity = (limit >> 16) & 0x0F;
    gdt[i].granularity |= gran & 0xF0;

    gdt[i].access = access;
}

void gdt_init(void) {
    gp.limit = sizeof(gdt) - 1;
    gp.base  = (uint32_t)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // code
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // data

    gdt_flush((uint32_t)&gp);

    printk("GDT initialized!\n");
}