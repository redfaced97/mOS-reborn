
#include <protected/irq.h>
#include <protected/pic.h>

static irq_handler_t handlers[16] = {0};

void irq_install_handler(uint32_t irq, irq_handler_t handler) {
    if (irq < 16)
        handlers[irq] = handler;
}

void irq_handler_c(uint32_t irq, uint64_t esp) {
    (void)esp;

    if (irq < 16 && handlers[irq]) {
        handlers[irq](irq);
    }

    pic_eoi(irq);
}

void irq_enable(uint32_t irq) {
    if (irq < 16)
        pic_clear_mask(irq);
}