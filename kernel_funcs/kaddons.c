#include <kaddons.h>


void panic(const char *fmt, ...) {
    va_list args;

    __asm__ volatile ("cli");

    printk("\n=== PANIC ===\n");

    va_start(args, fmt);
    printk(fmt, args);
    va_end(args);

    printk("\n");
    vga_cursor(0);

    for (;;) { __asm__ volatile ("hlt"); }

}