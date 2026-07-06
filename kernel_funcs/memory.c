#include "memory.h"

#define PAGE_SIZE 4096
#define HEAP_SIZE (64 * 1024)   // 64 KB куча

/* ================= GLOBAL STATE ================= */

static uint32_t LOW_MEM_KB;
static uint32_t HIGH_MEM_KB;

static uint32_t memory_end;
static uint32_t buffer_memory_start;
static uint32_t buffer_memory_end;
static uint32_t main_memory_start;

static uint8_t *page_map;
static uint32_t total_pages;

/* --- Heap --- */
static uint32_t heap_start;
static uint32_t heap_end;

typedef struct free_block {
    uint32_t size;               // общий размер блока (включая заголовок)
    struct free_block *next;
} free_block_t;

static free_block_t *free_list = NULL;

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

    /* --- выделяем кучу из буферной области --- */
    heap_start = buffer_memory_start;
    heap_end   = heap_start + HEAP_SIZE;

    // если куча не помещается, двигаем buffer_memory_start
    if (heap_end <= buffer_memory_end) {
        buffer_memory_start = heap_end;   // остаток буфера после кучи
    }

    // Инициализируем кучу одним свободным блоком
    free_block_t *initial = (free_block_t*)heap_start;
    initial->size = HEAP_SIZE;
    initial->next = NULL;
    free_list = initial;

    printk("[Memory pager]\nSet up pages = %d\n", memory_get_pages_count());

    printk("RAM     : 0x%08x - 0x%08x (%u KB)\n",
          memory_get_main_start(), memory_get_memory_end(),
          (memory_get_memory_end() - memory_get_main_start()) / 1024);

    printk("BUFFER  : 0x%08x - 0x%08x (%u KB)\n",
          memory_get_buffer_start(), memory_get_buffer_end(),
          (memory_get_buffer_end() - memory_get_buffer_start()) / 1024);
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

/* ================= PAGE API ================= */

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

/* ================= KERNEL HEAP (kmalloc/kfree) ================= */

void *kmalloc(uint32_t size) {
    if (size == 0) return NULL;

    // Выравнивание по 4 байта, добавляем заголовок
    uint32_t total = (size + 3) & ~3;
    total += sizeof(free_block_t);
    // Минимальный размер блока
    if (total < sizeof(free_block_t) + 4)
        total = sizeof(free_block_t) + 4;

    free_block_t *prev = NULL;
    free_block_t *curr = free_list;

    while (curr) {
        if (curr->size >= total) {
            // Удаляем из списка
            if (prev)
                prev->next = curr->next;
            else
                free_list = curr->next;

            // Если остаток достаточно велик – делим блок
            if (curr->size - total >= sizeof(free_block_t) + 4) {
                free_block_t *new_block = (free_block_t*)((uint8_t*)curr + total);
                new_block->size = curr->size - total;
                new_block->next = free_list;
                free_list = new_block;
                curr->size = total;
            }

            return (void*)(curr + 1);   // указатель на данные (сразу за заголовком)
        }
        prev = curr;
        curr = curr->next;
    }

    return NULL;   // нет памяти
}

void kfree(void *ptr) {
    if (!ptr) return;

    free_block_t *block = (free_block_t*)ptr - 1;
    // Пока просто добавляем в начало списка (без объединения)
    block->next = free_list;
    free_list = block;
}