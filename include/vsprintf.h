#ifndef VSPRINTF_H
#define VSPRINTF_H

#include <std/stdarg.h>
#include <io.h>

// курсор
void vga_cursor(uint32_t state);

// форматирование
int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);

// вывод
void printk(const char *fmt, ...);

#endif