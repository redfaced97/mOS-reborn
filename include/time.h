#ifndef TIME_H
#define TIME_H

#include "types.h"
#include "io.h"

#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

#define read_nvram(reg) ({ \
    outb(CMOS_ADDR, (reg)); \
    inb(CMOS_DATA); \
})

#define BCD_TO_BIN(val) (((val) & 0x0F) + (((val) >> 4) * 10))

struct tm {
    int sec;
    int min;
    int hour;
    int day;
    int mon;
    int year;
};


void get_system_time(struct tm *t);
u64 unix_time(struct tm *t);

#endif