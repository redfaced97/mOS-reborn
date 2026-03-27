
#include "io.h"
#include "time.h"

extern u64 startup_time;

struct tm t;

static char videobuffer[1024];

/* ---[ Точка входа в инит ядра ]--- */
void kinit() {

    get_system_time(&t);
    startup_time = unix_time(&t);

    unsigned short* video = (unsigned short*)0xB8000;
    video[0] = 0x0F41;
    for (;;) {}
}