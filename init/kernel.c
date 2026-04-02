#include <stddef.h>
#include <stdint.h>

#include <multiboot.h>

#include <memory.h>

/* Protected mode x86 */
#include <protected.h>

#include <time.h>
#include "vsprintf.h"

extern uint64_t startup_time;
struct tm t;

void kinit(uint32_t magic, uint32_t addr) {

    multiboot_init(magic, addr);
    multiboot_info_t *mbi = (multiboot_info_t *)addr;

    uint16_t root_dev = mb_get_root(mbi);

    uint32_t LOW_MEM_KB  = mbi->mem_lower;
    uint32_t HIGH_MEM_KB = mbi->mem_upper;

    memory_system_init(LOW_MEM_KB, HIGH_MEM_KB);

    protected_mode();
    load_idt();

    get_system_time(&t);
    startup_time = unix_time(&t);

    printk("Startup time (UTS) %d\n", startup_time);



    __asm__ volatile ("sti");

    for (;;) {}
}