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
    switch (errnum) {
        case 1: return "Operation not permitted";
        case 2: return "No such file or directory";
        case 3: return "No such process";
        case 4: return "Interrupted system call";
        case 5: return "Input/output error";
        case 6: return "No such device or address";
        case 7: return "Argument list too long";
        case 8: return "Exec format error";
        case 9: return "Bad file descriptor";
        case 10: return "No child processes";
        case 11: return "Resource temporarily unavailable";
        case 12: return "Cannot allocate memory";
        case 13: return "Permission denied";
        case 14: return "Bad address";
        case 15: return "Block device required";
        case 16: return "Device or resource busy";
        case 17: return "File exists";
        case 18: return "Invalid cross-device link";
        case 19: return "No such device";
        case 20: return "Not a directory";
        case 21: return "Is a directory";
        case 22: return "Invalid argument";
        case 23: return "Too many open files in system";
        case 24: return "Too many open files";
        case 25: return "Inappropriate ioctl for device";
        case 26: return "Text file busy";
        case 27: return "File too large";
        case 28: return "No space left on device";
        case 29: return "Illegal seek";
        case 30: return "Read-only file system";
        case 31: return "Too many links";
        case 32: return "Broken pipe";
        case 33: return "Numerical argument out of domain";
        case 34: return "Numerical result out of range";
        case 35: return "Resource deadlock avoided";
        case 36: return "File name too long";
        case 37: return "No locks available";
        case 38: return "Function not implemented";
        case 39: return "Directory not empty";
        case 40: return "Too many levels of symbolic links";
        case 42: return "No message of desired type";
        case 43: return "Identifier removed";
        case 44: return "Channel number out of range";
        case 45: return "Level 2 not synchronised";
        case 46: return "Level 3 halted";
        case 47: return "Level 3 reset";
        case 48: return "Link number out of range";
        case 49: return "Protocol driver not attached";
        case 50: return "No CSI structure available";
        case 51: return "Level 2 halted";
        case 52: return "Invalid exchange";
        case 53: return "Invalid request descriptor";
        case 54: return "Exchange full";
        case 55: return "No anode";
        case 56: return "Invalid request code";
        case 57: return "Invalid slot";
        case 59: return "Bad font file format";
        case 60: return "Device not a stream";
        case 61: return "No data available";
        case 62: return "Timer expired";
        case 63: return "Out of streams resources";
        case 64: return "Machine is not on the network";
        case 65: return "Package not installed";
        case 66: return "Object is remote";
        case 67: return "Link has been severed";
        case 68: return "Advertise error";
        case 69: return "Srmount error";
        case 70: return "Communication error on send";
        case 71: return "Protocol error";
        case 72: return "Multihop attempted";
        case 73: return "RFS specific error";
        case 74: return "Bad message";
        case 75: return "Value too large for defined data type";
        case 76: return "Name not unique on network";
        case 77: return "File descriptor in bad state";
        case 78: return "Remote address changed";
        case 79: return "Can not access a needed shared library";
        case 80: return "Accessing a corrupted shared library";
        case 81: return ".lib section in a.out corrupted";
        case 82: return "Attempting to link in too many shared libraries";
        case 83: return "Cannot exec a shared library directly";
        case 84: return "Invalid or incomplete multibyte or wide character";
        case 85: return "Interrupted system call should be restarted";
        case 86: return "Streams pipe error";
        case 87: return "Too many users";
        case 88: return "Socket operation on non-socket";
        case 89: return "Destination address required";
        case 90: return "Message too long";
        case 91: return "Protocol wrong type for socket";
        case 92: return "Protocol not available";
        case 93: return "Protocol not supported";
        case 94: return "Socket type not supported";
        case 95: return "Operation not supported";
        case 96: return "Protocol family not supported";
        case 97: return "Address family not supported by protocol";
        case 98: return "Address already in use";
        case 99: return "Cannot assign requested address";
        case 100: return "Network is down";
        case 101: return "Network is unreachable";
        case 102: return "Network dropped connection on reset";
        case 103: return "Software caused connection abort";
        case 104: return "Connection reset by peer";
        case 105: return "No buffer space available";
        case 106: return "Transport endpoint is already connected";
        case 107: return "Transport endpoint is not connected";
        case 108: return "Cannot send after transport endpoint shutdown";
        case 109: return "Too many references: cannot splice";
        case 110: return "Connection timed out";
        case 111: return "Connection refused";
        case 112: return "Host is down";
        case 113: return "No route to host";
        case 114: return "Operation already in progress";
        case 115: return "Operation now in progress";
        case 116: return "Stale file handle";
        case 117: return "Structure needs cleaning";
        case 118: return "Not a XENIX named type file";
        case 119: return "No XENIX semaphores available";
        case 120: return "Is a named type file";
        case 121: return "Remote I/O error";
        case 122: return "Disk quota exceeded";
        case 123: return "No medium found";
        case 124: return "Wrong medium type";
        case 125: return "Operation cancelled";
        case 126: return "Required key not available";
        case 127: return "Key has expired";
        case 128: return "Key has been revoked";
        case 129: return "Key was rejected by service";
        case 130: return "Owner died";
        case 131: return "State not recoverable";
        case 132: return "Operation not possible due to RF-kill";
        case 133: return "Memory page has hardware error";
        default: return "Unknown error NNN"; // FIXME: use sprintf to replace "NNN" with errno
    }
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

/* Get a string from a wide character string. Just throws away the high bytes.
 * Writes one additional byte for the NULL terminator at the end of the copied
 * string. */
size_t str_from_wchar(char *restrict dest, const uint16_t *restrict src, size_t n)
{
    size_t x = 0;
    char *ptr = (char *)src;
    while (*ptr != 0 && x++ < n) {
        *dest++ = *ptr;
        ptr += 2;
    }
    *dest = 0;  /* Copy the null terminating byte. */
    return x;
}
