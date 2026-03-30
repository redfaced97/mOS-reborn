#include "stddef.h"
#include "stdint.h"

#include "io.h"
#include "multiboot.h"
#include "time.h"
#include "memory.h"

#define HIGH_MEM_KB     (*(uint16_t *)0x90002)

extern uint64_t startup_time;
struct tm t;

static uint32_t memory_end = 0;
static uint32_t buffer_memory_end = 0;
static uint32_t main_memory_start = 0;

extern void panic(const char *msg);

/* --- Точка входа в init ядра --- */
void kinit(uint32_t magic, uint32_t addr) {
    uint16_t *video = (uint16_t *)0xB8000;

    multiboot_init(magic, addr);

    get_system_time(&t);
    startup_time = unix_time(&t);

    uint16_t root_dev = 0x301;  // fallback: первый HDD, первый раздел

    multiboot_info_t *mbi = (multiboot_info_t *)addr;
    if (mbi->flags & (1 << 1)) { // MB_INFO_BOOTDEV
        uint32_t bd = mbi->boot_device;
        uint8_t bios_drive = (bd >> 24) & 0xFF;
        uint8_t part      = (bd >> 16) & 0xFF;

        if (bios_drive == 0x80) {
            root_dev = 0x301 + part;
        }
    }

    memory_end = (1 << 20) + (HIGH_MEM_KB << 10);
    memory_end &= 0xfffff000;
    if (memory_end > 16 * 1024 * 1024)
        memory_end = 16 * 1024 * 1024;

    if (memory_end > 12 * 1024 * 1024)
        buffer_memory_end = 4 * 1024 * 1024;
    else if (memory_end > 6 * 1024 * 1024)
        buffer_memory_end = 2 * 1024 * 1024;
    else
        buffer_memory_end = 1 * 1024 * 1024;

    main_memory_start = buffer_memory_end;


    memory_init(main_memory_start, memory_end);

    video[0] = 0x0F42;
    video[1] = 0x0F45;
    video[2] = 0x0A47;

    for (;;) {
        asm volatile ("hlt");
    }
}