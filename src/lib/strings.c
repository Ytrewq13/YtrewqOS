#include "strings.h"

/* strings.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-10-25
 */

void *memset(void *s, int c, size_t n)
{
    uint8_t *ptr = s;
    if (c >> 8) return NULL; // c can't be larger than 1 byte
    while (n-- > 0) *ptr++ = c;
    return s;
}
