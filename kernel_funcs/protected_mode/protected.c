
#include <protected.h>

void protected_mode(void) {

    printk("[Protector]\n");

    gdt_init();
    idt_init();

    pic_remap();
    irq_init();

    printk("\nKernel run protected mode!\n\n");

}