
#ifndef PIC_H
#define PIC_H

#include <std/stdint.h>
#include <io.h>
#include <vsprintf.h>

#define PIC1         0x20
#define PIC2         0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA    (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA    (PIC2+1)

#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01
#define ICW4_8086    0x01

void pic_remap(void);
void pic_eoi(int irq);
void pic_clear_mask(uint8_t irq);

#endif