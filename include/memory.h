#ifndef MEMORY_H
#define MEMORY_H

#include "stdint.h"

#define PAGE_SIZE 4096

// Инициализация карты оперативки
void memory_init(uint32_t start, uint32_t end);

uint32_t page_alloc(void);      // Выделение одной странницы
void page_free(uint32_t phys);  // Очистка одной странницы

void page_set_shared(uint32_t phys); // Считать что страница шарная (Нужно для COW'a)
int page_is_shared(uint32_t phys);   // Проверить что странница шарная

uint32_t page_alloc_contiguous(uint32_t count);  // Выделить n-странниц памяти (под exec)

#endif