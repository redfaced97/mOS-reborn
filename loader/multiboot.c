
#include <multiboot.h>
#include <io.h>


#define MB_FLAG_MEM       (1 << 0)
#define MB_FLAG_BOOTDEV   (1 << 1)
#define MB_FLAG_CMDLINE   (1 << 2)

typedef struct {
    uint8_t bios_drive;   // Чтение из BIOS диска аля 0x8*
    uint8_t partition;
    uint8_t subpartition;
    uint16_t root_dev;    // Под Linux (0x301)
} boot_info_t;

boot_info_t boot_info;

static uint16_t parse_root_from_cmdline(char *cmd) {
    if (!cmd) return 0;

    char *p = cmd;

    while (*p) {
        if (p[0]=='r' && p[1]=='o' && p[2]=='o' && p[3]=='t' && p[4]=='=') {
            p += 5;

            if (*p == '0' && (p[1]=='x' || p[1]=='X')) p += 2;

            uint16_t val = 0;

            while ((*p >= '0' && *p <= '9') ||
                   (*p >= 'a' && *p <= 'f') ||
                   (*p >= 'A' && *p <= 'F')) {

                char c = *p;

                if (c >= '0' && c <= '9') val = (val << 4) | (c - '0');
                else if (c >= 'a' && c <= 'f') val = (val << 4) | (c - 'a' + 10);
                else if (c >= 'A' && c <= 'F') val = (val << 4) | (c - 'A' + 10);

                p++;
            }

            return val;
        }
        p++;
    }

    return 0;
}

void multiboot_init(uint32_t magic, uint32_t addr) {

    if (magic != 0x2BADB002) {
        char* msg = "NO MULTIBOOT!";
        for (int i = 0; msg[i]; i++) {
            ((uint16_t*)0xB8000)[i] = (0x04 << 8) | msg[i];
        }
        __asm__ volatile ("hlt");
    }

    multiboot_info_t *mbi = (multiboot_info_t *)addr;

    if (mbi->flags & MB_FLAG_MEM) {
        write_word(0x90002, (uint16_t)mbi->mem_upper);
    }

    boot_info.bios_drive = 0;
    boot_info.partition  = 0;
    boot_info.subpartition = 0;
    boot_info.root_dev   = 0x301; // Режим failback

    // --- CMDLINE (приоритет выше) ---
    if (mbi->flags & MB_FLAG_CMDLINE) {
        uint16_t cmd_root = parse_root_from_cmdline((char*)mbi->cmdline);
        if (cmd_root)
            boot_info.root_dev = cmd_root;
    }

    if (mbi->flags & MB_FLAG_BOOTDEV) {
        uint32_t bd = mbi->boot_device;

        boot_info.bios_drive  = (bd >> 24) & 0xFF;
        boot_info.partition   = (bd >> 16) & 0xFF;
        boot_info.subpartition = (bd >> 8) & 0xFF;

        // Формирование DID (Drive ID)
        if (boot_info.bios_drive == 0x80) {
            boot_info.root_dev = 0x301 + boot_info.partition;
        }
    }

    printk(" < ==============================[ Kernel log ]============================== >\n\n");
    printk("Kernel boot successfully! Start initialization...\n");
    printk("cmdline: %s\n", mbi->cmdline);
}

uint16_t mb_get_root(multiboot_info_t *mbi) {
    uint16_t root_dev = 0x301;

    if (mbi->flags & (1 << 1)) {
        uint32_t bd = mbi->boot_device;
        uint8_t bios_drive = (bd >> 24) & 0xFF;
        uint8_t part = (bd >> 16) & 0xFF;

        if (bios_drive == 0x80) {
            root_dev = 0x301 + part;
        }
    }

    printk("root_dev: 0x%x\n\n", root_dev);

    return root_dev;
}