
#include <vsprintf.h>

#define VGA_MEMORY ((volatile unsigned short*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define is_digit(c) ((c) >= '0' && (c) <= '9')

#define ZEROPAD  1
#define SIGN     2
#define PLUS     4
#define SPACE    8
#define LEFT     16
#define SMALL    32


static int cursor_x = 0;
static int cursor_y = 0;

static void move_cursor(int x, int y) {
    unsigned short pos = y * 80 + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, pos & 0xFF);

    outb(0x3D4, 0x0E);
    outb(0x3D5, (pos >> 8) & 0xFF);
}

static void scroll() {
    volatile unsigned short *video = VGA_MEMORY;

    // Сдвиг строк вверх
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            video[(y - 1) * VGA_WIDTH + x] =
                video[y * VGA_WIDTH + x];
        }
    }

    // Очистка последней строки
    for (int x = 0; x < VGA_WIDTH; x++) {
        video[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
            (0x07 << 8) | ' ';
    }
}

void vga_cursor(uint32_t state) {
    if (state) {
        outb(0x3D4, 0x0A);
        outb(0x3D5, 14);
        outb(0x3D4, 0x0B);
        outb(0x3D5, 15);
    } else {
        outb(0x3D4, 0x0A);
        outb(0x3D5, 0x20);
    }
}

static void vga_put_char(char c) {
    volatile unsigned short *video = VGA_MEMORY;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        video[cursor_y * 80 + cursor_x] = (0x07 << 8) | c;
        cursor_x++;
    }

    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= VGA_HEIGHT) {
        scroll();
        cursor_y = VGA_HEIGHT - 1;
    }

    move_cursor(cursor_x, cursor_y);
}

static void console_write(const char *s) {
    while (*s) {
        vga_put_char(*s++);
    }
}



static int skip_atoi(const char **s) {
    int i = 0;
    while (is_digit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

static char *number(char *str, long num, int base,
                    int width, int precision, int flags) {

    char tmp[32];
    const char *digits =
        (flags & SMALL) ?
        "0123456789abcdef" :
        "0123456789ABCDEF";

    int i = 0;
    unsigned long n;
    char sign = 0;
    char pad = (flags & ZEROPAD) ? '0' : ' ';

    if (flags & SIGN) {
        if (num < 0) {
            sign = '-';
            n = -num;
        } else {
            sign = (flags & PLUS) ? '+' :
                   (flags & SPACE) ? ' ' : 0;
            n = num;
        }
    } else {
        n = (unsigned long)num;
    }

    if (n == 0)
        tmp[i++] = '0';
    else {
        while (n) {
            tmp[i++] = digits[n % base];
            n /= base;
        }
    }

    if (i > precision)
        precision = i;

    width -= precision;
    if (sign) width--;

    if (!(flags & (ZEROPAD | LEFT)))
        while (width-- > 0)
            *str++ = ' ';

    if (sign)
        *str++ = sign;

    if (!(flags & LEFT))
        while (width-- > 0)
            *str++ = pad;

    while (i < precision--)
        *str++ = '0';

    while (i-- > 0)
        *str++ = tmp[i];

    while (width-- > 0)
        *str++ = ' ';

    return str;
}

int vsprintf(char *buf, const char *fmt, va_list args) {
    char *str = buf;

    while (*fmt) {
        if (*fmt != '%') {
            *str++ = *fmt++;
            continue;
        }

        int flags = 0;
        int width = -1;
        int precision = -1;

        fmt++;

        while (1) {
            if (*fmt == '-') flags |= LEFT;
            else if (*fmt == '+') flags |= PLUS;
            else if (*fmt == ' ') flags |= SPACE;
            else if (*fmt == '0') flags |= ZEROPAD;
            else break;
            fmt++;
        }

        if (is_digit(*fmt))
            width = skip_atoi(&fmt);

        if (*fmt == '*') {
            width = va_arg(args, int);
            fmt++;
        }

        if (*fmt == '.') {
            fmt++;
            if (is_digit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*')
                precision = va_arg(args, int);
        }

        switch (*fmt) {

        case 'c':
            *str++ = (char)va_arg(args, int);
            break;

        case 's': {
            char *s = va_arg(args, char *);
            while (*s)
                *str++ = *s++;
            break;
        }

        case 'd':
        case 'i':
            flags |= SIGN;
            /* fallthrough */
        case 'u':
            str = number(str, va_arg(args, long), 10,
                         width, precision, flags);
            break;

        case 'x':
            flags |= SMALL;
            /* fallthrough */
        case 'X':
            str = number(str, va_arg(args, long), 16,
                         width, precision, flags);
            break;

        case '%':
            *str++ = '%';
            break;

        default:
            *str++ = '%';
            *str++ = *fmt;
            break;
        }

        fmt++;
    }

    *str = '\0';
    return str - buf;
}

int sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    int r;

    va_start(args, fmt);
    r = vsprintf(buf, fmt, args);
    va_end(args);

    return r;
}

void printk(const char *fmt, ...) {
    char buf[512];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    console_write(buf);
}