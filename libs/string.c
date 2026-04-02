#include <std/string.h>

void *memcpy(void *dest, const void *src, size_t n)
{
    char *d = dest;
    const char *s = src;

    while (n--)
        *d++ = *s++;

    return dest;
}

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = s;

    while (n--)
        *p++ = (unsigned char)c;

    return s;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *a = s1;
    const unsigned char *b = s2;

    while (n--) {
        if (*a != *b)
            return *a - *b;
        a++; b++;
    }
    return 0;
}

size_t strlen(const char *s)
{
    const char *p = s;
    while (*p) p++;
    return p - s;
}

size_t strnlen(const char *s, size_t maxlen)
{
    size_t i = 0;
    while (i < maxlen && s[i])
        i++;
    return i;
}

char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    char *d = dest;

    while (n && *src) {
        *d++ = *src++;
        n--;
    }

    while (n--)
        *d++ = '\0';

    return dest;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n && *s1 && (*s1 == *s2)) {
        s1++; s2++; n--;
    }

    if (!n)
        return 0;

    return *(unsigned char*)s1 - *(unsigned char*)s2;
}