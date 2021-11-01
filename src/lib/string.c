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
    // TODO
    return NULL;
}

/* Scan memory for a character */
void *memchr(const void *s, int c, size_t n)
{
    // TODO
    return NULL;
}

/**/
int memcmp(const void *s1, const void *s2, size_t n)
{
    // TODO
    return 0;
}

/**/
void *memcpy(void *restrict s1, const void *restrict s2, size_t n)
{
    // TODO
    return NULL;
}

/**/
void *memmove(void *dest, const void *src, size_t n)
{
    // TODO
    return NULL;
}


/**/
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

/**/
char *strcat(char *restrict dest, const char *restrict src)
{
    // TODO
    return NULL;
}

/**/
char *strchr(const char *s, int c)
{
    // TODO
    return NULL;
}

/**/
int strcmp(const char *s1, const char *s2)
{
    // TODO
    return 0;
}

/* Compare two strings using the current locale. */
int strcoll(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

/**/
char *strcpy(char *restrict dest, const char *restrict src)
{
    // TODO
    return NULL;
}

/* Get length of a prefix substring. */
size_t strcspn(const char *s, const char *reject)
{
    // TODO
    return 0;
}

/* Duplicate a string using malloc. */
char *strdup(const char *s)
{
    // TODO
    return NULL;
}

/* Return string describing error number. */
char *strerror(int errnum)
{
    // TODO
    return NULL;
}

/* Thread-safe version of strerror. */
int strerror_r(int errnum, char *buf, size_t buflen)
{
    // TODO
    return 0;
}

/* Calculate the length of a string. */
size_t strlen(const char *s)
{
    // TODO
    return 0;
}

/**/
char *strncat(char *restrict dest, const char *restrict src, size_t n)
{
    // TODO
    return NULL;
}

/**/
int strncmp(const char *s1, const char *s2, size_t n)
{
    // TODO
    return 0;
}

/**/
char *strncpy(char *restrict dest, const char *restrict src, size_t n)
{
    // TODO
    return NULL;
}

/**/
char *strndup(const char *s, size_t n)
{
    // TODO
    return NULL;
}

/**/
size_t strnlen(const char *s, size_t n)
{
    // TODO
    return 0;
}

/**/
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

/**/
char *strstr(const char *haystack, const char *needle)
{
    // TODO
    return NULL;
}

/**/
char *strtok(char *restrict str, const char *restrict delim)
{
    // TODO
    return NULL;
}

/**/
char* strtok_r(char* restrict str, const char* restrict delim,
               char** restrict saveptr)
{
    // TODO
    return NULL;
}

/**/
size_t strxfrm(char *restrict dest, const char *restrict src, size_t n)
{
    // TODO
    return 0;
}

