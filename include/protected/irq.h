#ifndef IRQ_H
#define IRQ_H

#include <std/stdint.h>

typedef void (*irq_handler_t)(uint32_t);

struct irq_context {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t irq_no;
    uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed));

void irq_init(void);
void irq_install_handler(uint32_t irq, irq_handler_t handler);
void irq_enable(uint32_t irq);
void irq_handler_c(struct irq_context *ctx);

#endif