#include "memory.h"

#define PAGE_SIZE 4096

/* ================= GLOBAL STATE ================= */

static uint32_t LOW_MEM_KB;
static uint32_t HIGH_MEM_KB;

static uint32_t memory_end;
static uint32_t buffer_memory_start;
static uint32_t buffer_memory_end;
static uint32_t main_memory_start;

static uint8_t *page_map;
static uint32_t total_pages;

/* ================= SYSTEM INIT ================= */

void memory_system_init(uint32_t mem_lower_kb, uint32_t mem_upper_kb) {

    LOW_MEM_KB  = mem_lower_kb;
    HIGH_MEM_KB = mem_upper_kb;

    memory_end = (1 << 20) + (HIGH_MEM_KB << 10);
    memory_end &= 0xfffff000;

    buffer_memory_start = 1 * 1024 * 1024;

    if (memory_end > 12 * 1024 * 1024)
        buffer_memory_end = buffer_memory_start + 4 * 1024 * 1024;
    else if (memory_end > 6 * 1024 * 1024)
        buffer_memory_end = buffer_memory_start + 2 * 1024 * 1024;
    else
        buffer_memory_end = buffer_memory_start + 1 * 1024 * 1024;

    main_memory_start = buffer_memory_end;

    memory_init(main_memory_start, memory_end);

    printk("[Memory pager]\nSet up pages = %d\n", memory_get_pages_count());

    printk("RAM     : 0x%08x - 0x%08x (%u KB)\n",
          memory_get_main_start, memory_get_memory_end(), (memory_get_memory_end() - memory_get_main_start()) / 1024);

    printk("BUFFER  : 0x%08x - 0x%08x (%u KB)\n\n",
          memory_get_buffer_start(), memory_get_buffer_end(), (memory_get_buffer_end() - memory_get_buffer_start()) / 1024);
}

/* ================= BITMAP ALLOCATOR ================= */

void memory_init(uint32_t start, uint32_t end) {

    uint32_t mem_start = (start + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    uint32_t mem_end   = end & ~(PAGE_SIZE - 1);

    if (mem_end <= mem_start)
        return;

    uint32_t pages = (mem_end - mem_start) / PAGE_SIZE;
    uint32_t bitmap_size = (pages + 7) / 8;

    page_map = (uint8_t*)mem_start;
    mem_start += bitmap_size;

    total_pages = (mem_end - mem_start) / PAGE_SIZE;

    memset(page_map, 0, bitmap_size);
}

/* ================= BIT OPS ================= */

static inline int test_bit(uint32_t i) {
    return page_map[i >> 3] & (1 << (i & 7));
}

static inline void set_bit(uint32_t i) {
    page_map[i >> 3] |= (1 << (i & 7));
}

static inline void clear_bit(uint32_t i) {
    page_map[i >> 3] &= ~(1 << (i & 7));
}

static inline uint32_t idx(uint32_t phys) {
    return (phys - main_memory_start) / PAGE_SIZE;
}

static inline int valid(uint32_t phys) {
    return (phys >= main_memory_start && phys < memory_end);
}

/* ================= API ================= */

uint32_t page_alloc(void) {
    for (uint32_t i = 0; i < total_pages; i++) {
        if (!test_bit(i)) {
            set_bit(i);
            return main_memory_start + i * PAGE_SIZE;
        }
    }
    return 0;
}

void page_free(uint32_t phys) {
    if (!valid(phys)) return;
    clear_bit(idx(phys));
}

uint32_t page_alloc_contiguous(uint32_t count) {
    if (count == 0 || count > total_pages)
        return 0;

    for (uint32_t i = 0; i <= total_pages - count; i++) {
        int ok = 1;

        for (uint32_t j = 0; j < count; j++) {
            if (test_bit(i + j)) {
                ok = 0;
                break;
            }
        }

        if (ok) {
            for (uint32_t j = 0; j < count; j++)
                set_bit(i + j);

            return main_memory_start + i * PAGE_SIZE;
        }
    }

    return 0;
}

/* ================= GETTERS ================= */

uint32_t memory_get_pages_count(void) {
    return total_pages;
}

uint32_t memory_get_memory_end(void) {
    return memory_end;
}

uint32_t memory_get_main_start(void) {
    return main_memory_start;
}

uint32_t memory_get_buffer_start(void) {
    return buffer_memory_start;
}

uint32_t memory_get_buffer_end(void) {
    return buffer_memory_end;
}