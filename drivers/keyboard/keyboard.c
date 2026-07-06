#include <keyboard.h>

#include <vsprintf.h>
#include <protected/irq.h>
#include <io.h>

// Массив перевода скан-кодов клавиатуры в читаемые символы (US раскладка)
static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

void keyboard_handler(uint32_t irq) {
    (void)irq;

    uint8_t scancode = inb(0x60);

    if (!(scancode & 0x80)) {
        char c = kbd_us[scancode];
        if (c) {
            printk("%c", c);
        }
    }
}


void keyboard_init(void) {
    irq_install_handler(1, keyboard_handler);
    printk("Keyboard handler registered on IRQ1.\n");
}
