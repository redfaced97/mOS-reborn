#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <std/stdint.h>

void keyboard_init(void);
void keyboard_handler(uint32_t irq);

#endif
