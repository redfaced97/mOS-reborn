#ifndef IO_H
#define IO_H

#include "stdint.h"
#include "stddef.h"


static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

static inline void write_word(unsigned long phys, uint16_t val) {
    *(volatile uint16_t *)phys = val;
}

static inline void write_bytes(unsigned long phys, void *data, size_t size) {
    volatile uint8_t *dst = (uint8_t *)phys;
    uint8_t *src = (uint8_t *)data;
    for (size_t i = 0; i < size; i++)
        dst[i] = src[i];
}

#endif