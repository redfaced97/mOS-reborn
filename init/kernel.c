#include "stddef.h"
#include "stdint.h"

#include "io.h"

#include "multiboot.h"
#include "time.h"

#include "memory.h"

#define HIGH_MEM_KB     (*(uint16_t *)0x90002)
#define DRIVE_INFO      (*(struct drive_info *)0x90080)
#define MAIN_ROOT_DEV   (*(uint16_t *)0x901FC)

struct drive_info { char dummy[32]; };

extern uint64_t startup_time;
struct tm t;

static uint32_t memory_end = 0;
static uint32_t buffer_memory_end = 0;
static uint32_t main_memory_start = 0;


extern void panic(const char *msg);

/* ---[ Точка входа в инит ядра ]--- */
void kinit(uint32_t magic, uint32_t addr) {


    uint16_t *video = (uint16_t *)0xB8000;
    multiboot_init(magic, addr);

    //get_system_time(&t);
    //startup_time = unix_time(&t);

    memory_end = (1 << 20) + (HIGH_MEM_KB << 10);   // 1 MB + EXT RAM
    memory_end &= 0xfffff000;                       // Выравнивание по 4KB
    if (memory_end > 16 * 1024 * 1024)
        memory_end = 16 * 1024 * 1024;              // Ограничение на 16MB

    // Выбор размера буфферной части RAM
    if (memory_end > 12 * 1024 * 1024)
        buffer_memory_end = 4 * 1024 * 1024;
    else if (memory_end > 6 * 1024 * 1024)
        buffer_memory_end = 2 * 1024 * 1024;
    else
        buffer_memory_end = 1 * 1024 * 1024;

    // Адрес свободной памяти под процессы
    main_memory_start = buffer_memory_end;

    // Инфо о корневом диске
    //struct drive_info drive_info = DRIVE_INFO;
    //uint16_t root_dev = MAIN_ROOT_DEV;

    memory_init(main_memory_start, memory_end);

    video[0] = 0x0F41;

    for (;;) {
        asm volatile ("hlt");
    }
}

// void panic(const char *msg) {
//     color = 0x4F;

//     printk("\n=== KERNEL PANIC ===\n");
//     printk(msg);
//     printk("\nHALTED");

//     __asm__ volatile("cli");

//     while (1)
//         __asm__ volatile("hlt");
// }
