#include "memory.h"
#include "stdint.h"
#include "strings.h"

#define MAX_PAGES 4096   // 16MB / 4KB

static uint32_t mem_start;
static uint32_t mem_end;
static uint32_t total_pages;

static uint8_t page_map[MAX_PAGES];
static uint8_t page_flags[MAX_PAGES];

void memory_init(uint32_t start, uint32_t end) {
    // выравнивание
    mem_start = (start + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    mem_end   = end & ~(PAGE_SIZE - 1);

    if (mem_end <= mem_start)
        return;

    total_pages = (mem_end - mem_start) / PAGE_SIZE;

    if (total_pages > MAX_PAGES)
        total_pages = MAX_PAGES;

    memset(page_map, 0, total_pages);
    memset(page_flags, 0, total_pages);
}

static inline uint32_t idx(uint32_t phys) {
    return (phys - mem_start) / PAGE_SIZE;
}

static inline int valid(uint32_t phys) {
    return (phys >= mem_start && phys < mem_end);
}

uint32_t page_alloc(void) {
    for (uint32_t i = 0; i < total_pages; i++) {
        if (page_map[i] == 0) {
            page_map[i] = 1;
            page_flags[i] = 0;
            return mem_start + i * PAGE_SIZE;
        }
    }
    return 0;
}

void page_free(uint32_t phys) {
    if (!valid(phys)) return;

    uint32_t i = idx(phys);
    page_map[i] = 0;
    page_flags[i] = 0;
}

void page_set_shared(uint32_t phys) {
    if (!valid(phys)) return;

    uint32_t i = idx(phys);
    if (page_map[i])
        page_flags[i] = 1;
}

int page_is_shared(uint32_t phys) {
    if (!valid(phys)) return 0;

    return page_flags[idx(phys)];
}

uint32_t page_alloc_contiguous(uint32_t count) {
    if (count == 0 || count > total_pages)
        return 0;

    for (uint32_t i = 0; i <= total_pages - count; i++) {
        int ok = 1;

        for (uint32_t j = 0; j < count; j++) {
            if (page_map[i + j]) {
                ok = 0;
                break;
            }
        }

        if (ok) {
            for (uint32_t j = 0; j < count; j++) {
                page_map[i + j] = 1;
                page_flags[i + j] = 0;
            }
            return mem_start + i * PAGE_SIZE;
        }
    }

    return 0;
}