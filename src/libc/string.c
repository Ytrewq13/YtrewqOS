#include "string.h"

/* strings.c
 * Copyright Sam Whitehead, 2021
 * Last updated 2021-10-25
 */

int ffs(int i)
{
    int pos = 0;
    if (i == 0) return 0;
    while (!(i & 1 << pos)) pos++;
    return pos+1;
}

int strcasecmp(const char *s1, const char *s2)
{
    char a, b;
    while (*s1 != 0) {
        a = (*s1 >= 'A' && *s1 <= 'Z') ? *s1+0x20 : *s1;
        b = (*s2 >= 'A' && *s2 <= 'Z') ? *s2+0x20 : *s2;
        if (a < b) return -1;
        if (a > b) return 1;
        s1++; s2++;
    }
    return 0;
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    size_t i;
    char a, b;
    i = 0;
    while (*s1 != 0 && i < n) {
        a = (*s1 >= 'A' && *s1 <= 'Z') ? *s1+0x20 : *s1;
        b = (*s2 >= 'A' && *s2 <= 'Z') ? *s2+0x20 : *s2;
        if (a < b) return -1;
        if (a > b) return 1;
        s1++; s2++; i++;
    }
    return 0;
}

/* Copy memory area */
void* memccpy(void* restrict dest, const void* restrict src, int c, size_t n)
{
    size_t i = 0;
    uint8_t *dest_ptr = dest;
    const uint8_t *src_ptr = src;
    while (*dest_ptr != 0 && i++ < n && *dest_ptr != c)
        *dest_ptr++ = *src_ptr++;
    return dest;
}

/* Scan memory for a character */
void *memchr(const void *s, int c, size_t n)
{
    // TODO
    return NULL;
}

/* Compare memory areas */
int memcmp(const void *s1, const void *s2, size_t n)
{
    size_t i = 0;
    const uint8_t *b1 = s1;
    const uint8_t *b2 = s2;
    while (i < n && *b1 == *b2) {
        if (*b1 < *b2) return -1;
        if (*b1 > *b2) return 1;
        b1++; b2++; i++;
    }
    return 0;
}

/* Copy memory area */
void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
    size_t i = 0;
    uint8_t *dest_ptr = dest;
    const uint8_t *src_ptr = src;
    while (i++ < n)
        *dest_ptr++ = *src_ptr++;
    return dest;
}

/* Copy memory area */
void *memmove(void *dest, const void *src, size_t n)
{
    // TODO: verify this works
    if (dest + n > src) {
    //  1: overwrites the beginning of src - no problem
    memcpy(dest, src, n);
    } else if (src + n > dest) {
    //  2: overwrites the end of src
    memcpy(dest-(src-dest), dest, n-(dest-src));
    memcpy(dest, src, dest-src);
    }
    return dest;
}


/* Fill memory with a constant byte */
void *memset(void *s, int c, size_t n)
{
    uint8_t *ptr = s;
    if (c > 0xff) return NULL; // c can't be larger than 1 byte
    while (n-- > 0) *ptr++ = c;
    return s;
}

/* Copy a string returning a pointer to its end. */
char *stpcpy(char *restrict dest, const char *restrict src)
{
    // TODO
    return NULL;
}

/* Copy a fixed-size string, returning a pointer to its end. */
char *stpncpy(char *restrict dest, const char *restrict src, size_t n)
{
    // TODO
    return NULL;
}

/* Concatenate two strings */
char *strcat(char *restrict dest, const char *restrict src)
{
    // TODO
    return NULL;
}

/* Locate character in string (first occurrence) */
char *strchr(const char *s, int c)
{
    // TODO
    return NULL;
}

/* Compare two strings */
int strcmp(const char *s1, const char *s2)
{
    while (*s1 != 0) {
        if (*s1 < *s2) return -1;
        if (*s1 > *s2) return 1;
        s1++; s2++;
    }
    return 0;
}

/* Compare two strings using the current locale. */
int strcoll(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

/* Copy a string */
char *strcpy(char *restrict dest, const char *restrict src)
{
    char *ptr = dest;
    while (*src != 0) *ptr++ = *src++;
    return dest;
}

/* Get length of a prefix substring. */
size_t strcspn(const char *s, const char *reject)
{
    // TODO
    return 0;
}

/* Return string describing error number. */
char *strerror(int errnum)
{
    // TODO
    return NULL;
}

/* Calculate the length of a string. */
size_t strlen(const char *s)
{
    size_t i = 0;
    while (*s++) i++;
    return i;
}

/* Concatenate two strings */
char *strncat(char *restrict dest, const char *restrict src, size_t n)
{
    // TODO
    return NULL;
}

/* Copy a string */
char *strncpy(char *restrict dest, const char *restrict src, size_t n)
{
    size_t i = 0;
    char *ptr = dest;
    while (*src != 0 && i++ < n) *ptr++ = *src++;
    return dest;
}

/* Duplicate a string */
char *strndup(const char *s, size_t n)
{
    // TODO
    return NULL;
}

/* Determine the length of a fixed-size string */
size_t strnlen(const char *s, size_t n)
{
    size_t i = 0;
    while (*s++ != 0 && i++ < n);
    return i;
}

/* Search a string for any of a set of bytes */
char *strpbrk(const char *s, const char *accept)
{
    // TODO
    return NULL;
}

/* Locate the last occurrence of character in string. */
char *strrchr(const char *s, int c)
{
    // TODO
    return NULL;
}

/* Return string describing signal. */
char *strsignal(int sig)
{
    // TODO
    return NULL;
}

/* Calculate the length of the starting segment in the string s that consists
 * entirely of bytes in accept. */
size_t strspn(const char *s, const char *accept)
{
    // TODO
    return 0;
}

/* Locate a substring */
char *strstr(const char *haystack, const char *needle)
{
    // TODO
    return NULL;
}

/* Extract tokens from strings */
char *strtok(char *restrict str, const char *restrict delim)
{
    // TODO
    return NULL;
}

/* Extract tokens from strings */
char* strtok_r(char* restrict str, const char* restrict delim,
               char** restrict saveptr)
{
    // TODO
    return NULL;
}

/* String transformation */
size_t strxfrm(char *restrict dest, const char *restrict src, size_t n)
{
    // TODO
    return 0;
}

