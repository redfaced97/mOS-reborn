#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include "stdint.h"
#include "vsprintf.h"

#define MULTIBOOT_MAGIC 0x2BADB002
#define MB_INFO_MEMORY  (1 << 0)
#define MB_INFO_CMDLINE (1 << 2)
#define MB_INFO_DRIVES  (1 << 7)

// Cтруктура multiboot
typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t drives_count;
} multiboot_info_t;

// Геометрия дисков
typedef struct {
    uint32_t size;
    uint8_t  drive_number;
    uint8_t  drive_mode;
    uint16_t drive_cylinders;
    uint8_t  drive_heads;
    uint8_t  drive_sectors;
} multiboot_drive_t;

void multiboot_init(uint32_t magic, uint32_t addr);
uint16_t mb_get_root(multiboot_info_t *mbi);

#endif