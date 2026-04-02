#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <string.h>
#include <vsprintf.h>


void memory_system_init(uint32_t mem_lower_kb, uint32_t mem_upper_kb);
void memory_init(uint32_t start, uint32_t end);

uint32_t page_alloc(void);
void page_free(uint32_t phys);

uint32_t page_alloc_contiguous(uint32_t count);

uint32_t memory_get_pages_count(void);

uint32_t memory_get_memory_end(void);
uint32_t memory_get_main_start(void);
uint32_t memory_get_buffer_start(void);
uint32_t memory_get_buffer_end(void);

#endif