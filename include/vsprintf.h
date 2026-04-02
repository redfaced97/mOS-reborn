#ifndef VSPRINTF_H
#define VSPRINTF_H

#include "stdarg.h"
#include "io.h"

// форматирование
int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);

// вывод
void printk(const char *fmt, ...);

#endif