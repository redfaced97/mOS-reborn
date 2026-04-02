#ifndef IRQ_H
#define IRQ_H

#include <std/stdint.h>

typedef void (*irq_handler_t)(uint32_t);

void irq_init(void);
void irq_install_handler(uint32_t irq, irq_handler_t handler);
void irq_enable(uint32_t irq);
void irq_handler_c(uint32_t irq, uint64_t esp);

#endif