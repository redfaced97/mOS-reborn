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

    /* Drivers for devices */
    #include <drivers/fs/vfs/vfs.h>
    #include <keyboard.h>

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

        keyboard_init();

        init_vfs();   // здесь монтируется рамдиск и создаются /dev, /proc

        // Показать содержимое корня (в т.ч. файлы из initrd)
        vfs_list(root_fs);

        // Показать содержимое /dev и /proc
        vfs_node_t *dev = vfs_open("/dev");
        if (dev) {
            printk("/dev contents:\n");
            vfs_list(dev);
        }



        // Создаём /proc/uptime
        uint32_t test_read(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buf) {
            // параметр node не используется, это нормально для заглушки
            (void)node;
            char *str = "uptime: 12345 seconds\n";
            uint32_t len = strlen(str);
            if (offset >= len) return 0;
            if (offset + size > len) size = len - offset;
            memcpy(buf, str + offset, size);
            return size;
        }

        vfs_node_t *proc = vfs_open("/proc");   // <-- объявляем proc здесь
        if (proc) {
            vfs_node_t *uptime = vfs_create_node("uptime", VFS_FILE, proc);
            uptime->read = test_read;
            vfs_add_node(proc, uptime);
        }

        if (proc) {
            printk("/proc contents:\n");
            vfs_list(proc);
        }

        vfs_node_t *f = vfs_open("/proc/uptime");
        if (f) {
            char buf[64];
            uint32_t n = vfs_read(f, 0, sizeof(buf)-1, (uint8_t*)buf);
            buf[n] = '\0';
            printk("Content: %s", buf);
        }

        while(1) {
            __asm__ __volatile__("hlt");
        }

    }