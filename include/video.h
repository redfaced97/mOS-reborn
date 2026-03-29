#ifndef VGA_H
#define VGA_H

#include "stdint.h"

void vga_init(void);
void vga_clear(void);

void printk(const char *s);
void printf(const char *fmt, ...);

void panic(const char *msg);

#endif