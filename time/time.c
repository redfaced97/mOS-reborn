#include <time.h>
#include <std/stdint.h>

// Системное время запуска
uint64_t startup_time;

// ЭтоВысокостныйГод?
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

uint64_t unix_time(struct tm *t) {
    static const int mdays[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    int y = t->year;
    int m = t->mon += 1;

    uint64_t total_days =
        (uint64_t)y * 365ULL
        + (uint64_t)((y + 3) / 4)
        - (uint64_t)((y + 69) / 100)
        + (uint64_t)((y + 369) / 400);

    for (int i = 0; i < m; i++)
        total_days += mdays[i];

    if (m > 1 && is_leap(y))
        total_days++;

    total_days += (t->day - 1);

    return total_days * 86400ULL +
           (uint64_t)t->hour * 3600ULL +
           (uint64_t)t->min * 60ULL +
           (uint64_t)t->sec;
}