#ifndef PROTECTED_H
#define PROTECTED_H

#include <protected/gdt.h>
#include <protected/idt.h>
#include <protected/pic.h>
#include <protected/irq.h>
#include <vsprintf.h>

static inline void cli(void) {
    __asm__ volatile ("cli");
}

static inline void sti(void) {
    __asm__ volatile ("sti");
}

void protected_mode(void);

#endif