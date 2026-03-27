#include "time.h"


u64 startup_time;

static int is_leap(int y) {
    int year = y + 1970;
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

void get_system_time(struct tm *time) {
    int sec;

    do {
        sec = BCD_TO_BIN(read_nvram(0));

        time->sec  = sec;
        time->min  = BCD_TO_BIN(read_nvram(2));
        time->hour = BCD_TO_BIN(read_nvram(4));
        time->day  = BCD_TO_BIN(read_nvram(7));
        time->mon  = BCD_TO_BIN(read_nvram(8));
        time->year = BCD_TO_BIN(read_nvram(9));

    } while (sec != BCD_TO_BIN(read_nvram(0)));

    time->mon--;
}

u64 unix_time(struct tm *t)
{
    static const int mdays[] = {
        31,28,31,30,31,30,31,31,30,31,30,31
    };

    int y = t->year;
    int m = t->mon;

    u64 total_days =
        (u64)y * 365ULL
        + (u64)((y + 3) / 4)
        - (u64)((y + 69) / 100)
        + (u64)((y + 369) / 400);

    for (int i = 0; i < m; i++)
        total_days += mdays[i];

    if (m > 1 && is_leap(y))
        total_days++;

    total_days += (t->day - 1);

    return total_days * 86400ULL +
           (u64)t->hour * 3600ULL +
           (u64)t->min * 60ULL +
           (u64)t->sec;
}