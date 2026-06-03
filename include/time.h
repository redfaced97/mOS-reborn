#ifndef TIME_H
#define TIME_H

#include <std/stddef.h>
#include <std/stdint.h>
#include <io.h>

#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

#define read_nvram(reg) ({ \
    outb(CMOS_ADDR, (reg)); \
    inb(CMOS_DATA); \
})

#define BCD_TO_BIN(val) (((val) & 0x0F) + (((val) >> 4) * 10))

struct tm {
    uint32_t sec;
    uint32_t min;
    uint32_t hour;
    uint32_t day;
    uint32_t mon;
    uint32_t year;
};


uint64_t unix_time(struct tm *t);
void get_system_time(struct tm *t);

#endif