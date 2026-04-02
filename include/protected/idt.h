#ifndef IDT_H
#define IDT_H

#include <std/stdint.h>
#include <std/string.h>
#include <vsprintf.h>

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern struct idt_entry idt[256];
extern struct idt_ptr idtp;

extern void load_idt(void);

void idt_init(void);
void set_idt_gate(int n, uint32_t handler);
void load_idt(void);

#endif