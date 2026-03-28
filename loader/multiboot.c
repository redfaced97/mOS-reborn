#include "multiboot.h"
#include "io.h"

#include "stddef.h"
#include "stdint.h"

void multiboot_init(uint32_t magic, uint32_t addr) {
    if (magic != MULTIBOOT_MAGIC)
        return;

    multiboot_info_t *mbi = (multiboot_info_t *)addr;

    if (mbi->flags & MB_INFO_MEMORY) {
        write_word(0x90002, (uint16_t)mbi->mem_upper);
    }

    unsigned short root_dev = 0x301;
    if (mbi->flags & MB_INFO_CMDLINE) {
        char *cmd = (char *)mbi->cmdline;
        char *p = cmd;
        while (*p) {
            if (p[0]=='r' && p[1]=='o' && p[2]=='o' && p[3]=='t' && p[4]=='=') {
                p += 5;
                if (*p == '0' && (p[1]=='x' || p[1]=='X')) p += 2;
                unsigned short val = 0;
                while ((*p >= '0' && *p <= '9') ||
                       (*p >= 'a' && *p <= 'f') ||
                       (*p >= 'A' && *p <= 'F')) {
                    char c = *p;
                    if (c >= '0' && c <= '9') val = (val << 4) | (c - '0');
                    else if (c >= 'a' && c <= 'f') val = (val << 4) | (c - 'a' + 10);
                    else if (c >= 'A' && c <= 'F') val = (val << 4) | (c - 'A' + 10);
                    p++;
                }
                root_dev = val;
                break;
            }
            p++;
        }
    }
    write_word(0x901FC, root_dev);

    if ((mbi->flags & MB_INFO_DRIVES) && mbi->drives_count > 0) {
        multiboot_drive_t *drv = (multiboot_drive_t *)mbi->drives_addr;
        for (uint32_t i = 0; i < mbi->drives_count; i++) {
            if (drv->drive_number == 0x80) {
                unsigned char data[4];
                data[0] = drv->drive_cylinders & 0xFF;
                data[1] = (drv->drive_cylinders >> 8) & 0xFF;
                data[2] = drv->drive_heads;
                data[3] = drv->drive_sectors;
                write_bytes(0x90080, data, 4);
                break;
            }
            drv = (multiboot_drive_t *)((char *)drv + drv->size);
        }
    } else {
        unsigned char zero[4] = {0};
        write_bytes(0x90080, zero, 4);
    }
}