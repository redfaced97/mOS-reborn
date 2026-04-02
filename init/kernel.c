/* Standart files */
#include <std/stddef.h>
#include <std/stdint.h>

/* Basic init code */
#include <multiboot.h>
#include <memory.h>

/* Protected mode x86 */
#include <protected/protected.h>

/* Add-ons */
#include <time.h>
#include <vsprintf.h>
#include <kaddons.h>

extern uint64_t startup_time;   // Startup time in UTS
struct tm t;                    // Struct for time

void kinit(uint32_t magic, uint32_t addr) {

    multiboot_init(magic, addr);
    multiboot_info_t *mbi = (multiboot_info_t *)addr;

    uint16_t root_dev = mb_get_root(mbi);

    uint32_t LOW_MEM_KB  = mbi->mem_lower;
    uint32_t HIGH_MEM_KB = mbi->mem_upper;

    memory_system_init(LOW_MEM_KB, HIGH_MEM_KB);

    protected_mode();

    get_system_time(&t);
    startup_time = unix_time(&t);

    printk("Startup time (UTS) %d\n", startup_time);

    panic("End code in kernel.c");

}