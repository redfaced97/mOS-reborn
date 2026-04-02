#include <protected/idt.h>
#include <protected/irq.h>

extern void irq0_stub();
extern void irq1_stub();
extern void irq2_stub();
extern void irq3_stub();
extern void irq4_stub();
extern void irq5_stub();
extern void irq6_stub();
extern void irq7_stub();
extern void irq8_stub();
extern void irq9_stub();
extern void irq10_stub();
extern void irq11_stub();
extern void irq12_stub();
extern void irq13_stub();
extern void irq14_stub();
extern void irq15_stub();

void irq_init(void) {
    set_idt_gate(32, (uint32_t)irq0_stub);
    set_idt_gate(33, (uint32_t)irq1_stub);
    set_idt_gate(34, (uint32_t)irq2_stub);
    set_idt_gate(35, (uint32_t)irq3_stub);
    set_idt_gate(36, (uint32_t)irq4_stub);
    set_idt_gate(37, (uint32_t)irq5_stub);
    set_idt_gate(38, (uint32_t)irq6_stub);
    set_idt_gate(39, (uint32_t)irq7_stub);
    set_idt_gate(40, (uint32_t)irq8_stub);
    set_idt_gate(41, (uint32_t)irq9_stub);
    set_idt_gate(42, (uint32_t)irq10_stub);
    set_idt_gate(43, (uint32_t)irq11_stub);
    set_idt_gate(44, (uint32_t)irq12_stub);
    set_idt_gate(45, (uint32_t)irq13_stub);
    set_idt_gate(46, (uint32_t)irq14_stub);
    set_idt_gate(47, (uint32_t)irq15_stub);

    printk("IRQ list filled as placeholders!\n");
}