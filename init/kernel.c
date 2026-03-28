#include "stddef.h"
#include "stdint.h"

#include "multiboot.h"

#include "time.h"
#include "io.h"

extern uint64_t startup_time;
struct tm t;

/* ---[ Точка входа в инит ядра ]--- */
void kinit(uint32_t magic, uint32_t addr) {
    // Заполняем legacy-адреса 0x90002, 0x90080, 0x901FC из multiboot заголовка
    multiboot_init(magic, addr);

    get_system_time(&t);
    startup_time = unix_time(&t);

    unsigned short *video = (unsigned short *)0xB8000;
    video[0] = 0x0F41;

    for (;;) {}
}